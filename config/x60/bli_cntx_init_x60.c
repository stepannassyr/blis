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

#include <assert.h>

// no idea why it's missing
//extern void bli_cntx_init_x60_ref(cntx_t* cntx);

void bli_cntx_init_x60( cntx_t* cntx )
{
	blksz_t blkszs[ BLIS_NUM_BLKSZS ];

	bli_cntx_init_x60_ref( cntx );

	uint64_t vlen = bli_env_get_var("BLIS_RVV_OVERRIDE_VLEN", 0);
    assert(0 == (vlen % 8));
    if (0 == vlen)
    {
        // vsetvlmax
        __asm__(
                //"csrr %[vlen],vlenb\n\t"
                "vsetvli %[vlen], zero, e8, m1, ta, ma\n\t"
                : [vlen] "+r" (vlen)
                :
                :
           );

    }
    else
    {
        // override vlen
        __asm__(
                //"csrr %[vlen],vlenb\n\t"
                "vsetvli %[vlen], %[vlen], e8, m1, ta, ma\n\t"
                : [vlen] "+r" (vlen)
                :
                :
           );
    }

    const uint64_t mr_d = 2*vlen/sizeof(double);
    const uint64_t nr = 14;


    bli_cntx_set_ukrs
    (
      cntx,
      

      // 1
      BLIS_COPYV_KER, BLIS_DOUBLE, bli_dcopyv_x60,
      BLIS_AXPYV_KER, BLIS_DOUBLE, bli_daxpyv_x60,

      // 1m
      BLIS_PACKM_KER, BLIS_DOUBLE, bli_dpackm_x60,
      //BLIS_PACKM_KER, BLIS_DOUBLE, bli_dpackm_x60_2vx14,

      // 3
      // BLIS_GEMM_UKR, BLIS_FLOAT,    bli_sgemm_x60_2vx14_2u,
      BLIS_GEMM_UKR, BLIS_DOUBLE,   bli_dgemm_x60_2vx14_2u,
      // BLIS_GEMM_UKR, BLIS_SCOMPLEX, bli_cgemm_x60_2vx14_2u,
      // BLIS_GEMM_UKR, BLIS_DCOMPLEX, bli_zgemm_x60_2vx14_2u,

      BLIS_GEMMTRSM_L_UKR, BLIS_DOUBLE,   bli_dgemmtrsm_l_x60_2vx14,
      BLIS_GEMMTRSM_U_UKR, BLIS_DOUBLE,   bli_dgemmtrsm_u_x60_2vx14,
      BLIS_VA_END
    );

    //Update the context with storage preferences.
    bli_cntx_set_ukr_prefs
    (
      cntx,

      // level-3
      //BLIS_GEMM_UKR_ROW_PREF, BLIS_FLOAT,    FALSE,
      BLIS_GEMM_UKR_ROW_PREF, BLIS_DOUBLE,   FALSE,
      //BLIS_GEMM_UKR_ROW_PREF, BLIS_SCOMPLEX, FALSE,
      //BLIS_GEMM_UKR_ROW_PREF, BLIS_DCOMPLEX, FALSE,

      BLIS_GEMMTRSM_L_UKR_ROW_PREF, BLIS_DOUBLE,   FALSE,
      BLIS_GEMMTRSM_U_UKR_ROW_PREF, BLIS_DOUBLE,   FALSE,

      BLIS_VA_END
    );

    dim_t mc_f = bli_env_get_var("BLIS_OVERRIDE_MC_FACTOR", 20);
    dim_t nc_f = bli_env_get_var("BLIS_OVERRIDE_NC_FACTOR", 304);
    dim_t kc   = bli_env_get_var("BLIS_OVERRIDE_KC", 280);

    bli_blksz_init_easy( &blkszs[ BLIS_MR ],       -1,      mr_d,      -1,      -1 );
    bli_blksz_init_easy( &blkszs[ BLIS_NR ],       -1,        nr,      -1,      -1 );
    bli_blksz_init_easy( &blkszs[ BLIS_MC ],       -1, mc_f*mr_d,      -1,      -1 );
    bli_blksz_init_easy( &blkszs[ BLIS_KC ],       -1,        kc,      -1,      -1 );
    bli_blksz_init_easy( &blkszs[ BLIS_NC ],       -1,   nc_f*nr,      -1,      -1 );

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
