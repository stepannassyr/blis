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



	// Update the context with optimized native gemm micro-kernels.
	bli_cntx_set_ukrs
	(
	  cntx,

	  // level-3

#if defined BLIS_ARMSME_USE_PF_KERNELS
	  BLIS_GEMM_UKR, BLIS_FLOAT,    bli_sgemm_armsme_2Vx2Vx4_pf,
	  BLIS_GEMM_UKR, BLIS_DOUBLE,   bli_dgemm_armsme_2Vx4Vx4_pf,
#else
	  BLIS_GEMM_UKR, BLIS_FLOAT,    bli_sgemm_armsme_2Vx2Vx4,
	  BLIS_GEMM_UKR, BLIS_DOUBLE,   bli_dgemm_armsme_2Vx4Vx4,
#endif

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

