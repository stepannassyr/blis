/*
   lx2_malloc.c

   See lx2_malloc.h.

   Design notes
   ------------
   * mmap + mbind(MPOL_BIND) instead of memkind/jemalloc. No thread-specific
     data, no pthread_key destructors, no library destructor, so nothing runs
     during thread teardown or _dl_fini. That is the class of bug that makes
     libmemkind crash while OpenMP reaps its workers at exit.

   * mbind() and get_mempolicy() are issued as raw syscalls so the library
     needs no -lnuma on the link line. Nothing to change in make_defs.mk.

   * The mapping is bound *before* first touch. MPOL_BIND then governs which
     node each page comes from when it is faulted in. MPOL_MF_STRICT is
     deliberately not passed: it only validates pages that already exist, and
     a fresh anonymous mapping has none, so it would be a no-op here. The
     consequence is that under `bind`, exhausting the node's 4 GiB reaches
     the OOM killer at fault time rather than returning NULL. Use
     LX2_HBM_POLICY=preferred if you would rather spill to DDR.

   * Two size classes -- see the comment on lx2_malloc in the header. BLIS
     routes bli_apool's control arrays through BLIS_MALLOC_POOL alongside
     the packing buffers, and those are far too small to be worth a mapping.

   * Sizes and size class are tracked in a table rather than a header inside
     each block. Costs a lock on alloc/free (irrelevant: BLIS pool blocks are
     large and the pool caches them) and buys detection of mismatched frees.

   Environment
   -----------
   LX2_HBM_POLICY  bind (default) | preferred | off
                   bind      - hard bind to the cluster-local HBM node
                   preferred - spill to DDR when that node is full
                   off       - no mbind at all; plain first-touch. Use this
                               for DDR-vs-HBM A/B runs on one binary.
   LX2_HBM_NODE    force a specific node for every allocation, ignoring
                   sched_getcpu(). Use this with one rank per cluster.
   LX2_MMAP_MIN    byte threshold for the mmap path (default 65536).
   LX2_HUGEPAGE    1 (default) | 0 - whether to madvise(MADV_HUGEPAGE).
   LX2_VERBOSE     1 - log every allocation decision to stderr.
*/

/* Guarded: BLIS's make_defs.mk may already pass -D_GNU_SOURCE, which defines
   it as 1. Redefining it to empty here would be a redefinition warning, and
   an error under -Werror. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

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

/* Default mmap threshold. Packing buffers (m_c*k_c, k_c*n_c) are megabytes
   and land well above this; apool arrays are hundreds of bytes to a few KB
   and land below. */
#define LX2_MMAP_MIN_DEFAULT   ( 64 * 1024 )

/* Abort on a free of a pointer we never handed out. Set to 0 for a warning. */
#define LX2_STRICT_FREE 1

/* ------------------------------------------------------------------ */
/* Raw mempolicy syscalls                                              */
/* ------------------------------------------------------------------ */

#define LX2_MPOL_DEFAULT    0
#define LX2_MPOL_PREFERRED  1
#define LX2_MPOL_BIND       2

#define LX2_MPOL_F_NODE     ( 1 << 0 )
#define LX2_MPOL_F_ADDR     ( 1 << 1 )

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
static size_t         lx2_mmap_min   = LX2_MMAP_MIN_DEFAULT;
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

	if ( ( e = getenv( "LX2_MMAP_MIN" ) ) != NULL )
		lx2_mmap_min = ( size_t )strtoull( e, NULL, 0 );

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
/* pointer -> { length, size class } table                             */
/*                                                                     */
/* Open addressing, linear probing, backward-shift deletion, grows at   */
/* 50% load. Holds both page-aligned (mmap) and malloc-aligned keys, so */
/* the hash is a full 64-bit finalizer rather than one keyed on page    */
/* numbers. Its own storage comes from libc malloc, independent of the  */
/* blocks it tracks.                                                    */
/* ------------------------------------------------------------------ */

typedef struct
{
	void*  p;
	size_t len;     /* mmap length; 0 for the malloc class */
	int    mapped;  /* 1 = mmap + mbind, 0 = plain malloc */
} lx2_blk_t;

static lx2_blk_t*      lx2_tab        = NULL;
static size_t          lx2_cap        = 0;   /* power of two, or 0 */
static size_t          lx2_cnt        = 0;
static size_t          lx2_bytes      = 0;
static size_t          lx2_cnt_map    = 0;
static size_t          lx2_bytes_map  = 0;
static pthread_mutex_t lx2_mtx        = PTHREAD_MUTEX_INITIALIZER;

