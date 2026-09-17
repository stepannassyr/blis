/*

   BLIS
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2014, The University of Texas at Austin

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name(s) of the copyright holder(s) nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

//#ifndef BLIS_FAMILY_H
//#define BLIS_FAMILY_H


// -- MEMORY ALLOCATION --------------------------------------------------------

#define BLIS_SIMD_MAX_NUM_REGISTERS 32
#define BLIS_SIMD_MAX_SIZE          512

#if defined(USE_LX2_ALLOCATOR)
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
  #define BLIS_MALLOC_POOL lx2_malloc
  #define BLIS_FREE_POOL lx2_free
#endif


//#endif

