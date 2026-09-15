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

/*-----------------------------------------------------------------------------
  cntx registration sketch -- in your bli_cntx_init_<arch>.c, after the
  runtime SME probe:

      const dim_t svl_d = bli_sme_svl_bytes() / sizeof( double );

      bli_cntx_set_ukrs( cntx,
        BLIS_GEMM_UKR, BLIS_DOUBLE, bli_dgemm_armsme_2Vx4Vx4,
        BLIS_VA_END );

      bli_cntx_set_ukr_prefs( cntx,
        BLIS_GEMM_UKR_ROW_PREF, BLIS_DOUBLE, TRUE,   // horizontal-slice store
        BLIS_VA_END );

      bli_blksz_init_easy( &blkszs[ BLIS_MR ], -1, 2*svl_d, -1, -1 );
      bli_blksz_init_easy( &blkszs[ BLIS_NR ], -1, 4*svl_d, -1, -1 );

  Runtime gate (Linux):
      unsigned long h2 = getauxval( AT_HWCAP2 );
      h2 & HWCAP2_SME          -> SME present
      h2 & HWCAP2_SME_F64F64   -> REQUIRED by the dgemm kernel (fmopa .d);
                                  the sgemm kernel needs only HWCAP2_SME
      h2 & HWCAP2_SME_FA64     -> only then may you build the .S with
                                  -DSME_FA64=1
-----------------------------------------------------------------------------*/