static size_t lx2_hash( const void* p )
{
	/* murmur3 fmix64 */
	uint64_t x = ( uint64_t )( uintptr_t )p;
	x ^= x >> 33;
	x *= 0xff51afd7ed558ccdULL;
	x ^= x >> 33;
	x *= 0xc4ceb9fe1a85ec53ULL;
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
static int lx2_tab_insert( void* p, size_t len, size_t bytes, int mapped )
{
	if ( lx2_cnt * 2 >= lx2_cap )
		if ( lx2_tab_grow() != 0 ) return -1;

	size_t m = lx2_cap - 1;
	size_t i = lx2_hash( p ) & m;

	while ( lx2_tab[ i ].p != NULL ) i = ( i + 1 ) & m;

	lx2_tab[ i ].p      = p;
	lx2_tab[ i ].len    = len;
	lx2_tab[ i ].mapped = mapped;

	lx2_cnt   += 1;
	lx2_bytes += bytes;

	if ( mapped ) { lx2_cnt_map += 1; lx2_bytes_map += bytes; }

	return 0;
}

/* caller holds lx2_mtx; returns 1 and fills *out if found, else 0 */
static int lx2_tab_remove( void* p, lx2_blk_t* out )
{
	if ( lx2_cap == 0 ) return 0;

	size_t m = lx2_cap - 1;
	size_t i = lx2_hash( p ) & m;

	while ( lx2_tab[ i ].p != NULL && lx2_tab[ i ].p != p )
		i = ( i + 1 ) & m;

	if ( lx2_tab[ i ].p == NULL ) return 0;

	*out = lx2_tab[ i ];

	/* Backward-shift deletion: pull up any entry whose ideal slot lies
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

	lx2_cnt -= 1;

	if ( out->mapped )
	{
		lx2_cnt_map   -= 1;
		lx2_bytes_map -= out->len;
		lx2_bytes     -= out->len;
	}

	return 1;
}

/* ------------------------------------------------------------------ */
/* Public entry points                                                 */
/* ------------------------------------------------------------------ */

static void* lx2_malloc_small( size_t size )
{
	void* p = malloc( size );

	if ( p == NULL ) return NULL;

	pthread_mutex_lock( &lx2_mtx );
	int rc = lx2_tab_insert( p, 0, 0, 0 );
	pthread_mutex_unlock( &lx2_mtx );

	if ( rc != 0 ) { free( p ); return NULL; }

	if ( lx2_verbose )
		fprintf( stderr, "lx2: alloc small  %p  %zu B (DDR, malloc)\n",
		         p, size );

	return p;
}

void* lx2_malloc( size_t size )
{
	pthread_once( &lx2_once, lx2_init );

	if ( size == 0 ) return NULL;

	if ( size < lx2_mmap_min ) return lx2_malloc_small( size );

	size_t len = ( size + lx2_pagesz - 1 ) & ~( lx2_pagesz - 1 );
	if ( len < size ) return NULL;   /* rounding overflowed */

	void* p = mmap( NULL, len, PROT_READ | PROT_WRITE,
	                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
	if ( p == MAP_FAILED ) return NULL;

	int cpu  = -1;
	int node = -1;

	if ( lx2_pol != LX2_POL_OFF )
	{
		cpu  = sched_getcpu();
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
	}

#ifdef MADV_HUGEPAGE
	if ( lx2_hugepage ) madvise( p, len, MADV_HUGEPAGE );
#endif

	pthread_mutex_lock( &lx2_mtx );
	int rc = lx2_tab_insert( p, len, len, 1 );
	pthread_mutex_unlock( &lx2_mtx );

	if ( rc != 0 ) { munmap( p, len ); return NULL; }

	if ( lx2_verbose )
		fprintf( stderr, "lx2: alloc mmap   %p  %zu B  cpu %4d -> node %3d\n",
		         p, len, cpu, node );

	return p;
}

void lx2_free( void* p )
{
	if ( p == NULL ) return;

	lx2_blk_t blk;

	pthread_mutex_lock( &lx2_mtx );
	int found = lx2_tab_remove( p, &blk );
	pthread_mutex_unlock( &lx2_mtx );

	if ( !found )
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

	if ( blk.mapped ) munmap( p, blk.len );
	else              free( p );
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

void lx2_stats_hbm( size_t* n_live, size_t* bytes_live )
{
	pthread_mutex_lock( &lx2_mtx );
	if ( n_live     ) *n_live     = lx2_cnt_map;
	if ( bytes_live ) *bytes_live = lx2_bytes_map;
	pthread_mutex_unlock( &lx2_mtx );
}
