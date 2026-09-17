/*
   lx2_malloc.h

   HBM-binding allocator for the LX2 node topology.

   LX2: 2 sockets, 608 cores, 16 CPU clusters of 38 cores.
        NUMA 0-15  = DDR  (~31.5 GiB each)
        NUMA 16-31 = HBM  (4 GiB each)
        HBM node for a core = 16 + core/38

   Intended as a drop-in for BLIS_MALLOC_POOL / BLIS_FREE_POOL:

       #include "lx2_malloc.h"
       #define BLIS_MALLOC_POOL lx2_malloc
       #define BLIS_FREE_POOL   lx2_free

   The signatures match BLIS's malloc_ft / free_ft exactly.

   This header is kept to <stddef.h> plus prototypes on purpose: BLIS's
   flatten-headers pass inlines it into the installed monolithic blis.h,
   so nothing here may pull in system headers or define feature macros.
*/

#ifndef LX2_MALLOC_H
#define LX2_MALLOC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Allocate `size` bytes, rounded up to a page, bound to the HBM node local
   to the calling thread's current CPU. Returns NULL on failure. The returned
   pointer is page-aligned. */
void* lx2_malloc( size_t size );

/* Release a pointer previously returned by lx2_malloc. Passing anything else
   is a programming error and aborts (see LX2_STRICT_FREE in lx2_malloc.c). */
void  lx2_free( void* p );

/* --- Introspection, for tests and sanity checks --- */

/* HBM node that serves `cpu`, or -1 if cpu is out of range. */
int   lx2_hbm_node_for_cpu( int cpu );

/* NUMA node currently backing the page at `p`, or -1 if not faulted in /
   not determinable. Faults the page in if it is not resident. */
int   lx2_node_of_addr( const void* p );

/* Live mapping count and total mapped bytes. Either pointer may be NULL. */
void  lx2_stats( size_t* n_live, size_t* bytes_live );

#ifdef __cplusplus
}
#endif

#endif /* LX2_MALLOC_H */
