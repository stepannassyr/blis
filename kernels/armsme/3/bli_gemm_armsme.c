/*=============================================================================
  bli_dgemm_armsme_2Vx4Vx4.c

  Glue for the SME dgemm micro-kernel.  MR = 2*SVL_D, NR = 4*SVL_D.

  The storage-variant branching (cs_c==1 / rs_c==1 / general) and the m<MR,
  n<NR edge handling both live in the .S file, so this wrapper is a shim.
  It exists only to (a) keep a C symbol in the BLIS build and (b) give you a
  one-flag escape hatch back to the CT-buffer edge handling while you are
  bringing the kernel up on hardware.

  Build this file and the .S with, e.g.
      -march=armv9-a+sme+sme-f64f64        (binutils < 2.39: +sme-f64)
  and add -DSME_FA64=1 to the .S *only* if HWCAP2_SME_FA64 is set on the
  target -- see the header comment in the .S.
=============================================================================*/

#include "blis.h"

#include "../bli_sme_utils.h"

/* 0 = kernel handles all edge cases itself (what you want)
   1 = fall back to BLIS's temp-C buffer for m<MR || n<NR                    */
#ifndef BLI_SME_UKR_USE_CT
#define BLI_SME_UKR_USE_CT 0
#endif


/* MR = 2*SVL_<dt>, NR = 2*nblk*SVL_<dt> */
dim_t bli_dgemm_armsme_2Vx4Vx4_mr( void )
{ return 2 * (dim_t)( bli_sme_svl_bytes() / sizeof( double ) ); }

dim_t bli_dgemm_armsme_2Vx4Vx4_nr( void )
{ return 4 * (dim_t)( bli_sme_svl_bytes() / sizeof( double ) ); }

dim_t bli_sgemm_armsme_2Vx2Vx4_mr( void )
{ return 2 * (dim_t)( bli_sme_svl_bytes() / sizeof( float ) ); }

dim_t bli_sgemm_armsme_2Vx2Vx4_nr( void )
{ return 2 * (dim_t)( bli_sme_svl_bytes() / sizeof( float ) ); }


extern void bli_sgemm_armsme_2Vx2Vx4
     (
       dim_t m, dim_t n, dim_t k,
       const void* alpha,
       const void* a,
       const void* b,
       const void* beta,
       void* c, inc_t rs_c, inc_t cs_c,
       const auxinfo_t* data, const cntx_t* cntx
     );

extern void bli_dgemm_armsme_2Vx4Vx4
     (
       dim_t m, dim_t n, dim_t k,
       const void* alpha,
       const void* a,
       const void* b,
       const void* beta,
       void* c, inc_t rs_c, inc_t cs_c,
       const auxinfo_t* data, const cntx_t* cntx
     );

#if BLI_SME_UKR_USE_CT

/* Bring-up fallback: let BLIS stage edge blocks through a temp buffer.
   Rename this to the registered symbol and rename the .S symbol to
   ..._asm if you need to switch to it.

   Careful: the CT buffer is MR*NR*8 == SVL_B^2 bytes, i.e. 4 KiB at
   SVL=512b but 16 KiB at SVL=1024b and 64 KiB at SVL=2048b.  You will have
   to raise BLIS_STACK_BUF_MAX_SIZE accordingly, which is one more reason to
   keep the tail kernel and leave this at 0.                                */
void bli_dgemm_armsme_2Vx4Vx4_ct
     (
       dim_t m, dim_t n, dim_t k,
       const void* alpha, const void* a, const void* b, const void* beta,
       void* c, inc_t rs_c0, inc_t cs_c0,
       const auxinfo_t* data, const cntx_t* cntx
     )
{
    const dim_t mr = bli_dgemm_armsme_2Vx4Vx4_mr();
    const dim_t nr = bli_dgemm_armsme_2Vx4Vx4_nr();

    uint64_t rs_c = rs_c0;
    uint64_t cs_c = cs_c0;

    GEMM_UKR_SETUP_CT_ANY( d, mr, nr, false )

    bli_dgemm_armsme_2Vx4Vx4( m, n, k, alpha, a, b, beta,
                                   c, rs_c, cs_c, data, cntx );

    GEMM_UKR_FLUSH_CT( d )
}

#endif


// prefetching variants

/* SVL is fixed for the life of the process; rdsvl per ukr call is wasteful.
   The race is benign -- every thread computes the same value. */
static dim_t bli_armsme_nr_d = 0;

void bli_dgemm_armsme_2Vx4Vx4_pf
     (
       dim_t m, dim_t n, dim_t k,
       const void* alpha, const void* a, const void* b, const void* beta,
       void* c, inc_t rs_c, inc_t cs_c,
       const auxinfo_t* data, const cntx_t* cntx
     )
{
    dim_t nr = bli_armsme_nr_d;
    if ( nr == 0 ) { nr = bli_dgemm_armsme_2Vx4Vx4_nr(); bli_armsme_nr_d = nr; }

    /* One line per page: warm the TLB and start the hardware streams.  A
       k_c x n_r panel is ~100 KiB at SVL=512b -- far too large to pull in
       with prfm, and that is not the goal. */
    const char*  bf    = bli_auxinfo_future_b( data );
    const size_t panel = ( size_t )k * ( size_t )nr * sizeof( double );

    if ( bf != NULL )
        for ( size_t off = 0; off < panel; off += 4096 )
            __builtin_prefetch( bf + off, 0, 3 );

    const char* af = bli_auxinfo_future_a( data );
    if ( af != NULL )
        __builtin_prefetch( af, 0, 3 );

    bli_dgemm_armsme_2Vx4Vx4( m, n, k, alpha, a, b, beta,
                              c, rs_c, cs_c, data, cntx );
}

/* SVL is fixed for the life of the process; rdsvl per ukr call is wasteful.
   The race is benign -- every thread computes the same value. */
static dim_t bli_armsme_nr_s = 0;

void bli_sgemm_armsme_2Vx2Vx4_pf
     (
       dim_t m, dim_t n, dim_t k,
       const void* alpha, const void* a, const void* b, const void* beta,
       void* c, inc_t rs_c, inc_t cs_c,
       const auxinfo_t* data, const cntx_t* cntx
     )
{
    dim_t nr = bli_armsme_nr_s;
    if ( nr == 0 ) { nr = bli_sgemm_armsme_2Vx2Vx4_nr(); bli_armsme_nr_s = nr; }

    /* One line per page: warm the TLB and start the hardware streams.  A
       k_c x n_r panel is ~100 KiB at SVL=512b -- far too large to pull in
       with prfm, and that is not the goal. */
    const char*  bf    = bli_auxinfo_future_b( data );
    const size_t panel = ( size_t )k * ( size_t )nr * sizeof( float );

    if ( bf != NULL )
        for ( size_t off = 0; off < panel; off += 4096 )
            __builtin_prefetch( bf + off, 0, 3 );

    const char* af = bli_auxinfo_future_a( data );
    if ( af != NULL )
        __builtin_prefetch( af, 0, 3 );

    bli_sgemm_armsme_2Vx2Vx4( m, n, k, alpha, a, b, beta,
                              c, rs_c, cs_c, data, cntx );
}
