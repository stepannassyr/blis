/*
   lx2_malloc.c

   See lx2_malloc.h.

   Design notes
   ------------
   * mmap + mbind(MPOL_BIND) instead of memkind/jemalloc. No thread-specific
     data, no pthread_key destructors, no library destructor, so nothing runs
     during thread teardown or _dl_fini. That is the class of bug that makes
     libmemkind crash in OpenMP worker reaping at exit.

   * mbind() and get_mempolicy() are issued as raw syscalls so the library
     needs no -lnuma on the link line. Nothing to change in make_defs.mk.

   * The mapping is bound *before* first touch. MPOL_BIND then governs which
     node each page comes from when it is faulted in. MPOL_MF_STRICT is
     deliberately not passed: it only validates pages that already exist, and
     a fresh anonymous mapping has none, so it would be a no-op here.

   * Sizes are tracked in a table rather than a header inside the mapping.
     Costs a lock on alloc/free (irrelevant: BLIS pool blocks are large and
     the pool caches them) and buys detection of mismatched frees.

   Environment
   -----------
   LX2_HBM_POLICY  bind (default) | preferred | off
                   bind      - hard bind; overflow OOMs rather than silently
                               spilling to DDR
                   preferred - spill to DDR when the HBM node is full
                   off       - no mbind at all; plain first-touch. Use this
                               for DDR-vs-HBM A/B runs.
   LX2_HBM_NODE    force a specific node for every allocation, ignoring
                   sched_getcpu(). Use this with one rank per cluster.
   LX2_HUGEPAGE    1 (default) | 0 - whether to madvise(MADV_HUGEPAGE).
   LX2_VERBOSE     1 - log each mapping's cpu -> node decision to stderr.
*/

#define _GNU_SOURCE

#include "lx2_malloc.h"

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

/* ------------------------------------------------------------------ */
/* LX2 topology. Hardcoded on purpose.                                 */
/* ------------------------------------------------------------------ */

#define LX2_CORES_PER_CLUSTER  38
#define LX2_NUM_CLUSTERS       16
#define LX2_HBM_NODE_BASE      16

/* Abort on a free of a pointer we never handed out. Set to 0 to make it a
   warning instead. */
#define LX2_STRICT_FREE 1

/* ------------------------------------------------------------------ */
/* Raw mempolicy syscalls                                              */
/* ------------------------------------------------------------------ */

#define LX2_MPOL_DEFAULT    0
#define LX2_MPOL_PREFERRED  1
#define LX2_MPOL_BIND       2

#define LX2_MPOL_F_NODE     (1 << 0)
#define LX2_MPOL_F_ADDR     (1 << 1)

#ifndef SYS_mbind
#define SYS_mbind __NR_mbind
#endif
#ifndef SYS_get_mempolicy
#define SYS_get_mempolicy __NR_get_mempolicy
#endif

static long lx2_sys_mbind( void* addr, unsigned long len, int mode,
                           const unsigned long* nodemask, unsigned long maxnode,
                           unsigned flags )
{
	return syscall( SYS_mbind, addr, len, mode, nodemask, maxnode, flags );
}

static long lx2_sys_get_mempolicy( int* mode, unsigned long* nodemask,
                                   unsigned long maxnode, void* addr,
                                   unsigned long flags )
{
	return syscall( SYS_get_mempolicy, mode, nodemask, maxnode, addr, flags );
}

/* ------------------------------------------------------------------ */
/* One-time configuration                                              */
/* ------------------------------------------------------------------ */

typedef enum { LX2_POL_BIND = 0, LX2_POL_PREFERRED, LX2_POL_OFF } lx2_pol_t;

static lx2_pol_t      lx2_pol        = LX2_POL_BIND;
static int            lx2_fixed_node = -1;
static int            lx2_hugepage   = 1;
static int            lx2_verbose    = 0;
static size_t         lx2_pagesz     = 4096;
static pthread_once_t lx2_once       = PTHREAD_ONCE_INIT;

