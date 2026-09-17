/*
   lx2_malloc.h

   HBM-binding allocator for the LX2 node topology.

   LX2: 2 sockets, 608 cores, 16 CPU clusters of 38 cores.
        NUMA 0-15  = DDR  (~31.5 GiB each)
        NUMA 16-31 = HBM  (4 GiB each)
        HBM node for a core = 16 + core/38

   Intended as a drop-in for BLIS_MALLOC_POOL / BLIS_FREE_POOL. The
   signatures match BLIS's malloc_ft / free_ft exactly.

   Note on BLIS integration: do NOT #include this header from
   bli_family_<config>.h. BLIS's flatten-headers pass tries to inline
   quoted includes into the monolithic blis.h. Declare the two prototypes
   directly in the family header instead:

       #if defined(USE_LX2_ALLOCATOR)
         #include <stddef.h>
         #ifdef __cplusplus
         extern "C" {
         #endif
         void* lx2_malloc( size_t size );
         void  lx2_free  ( void*  p    );
         #ifdef __cplusplus
         }
         #endif
         #define BLIS_MALLOC_POOL lx2_malloc
         #define BLIS_FREE_POOL   lx2_free
       #endif

   This header stays on disk for lx2_malloc.c itself and for standalone
   tests; it just never goes near the flattener.
*/

#ifndef LX2_MALLOC_H
#define LX2_MALLOC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Allocate `size` bytes.

   Two size classes:
     size >= LX2_MMAP_MIN (default 64 KiB)
         mmap, rounded up to a page, bound with mbind() to the HBM node
         local to the calling thread's current CPU. Page-aligned.
     size <  LX2_MMAP_MIN
         plain malloc, left on DDR. These are BLIS's apool control arrays
         (thrinfo_t / mem_t tables) -- they live in cache after first
         touch, so node placement buys nothing, and a 4 KiB mapping plus
         its own VMA per few-hundred-byte object is pure overhead at 608
         threads. Set LX2_MMAP_MIN=0 to force everything through mmap.

   Returns NULL on failure. */
void* lx2_malloc( size_t size );

/* Release a pointer previously returned by lx2_malloc; dispatches to
   munmap or free automatically. Passing anything else is a programming
   error and aborts (see LX2_STRICT_FREE in lx2_malloc.c). */
void  lx2_free( void* p );

/* --- Introspection, for tests and sanity checks --- */

/* HBM node that serves `cpu`, or -1 if cpu is out of range. */
int   lx2_hbm_node_for_cpu( int cpu );

/* NUMA node currently backing the page at `p`, or -1 if not determinable.
   Faults the page in if it is not resident. */
int   lx2_node_of_addr( const void* p );

/* All live blocks, both size classes. Either pointer may be NULL. */
void  lx2_stats( size_t* n_live, size_t* bytes_live );

/* Only the mmap'd, node-bound blocks -- i.e. how much of this cluster's
   4 GiB of HBM is currently committed. Either pointer may be NULL. */
void  lx2_stats_hbm( size_t* n_live, size_t* bytes_live );

#ifdef __cplusplus
}
#endif

#endif /* LX2_MALLOC_H */
