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

#include "blis.h"

#include "../../kernels/armsme/bli_sme_utils.h"

// Hot/cooldown split point for the pf_c kernel variants, in k-loop iterations
// from the end.  Read directly by the assembly (adrp/ldr), so it must be
// PC-relative -- hence the hidden visibility -- and must never be 0, or the
// cooldown loop counter underflows.
__attribute__((visibility("hidden")))
int32_t bli_armsme_pf_c_dist = 2;

// Map a BLIS_ARMSME_UKR_VARIANT value onto the built kernels.  Entries left
// NULL are bit patterns that are encodable but not built (pf policy 3); they
// fall back to variant 0.
#define ARMSME_NUM_VARIANTS 128


void bli_cntx_init_armsme( cntx_t* cntx )
{
	// Set default kernel blocksizes and functions.
	bli_cntx_init_armsme_ref( cntx );

	blksz_t blkszs[ BLIS_NUM_BLKSZS ];

	// -------------------------------------------------------------------------

    const dim_t svl_d = bli_sme_svl_bytes() / sizeof(double);
    const dim_t svl_s = bli_sme_svl_bytes() / sizeof(float);

	// Block size.
	const dim_t m_r_s = svl_s*2, n_r_s=svl_s*2;
	const dim_t m_r_d = svl_d*2, n_r_d=svl_d*4;

    const dim_t mr_in_mc_d = bli_env_get_var("BLIS_MR_IN_MC_D", 16);
    const dim_t mr_in_mc_s = bli_env_get_var("BLIS_MR_IN_MC_S", 16);
    const dim_t nr_in_nc_d = bli_env_get_var("BLIS_NR_IN_NC_D", 200);
    const dim_t nr_in_nc_s = bli_env_get_var("BLIS_NR_IN_NC_S", 200);
    const dim_t k_c_d = bli_env_get_var("BLIS_KC_D", 256);
    const dim_t k_c_s = bli_env_get_var("BLIS_KC_S", 256);


    const dim_t m_c_s = m_r_s * mr_in_mc_s;
    const dim_t m_c_d = m_r_d * mr_in_mc_d;

    const dim_t n_c_s = n_r_s * nr_in_nc_s;
    const dim_t n_c_d = n_r_d * nr_in_nc_d;



	// ---- runtime micro-kernel variant selection --------------------------
	// See bli_kernels_armsme.h for the bit layout of BLIS_ARMSME_UKR_VARIANT.
	static gemm_ukr_ft d_variants[ ARMSME_NUM_VARIANTS ] = {
		[   0 ] = bli_dgemm_armsme_2Vx4Vx4_v0,
		[   1 ] = bli_dgemm_armsme_2Vx4Vx4_v1,
		[   2 ] = bli_dgemm_armsme_2Vx4Vx4_v2,
		[   3 ] = bli_dgemm_armsme_2Vx4Vx4_v3,
		[   4 ] = bli_dgemm_armsme_2Vx4Vx4_v4,
		[   5 ] = bli_dgemm_armsme_2Vx4Vx4_v5,
		[   8 ] = bli_dgemm_armsme_2Vx4Vx4_v8,
		[   9 ] = bli_dgemm_armsme_2Vx4Vx4_v9,
		[  10 ] = bli_dgemm_armsme_2Vx4Vx4_v10,
		[  11 ] = bli_dgemm_armsme_2Vx4Vx4_v11,
		[  12 ] = bli_dgemm_armsme_2Vx4Vx4_v12,
		[  13 ] = bli_dgemm_armsme_2Vx4Vx4_v13,
		[  16 ] = bli_dgemm_armsme_2Vx4Vx4_v16,
		[  17 ] = bli_dgemm_armsme_2Vx4Vx4_v17,
		[  18 ] = bli_dgemm_armsme_2Vx4Vx4_v18,
		[  19 ] = bli_dgemm_armsme_2Vx4Vx4_v19,
		[  20 ] = bli_dgemm_armsme_2Vx4Vx4_v20,
		[  21 ] = bli_dgemm_armsme_2Vx4Vx4_v21,
		[  32 ] = bli_dgemm_armsme_2Vx4Vx4_v32,
		[  33 ] = bli_dgemm_armsme_2Vx4Vx4_v33,
		[  34 ] = bli_dgemm_armsme_2Vx4Vx4_v34,
		[  35 ] = bli_dgemm_armsme_2Vx4Vx4_v35,
		[  36 ] = bli_dgemm_armsme_2Vx4Vx4_v36,
		[  37 ] = bli_dgemm_armsme_2Vx4Vx4_v37,
		[  40 ] = bli_dgemm_armsme_2Vx4Vx4_v40,
		[  41 ] = bli_dgemm_armsme_2Vx4Vx4_v41,
		[  42 ] = bli_dgemm_armsme_2Vx4Vx4_v42,
		[  43 ] = bli_dgemm_armsme_2Vx4Vx4_v43,
		[  44 ] = bli_dgemm_armsme_2Vx4Vx4_v44,
		[  45 ] = bli_dgemm_armsme_2Vx4Vx4_v45,
		[  48 ] = bli_dgemm_armsme_2Vx4Vx4_v48,
		[  49 ] = bli_dgemm_armsme_2Vx4Vx4_v49,
		[  50 ] = bli_dgemm_armsme_2Vx4Vx4_v50,
		[  51 ] = bli_dgemm_armsme_2Vx4Vx4_v51,
		[  52 ] = bli_dgemm_armsme_2Vx4Vx4_v52,
		[  53 ] = bli_dgemm_armsme_2Vx4Vx4_v53,
		[  64 ] = bli_dgemm_armsme_2Vx4Vx4_v64,
		[  65 ] = bli_dgemm_armsme_2Vx4Vx4_v65,
		[  66 ] = bli_dgemm_armsme_2Vx4Vx4_v66,
		[  67 ] = bli_dgemm_armsme_2Vx4Vx4_v67,
		[  68 ] = bli_dgemm_armsme_2Vx4Vx4_v68,
		[  69 ] = bli_dgemm_armsme_2Vx4Vx4_v69,
		[  72 ] = bli_dgemm_armsme_2Vx4Vx4_v72,
		[  73 ] = bli_dgemm_armsme_2Vx4Vx4_v73,
		[  74 ] = bli_dgemm_armsme_2Vx4Vx4_v74,
		[  75 ] = bli_dgemm_armsme_2Vx4Vx4_v75,
		[  76 ] = bli_dgemm_armsme_2Vx4Vx4_v76,
		[  77 ] = bli_dgemm_armsme_2Vx4Vx4_v77,
		[  80 ] = bli_dgemm_armsme_2Vx4Vx4_v80,
		[  81 ] = bli_dgemm_armsme_2Vx4Vx4_v81,
		[  82 ] = bli_dgemm_armsme_2Vx4Vx4_v82,
		[  83 ] = bli_dgemm_armsme_2Vx4Vx4_v83,
		[  84 ] = bli_dgemm_armsme_2Vx4Vx4_v84,
		[  85 ] = bli_dgemm_armsme_2Vx4Vx4_v85,
		[  96 ] = bli_dgemm_armsme_2Vx4Vx4_v96,
		[  97 ] = bli_dgemm_armsme_2Vx4Vx4_v97,
		[  98 ] = bli_dgemm_armsme_2Vx4Vx4_v98,
		[  99 ] = bli_dgemm_armsme_2Vx4Vx4_v99,
		[ 100 ] = bli_dgemm_armsme_2Vx4Vx4_v100,
		[ 101 ] = bli_dgemm_armsme_2Vx4Vx4_v101,
		[ 104 ] = bli_dgemm_armsme_2Vx4Vx4_v104,
		[ 105 ] = bli_dgemm_armsme_2Vx4Vx4_v105,
		[ 106 ] = bli_dgemm_armsme_2Vx4Vx4_v106,
		[ 107 ] = bli_dgemm_armsme_2Vx4Vx4_v107,
		[ 108 ] = bli_dgemm_armsme_2Vx4Vx4_v108,
		[ 109 ] = bli_dgemm_armsme_2Vx4Vx4_v109,
		[ 112 ] = bli_dgemm_armsme_2Vx4Vx4_v112,
		[ 113 ] = bli_dgemm_armsme_2Vx4Vx4_v113,
		[ 114 ] = bli_dgemm_armsme_2Vx4Vx4_v114,
		[ 115 ] = bli_dgemm_armsme_2Vx4Vx4_v115,
		[ 116 ] = bli_dgemm_armsme_2Vx4Vx4_v116,
		[ 117 ] = bli_dgemm_armsme_2Vx4Vx4_v117
	};
	static gemm_ukr_ft s_variants[ ARMSME_NUM_VARIANTS ] = {
		[   0 ] = bli_sgemm_armsme_2Vx2Vx4_v0,
		[   1 ] = bli_sgemm_armsme_2Vx2Vx4_v1,
		[   2 ] = bli_sgemm_armsme_2Vx2Vx4_v2,
		[   3 ] = bli_sgemm_armsme_2Vx2Vx4_v3,
		[   4 ] = bli_sgemm_armsme_2Vx2Vx4_v4,
		[   5 ] = bli_sgemm_armsme_2Vx2Vx4_v5,
		[   8 ] = bli_sgemm_armsme_2Vx2Vx4_v8,
		[   9 ] = bli_sgemm_armsme_2Vx2Vx4_v9,
		[  10 ] = bli_sgemm_armsme_2Vx2Vx4_v10,
		[  11 ] = bli_sgemm_armsme_2Vx2Vx4_v11,
		[  12 ] = bli_sgemm_armsme_2Vx2Vx4_v12,
		[  13 ] = bli_sgemm_armsme_2Vx2Vx4_v13,
		[  16 ] = bli_sgemm_armsme_2Vx2Vx4_v16,
		[  17 ] = bli_sgemm_armsme_2Vx2Vx4_v17,
		[  18 ] = bli_sgemm_armsme_2Vx2Vx4_v18,
		[  19 ] = bli_sgemm_armsme_2Vx2Vx4_v19,
		[  20 ] = bli_sgemm_armsme_2Vx2Vx4_v20,
		[  21 ] = bli_sgemm_armsme_2Vx2Vx4_v21,
		[  32 ] = bli_sgemm_armsme_2Vx2Vx4_v32,
		[  33 ] = bli_sgemm_armsme_2Vx2Vx4_v33,
		[  34 ] = bli_sgemm_armsme_2Vx2Vx4_v34,
		[  35 ] = bli_sgemm_armsme_2Vx2Vx4_v35,
		[  36 ] = bli_sgemm_armsme_2Vx2Vx4_v36,
		[  37 ] = bli_sgemm_armsme_2Vx2Vx4_v37,
		[  40 ] = bli_sgemm_armsme_2Vx2Vx4_v40,
		[  41 ] = bli_sgemm_armsme_2Vx2Vx4_v41,
		[  42 ] = bli_sgemm_armsme_2Vx2Vx4_v42,
		[  43 ] = bli_sgemm_armsme_2Vx2Vx4_v43,
		[  44 ] = bli_sgemm_armsme_2Vx2Vx4_v44,
		[  45 ] = bli_sgemm_armsme_2Vx2Vx4_v45,
		[  48 ] = bli_sgemm_armsme_2Vx2Vx4_v48,
		[  49 ] = bli_sgemm_armsme_2Vx2Vx4_v49,
		[  50 ] = bli_sgemm_armsme_2Vx2Vx4_v50,
		[  51 ] = bli_sgemm_armsme_2Vx2Vx4_v51,
		[  52 ] = bli_sgemm_armsme_2Vx2Vx4_v52,
		[  53 ] = bli_sgemm_armsme_2Vx2Vx4_v53,
		[  64 ] = bli_sgemm_armsme_2Vx2Vx4_v64,
		[  65 ] = bli_sgemm_armsme_2Vx2Vx4_v65,
		[  66 ] = bli_sgemm_armsme_2Vx2Vx4_v66,
		[  67 ] = bli_sgemm_armsme_2Vx2Vx4_v67,
		[  68 ] = bli_sgemm_armsme_2Vx2Vx4_v68,
		[  69 ] = bli_sgemm_armsme_2Vx2Vx4_v69,
		[  72 ] = bli_sgemm_armsme_2Vx2Vx4_v72,
		[  73 ] = bli_sgemm_armsme_2Vx2Vx4_v73,
		[  74 ] = bli_sgemm_armsme_2Vx2Vx4_v74,
		[  75 ] = bli_sgemm_armsme_2Vx2Vx4_v75,
		[  76 ] = bli_sgemm_armsme_2Vx2Vx4_v76,
		[  77 ] = bli_sgemm_armsme_2Vx2Vx4_v77,
		[  80 ] = bli_sgemm_armsme_2Vx2Vx4_v80,
		[  81 ] = bli_sgemm_armsme_2Vx2Vx4_v81,
		[  82 ] = bli_sgemm_armsme_2Vx2Vx4_v82,
		[  83 ] = bli_sgemm_armsme_2Vx2Vx4_v83,
		[  84 ] = bli_sgemm_armsme_2Vx2Vx4_v84,
		[  85 ] = bli_sgemm_armsme_2Vx2Vx4_v85,
		[  96 ] = bli_sgemm_armsme_2Vx2Vx4_v96,
		[  97 ] = bli_sgemm_armsme_2Vx2Vx4_v97,
		[  98 ] = bli_sgemm_armsme_2Vx2Vx4_v98,
		[  99 ] = bli_sgemm_armsme_2Vx2Vx4_v99,
		[ 100 ] = bli_sgemm_armsme_2Vx2Vx4_v100,
		[ 101 ] = bli_sgemm_armsme_2Vx2Vx4_v101,
		[ 104 ] = bli_sgemm_armsme_2Vx2Vx4_v104,
		[ 105 ] = bli_sgemm_armsme_2Vx2Vx4_v105,
		[ 106 ] = bli_sgemm_armsme_2Vx2Vx4_v106,
		[ 107 ] = bli_sgemm_armsme_2Vx2Vx4_v107,
		[ 108 ] = bli_sgemm_armsme_2Vx2Vx4_v108,
		[ 109 ] = bli_sgemm_armsme_2Vx2Vx4_v109,
		[ 112 ] = bli_sgemm_armsme_2Vx2Vx4_v112,
		[ 113 ] = bli_sgemm_armsme_2Vx2Vx4_v113,
		[ 114 ] = bli_sgemm_armsme_2Vx2Vx4_v114,
		[ 115 ] = bli_sgemm_armsme_2Vx2Vx4_v115,
		[ 116 ] = bli_sgemm_armsme_2Vx2Vx4_v116,
		[ 117 ] = bli_sgemm_armsme_2Vx2Vx4_v117
	};

	dim_t ukr_variant = bli_env_get_var( "BLIS_ARMSME_UKR_VARIANT", 0 );
	if ( ukr_variant < 0 || ukr_variant >= ARMSME_NUM_VARIANTS ||
	     d_variants[ ukr_variant ] == NULL )
		ukr_variant = 0;

	const dim_t pf_c_dist = bli_env_get_var( "BLIS_ARMSME_PF_C_DIST", 2 );
	bli_armsme_pf_c_dist = ( int32_t )( pf_c_dist < 1 ? 1 : pf_c_dist );

	gemm_ukr_ft d_gemm_ukr = d_variants[ ukr_variant ];
	gemm_ukr_ft s_gemm_ukr = s_variants[ ukr_variant ];

	// Update the context with optimized native gemm micro-kernels.
	bli_cntx_set_ukrs
	(
	  cntx,

	  // level-3

	  BLIS_GEMM_UKR, BLIS_FLOAT,    s_gemm_ukr,
	  BLIS_GEMM_UKR, BLIS_DOUBLE,   d_gemm_ukr,

      // level-1m
	  BLIS_PACKM_KER, BLIS_FLOAT, bli_spackm_sve,
	  BLIS_PACKM_KER, BLIS_DOUBLE, bli_dpackm_sve,

	  BLIS_VA_END
	);

	// Update the context with storage preferences.
	bli_cntx_set_ukr_prefs
	(
	  cntx,

	  // level-3
	  BLIS_GEMM_UKR_ROW_PREF, BLIS_FLOAT,    TRUE,
	  BLIS_GEMM_UKR_ROW_PREF, BLIS_DOUBLE,   TRUE,

	  BLIS_VA_END
	);

	// Initialize level-3 blocksize objects with architecture-specific values.
	//                                           s      d      c      z
	bli_blksz_init_easy( &blkszs[ BLIS_MR ], m_r_s, m_r_d, -1, -1 );
	bli_blksz_init_easy( &blkszs[ BLIS_NR ], n_r_s, n_r_d, -1, -1 );
	bli_blksz_init_easy( &blkszs[ BLIS_MC ], m_c_s, m_c_d, -1, -1 );
	bli_blksz_init_easy( &blkszs[ BLIS_KC ], k_c_s, k_c_d, -1, -1 );
	bli_blksz_init_easy( &blkszs[ BLIS_NC ], n_c_s, n_c_d, -1, -1 );

	// Update the context with the current architecture's register and cache
	// blocksizes (and multiples) for native execution.
	bli_cntx_set_blkszs
	(
	  cntx,

	  // level-3
	  BLIS_NC, &blkszs[ BLIS_NC ], BLIS_NR,
	  BLIS_KC, &blkszs[ BLIS_KC ], BLIS_KR,
	  BLIS_MC, &blkszs[ BLIS_MC ], BLIS_MR,
	  BLIS_NR, &blkszs[ BLIS_NR ], BLIS_NR,
	  BLIS_MR, &blkszs[ BLIS_MR ], BLIS_MR,

	  BLIS_VA_END
	);
}