static void lx2_init( void )
{
	long ps = sysconf( _SC_PAGESIZE );
	if ( ps > 0 ) lx2_pagesz = ( size_t )ps;

	const char* e;

	if ( ( e = getenv( "LX2_HBM_POLICY" ) ) != NULL )
	{
		if      ( strcmp( e, "bind"      ) == 0 ) lx2_pol = LX2_POL_BIND;
		else if ( strcmp( e, "preferred" ) == 0 ) lx2_pol = LX2_POL_PREFERRED;
		else if ( strcmp( e, "off"       ) == 0 ) lx2_pol = LX2_POL_OFF;
		else fprintf( stderr, "lx2: ignoring LX2_HBM_POLICY='%s'\n", e );
	}

	if ( ( e = getenv( "LX2_HBM_NODE" ) ) != NULL )
		lx2_fixed_node = atoi( e );

	if ( ( e = getenv( "LX2_HUGEPAGE" ) ) != NULL )
		lx2_hugepage = atoi( e );

	if ( ( e = getenv( "LX2_VERBOSE" ) ) != NULL )
		lx2_verbose = atoi( e );
}

int lx2_hbm_node_for_cpu( int cpu )
{
	if ( cpu < 0 ) return -1;

	int cluster = cpu / LX2_CORES_PER_CLUSTER;

	if ( cluster >= LX2_NUM_CLUSTERS ) return -1;

	return LX2_HBM_NODE_BASE + cluster;
}

/* ------------------------------------------------------------------ */
/* pointer -> length table                                             */
/*                                                                     */
/* Open addressing, linear probing, backward-shift deletion, grows at   */
/* 50% load. Keyed on page-aligned pointers. Uses libc malloc/free for  */
/* its own storage, which is independent of the mappings it tracks.     */
/* ------------------------------------------------------------------ */

typedef struct { void* p; size_t len; } lx2_blk_t;

static lx2_blk_t*      lx2_tab = NULL;
static size_t          lx2_cap = 0;   /* power of two, or 0 */
static size_t          lx2_cnt = 0;
static size_t          lx2_bytes = 0;
static pthread_mutex_t lx2_mtx = PTHREAD_MUTEX_INITIALIZER;

static size_t lx2_hash( const void* p )
{
	uint64_t x = ( uint64_t )( uintptr_t )p >> 12;   /* pages, not bytes */
	x ^= x >> 33;
	x *= 0xff51afd7ed558ccdULL;
	x ^= x >> 33;
	return ( size_t )x;
}

/* caller holds lx2_mtx */
static int lx2_tab_grow( void )
{
	size_t     newcap = ( lx2_cap == 0 ) ? 1024 : lx2_cap * 2;
	lx2_blk_t* newtab = ( lx2_blk_t* )calloc( newcap, sizeof( lx2_blk_t ) );

	if ( newtab == NULL ) return -1;

	lx2_blk_t* oldtab = lx2_tab;
	size_t     oldcap = lx2_cap;

	lx2_tab = newtab;
	lx2_cap = newcap;

	for ( size_t i = 0; i < oldcap; ++i )
	{
		if ( oldtab[ i ].p == NULL ) continue;

		size_t j = lx2_hash( oldtab[ i ].p ) & ( lx2_cap - 1 );
		while ( lx2_tab[ j ].p != NULL ) j = ( j + 1 ) & ( lx2_cap - 1 );
		lx2_tab[ j ] = oldtab[ i ];
	}

	free( oldtab );
	return 0;
}

/* caller holds lx2_mtx */
static int lx2_tab_insert( void* p, size_t len )
{
	if ( lx2_cnt * 2 >= lx2_cap )
		if ( lx2_tab_grow() != 0 ) return -1;

	size_t m = lx2_cap - 1;
	size_t i = lx2_hash( p ) & m;

	while ( lx2_tab[ i ].p != NULL ) i = ( i + 1 ) & m;

	lx2_tab[ i ].p   = p;
	lx2_tab[ i ].len = len;
	lx2_cnt   += 1;
	lx2_bytes += len;

	return 0;
}

/* caller holds lx2_mtx; returns the length, or 0 if p is not present */
static size_t lx2_tab_remove( void* p )
{
	if ( lx2_cap == 0 ) return 0;

	size_t m = lx2_cap - 1;
	size_t i = lx2_hash( p ) & m;

	while ( lx2_tab[ i ].p != NULL && lx2_tab[ i ].p != p )
		i = ( i + 1 ) & m;

	if ( lx2_tab[ i ].p == NULL ) return 0;

	size_t len = lx2_tab[ i ].len;

	/* Backward-shift deletion: move up any entry whose ideal slot lies
	   outside the cyclic interval (i, j], so probe chains stay intact. */
	size_t j = i;
	lx2_tab[ i ].p = NULL;

	for (;;)
	{
		j = ( j + 1 ) & m;

		if ( lx2_tab[ j ].p == NULL ) break;

		size_t k = lx2_hash( lx2_tab[ j ].p ) & m;
		int in_range = ( i <= j ) ? ( i < k && k <= j )
		                          : ( i < k || k <= j );
		if ( in_range ) continue;

		lx2_tab[ i ]   = lx2_tab[ j ];
		lx2_tab[ j ].p = NULL;
		i = j;
	}

	lx2_cnt   -= 1;
	lx2_bytes -= len;

	return len;
}

/* ------------------------------------------------------------------ */
/* Public entry points                                                 */
/* ------------------------------------------------------------------ */

void* lx2_malloc( size_t size )
{
	pthread_once( &lx2_once, lx2_init );

	if ( size == 0 ) return NULL;

	size_t len = ( size + lx2_pagesz - 1 ) & ~( lx2_pagesz - 1 );
	if ( len < size ) return NULL;   /* rounding overflowed */

	void* p = mmap( NULL, len, PROT_READ | PROT_WRITE,
	                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
	if ( p == MAP_FAILED ) return NULL;

	int node = -1;

	if ( lx2_pol != LX2_POL_OFF )
	{
		int cpu = sched_getcpu();

		node = ( lx2_fixed_node >= 0 ) ? lx2_fixed_node
		                               : lx2_hbm_node_for_cpu( cpu );

		if ( node >= 0 && node < 128 )
		{
			unsigned long mask[ 2 ] = { 0, 0 };
			mask[ node / 64 ] = 1UL << ( node % 64 );

			int mode = ( lx2_pol == LX2_POL_BIND ) ? LX2_MPOL_BIND
			                                       : LX2_MPOL_PREFERRED;

			/* maxnode 128 matches the two longs of storage above. */
			if ( lx2_sys_mbind( p, len, mode, mask, 128, 0 ) != 0 )
			{
				/* Not fatal: the mapping stays usable, just unbound. */
				if ( lx2_verbose )
					fprintf( stderr,
					         "lx2: mbind(%p, %zu, node %d) failed: %s\n",
					         p, len, node, strerror( errno ) );
				node = -1;
			}
		}

		if ( lx2_verbose )
			fprintf( stderr, "lx2: alloc cpu %4d -> node %3d  %p  %zu B\n",
			         cpu, node, p, len );
	}

#ifdef MADV_HUGEPAGE
	if ( lx2_hugepage ) madvise( p, len, MADV_HUGEPAGE );
#endif

	pthread_mutex_lock( &lx2_mtx );
	int rc = lx2_tab_insert( p, len );
	pthread_mutex_unlock( &lx2_mtx );

	if ( rc != 0 ) { munmap( p, len ); return NULL; }

	return p;
}

void lx2_free( void* p )
{
	if ( p == NULL ) return;

	pthread_mutex_lock( &lx2_mtx );
	size_t len = lx2_tab_remove( p );
	pthread_mutex_unlock( &lx2_mtx );

	if ( len == 0 )
	{
		fprintf( stderr,
		         "lx2_free: %p was not returned by lx2_malloc "
		         "(double free, or an allocator mismatch)\n", p );
#if LX2_STRICT_FREE
		abort();
#else
		return;
#endif
	}

	munmap( p, len );
}

int lx2_node_of_addr( const void* p )
{
	if ( p == NULL ) return -1;

	/* get_mempolicy with MPOL_F_ADDR reports the node of the page backing
	   the address, so the page has to exist. Touch it first. */
	( ( volatile char* )p )[ 0 ] = ( ( volatile char* )p )[ 0 ];

	int node = -1;

	if ( lx2_sys_get_mempolicy( &node, NULL, 0, ( void* )( uintptr_t )p,
	                            LX2_MPOL_F_NODE | LX2_MPOL_F_ADDR ) != 0 )
		return -1;

	return node;
}

void lx2_stats( size_t* n_live, size_t* bytes_live )
{
	pthread_mutex_lock( &lx2_mtx );
	if ( n_live     ) *n_live     = lx2_cnt;
	if ( bytes_live ) *bytes_live = lx2_bytes;
	pthread_mutex_unlock( &lx2_mtx );
}
