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

GEMM_UKR_PROT( double,   d, gemm_armsme_2Vx4Vx4 )
GEMM_UKR_PROT( float,    s, gemm_armsme_2Vx2Vx4 )
GEMM_UKR_PROT( double,   d, gemm_armsme_2Vx4Vx4_pf )
GEMM_UKR_PROT( float,    s, gemm_armsme_2Vx2Vx4_pf )

// -----------------------------------------------------------------------------
// Prefetch variants, selected at runtime with BLIS_ARMSME_UKR_VARIANT.
//
// The variant number IS the configuration -- there is no lookup table to keep
// in sync.  Bit layout:
//
//     bit  0     future    0 = off, 1 = prefetch next A/B panel to L2
//     bits 1-2   pf_a      0 = off, 1 = PLDL1KEEP, 2 = PLDL1STRM, 3 = reserved
//     bits 3-4   pf_b      0 = off, 1 = PLDL1KEEP, 2 = PLDL1STRM, 3 = reserved
//     bit  5     pf_c      0 = off, 1 = PSTL1KEEP the C microtile between the
//                          hot and cooldown k-loops
//     bit  6     pf_cnext  0 = off, 1 = PSTL2KEEP the C microtile the NEXT ir
//                          iteration will use (c + MR*rs_c), issued before the
//                          k-loop so it has a whole ukr call of lead time
//
// pf_a / pf_b policy 3 (L2KEEP) is encodable but not built, so 72 of the 128
// patterns exist.  An unbuilt or out-of-range value falls back to variant 0,
// which is bit-identical to the kernel without any of this machinery.
//
// The pf_c split point is BLIS_ARMSME_PF_C_DIST (k-loop iterations before the
// end, clamped to >= 1, default 2) -- a separate knob so it does not multiply
// the variant count.
//
// Useful points:
//     0   nothing (baseline)
//     1   future only
//    12   A PLDL1STRM + B PLDL1KEEP
//    13   future + A PLDL1STRM + B PLDL1KEEP
//    44   A PLDL1STRM + B PLDL1KEEP + C
//    45   all four
//    64   next tile to L2 only
//    96   C to L1 (32) + next tile to L2 (64)
//   109   45 + next tile to L2
//
// NOTE: pf_b can only pay off once B_r = n_r * k_c * sizeof(dt) fits L1.  At
// the default k_c = 256 that is 64 KiB against a 32 KiB L1, so sweep
// BLIS_KC_D / BLIS_MR_IN_MC_D together with the variant.
// -----------------------------------------------------------------------------

GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v0   )  // future=0 a=off    b=off    c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v1   )  // future=1 a=off    b=off    c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v2   )  // future=0 a=l1keep b=off    c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v3   )  // future=1 a=l1keep b=off    c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v4   )  // future=0 a=l1strm b=off    c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v5   )  // future=1 a=l1strm b=off    c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v8   )  // future=0 a=off    b=l1keep c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v9   )  // future=1 a=off    b=l1keep c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v10  )  // future=0 a=l1keep b=l1keep c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v11  )  // future=1 a=l1keep b=l1keep c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v12  )  // future=0 a=l1strm b=l1keep c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v13  )  // future=1 a=l1strm b=l1keep c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v16  )  // future=0 a=off    b=l1strm c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v17  )  // future=1 a=off    b=l1strm c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v18  )  // future=0 a=l1keep b=l1strm c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v19  )  // future=1 a=l1keep b=l1strm c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v20  )  // future=0 a=l1strm b=l1strm c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v21  )  // future=1 a=l1strm b=l1strm c=0 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v32  )  // future=0 a=off    b=off    c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v33  )  // future=1 a=off    b=off    c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v34  )  // future=0 a=l1keep b=off    c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v35  )  // future=1 a=l1keep b=off    c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v36  )  // future=0 a=l1strm b=off    c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v37  )  // future=1 a=l1strm b=off    c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v40  )  // future=0 a=off    b=l1keep c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v41  )  // future=1 a=off    b=l1keep c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v42  )  // future=0 a=l1keep b=l1keep c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v43  )  // future=1 a=l1keep b=l1keep c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v44  )  // future=0 a=l1strm b=l1keep c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v45  )  // future=1 a=l1strm b=l1keep c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v48  )  // future=0 a=off    b=l1strm c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v49  )  // future=1 a=off    b=l1strm c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v50  )  // future=0 a=l1keep b=l1strm c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v51  )  // future=1 a=l1keep b=l1strm c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v52  )  // future=0 a=l1strm b=l1strm c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v53  )  // future=1 a=l1strm b=l1strm c=1 cnext=0
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v64  )  // future=0 a=off    b=off    c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v65  )  // future=1 a=off    b=off    c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v66  )  // future=0 a=l1keep b=off    c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v67  )  // future=1 a=l1keep b=off    c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v68  )  // future=0 a=l1strm b=off    c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v69  )  // future=1 a=l1strm b=off    c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v72  )  // future=0 a=off    b=l1keep c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v73  )  // future=1 a=off    b=l1keep c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v74  )  // future=0 a=l1keep b=l1keep c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v75  )  // future=1 a=l1keep b=l1keep c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v76  )  // future=0 a=l1strm b=l1keep c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v77  )  // future=1 a=l1strm b=l1keep c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v80  )  // future=0 a=off    b=l1strm c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v81  )  // future=1 a=off    b=l1strm c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v82  )  // future=0 a=l1keep b=l1strm c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v83  )  // future=1 a=l1keep b=l1strm c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v84  )  // future=0 a=l1strm b=l1strm c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v85  )  // future=1 a=l1strm b=l1strm c=0 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v96  )  // future=0 a=off    b=off    c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v97  )  // future=1 a=off    b=off    c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v98  )  // future=0 a=l1keep b=off    c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v99  )  // future=1 a=l1keep b=off    c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v100 )  // future=0 a=l1strm b=off    c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v101 )  // future=1 a=l1strm b=off    c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v104 )  // future=0 a=off    b=l1keep c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v105 )  // future=1 a=off    b=l1keep c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v106 )  // future=0 a=l1keep b=l1keep c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v107 )  // future=1 a=l1keep b=l1keep c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v108 )  // future=0 a=l1strm b=l1keep c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v109 )  // future=1 a=l1strm b=l1keep c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v112 )  // future=0 a=off    b=l1strm c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v113 )  // future=1 a=off    b=l1strm c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v114 )  // future=0 a=l1keep b=l1strm c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v115 )  // future=1 a=l1keep b=l1strm c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v116 )  // future=0 a=l1strm b=l1strm c=1 cnext=1
GEMM_UKR_PROT( double,  d, gemm_armsme_2Vx4Vx4_v117 )  // future=1 a=l1strm b=l1strm c=1 cnext=1

GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v0   )  // future=0 a=off    b=off    c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v1   )  // future=1 a=off    b=off    c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v2   )  // future=0 a=l1keep b=off    c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v3   )  // future=1 a=l1keep b=off    c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v4   )  // future=0 a=l1strm b=off    c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v5   )  // future=1 a=l1strm b=off    c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v8   )  // future=0 a=off    b=l1keep c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v9   )  // future=1 a=off    b=l1keep c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v10  )  // future=0 a=l1keep b=l1keep c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v11  )  // future=1 a=l1keep b=l1keep c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v12  )  // future=0 a=l1strm b=l1keep c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v13  )  // future=1 a=l1strm b=l1keep c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v16  )  // future=0 a=off    b=l1strm c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v17  )  // future=1 a=off    b=l1strm c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v18  )  // future=0 a=l1keep b=l1strm c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v19  )  // future=1 a=l1keep b=l1strm c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v20  )  // future=0 a=l1strm b=l1strm c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v21  )  // future=1 a=l1strm b=l1strm c=0 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v32  )  // future=0 a=off    b=off    c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v33  )  // future=1 a=off    b=off    c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v34  )  // future=0 a=l1keep b=off    c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v35  )  // future=1 a=l1keep b=off    c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v36  )  // future=0 a=l1strm b=off    c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v37  )  // future=1 a=l1strm b=off    c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v40  )  // future=0 a=off    b=l1keep c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v41  )  // future=1 a=off    b=l1keep c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v42  )  // future=0 a=l1keep b=l1keep c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v43  )  // future=1 a=l1keep b=l1keep c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v44  )  // future=0 a=l1strm b=l1keep c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v45  )  // future=1 a=l1strm b=l1keep c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v48  )  // future=0 a=off    b=l1strm c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v49  )  // future=1 a=off    b=l1strm c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v50  )  // future=0 a=l1keep b=l1strm c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v51  )  // future=1 a=l1keep b=l1strm c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v52  )  // future=0 a=l1strm b=l1strm c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v53  )  // future=1 a=l1strm b=l1strm c=1 cnext=0
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v64  )  // future=0 a=off    b=off    c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v65  )  // future=1 a=off    b=off    c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v66  )  // future=0 a=l1keep b=off    c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v67  )  // future=1 a=l1keep b=off    c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v68  )  // future=0 a=l1strm b=off    c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v69  )  // future=1 a=l1strm b=off    c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v72  )  // future=0 a=off    b=l1keep c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v73  )  // future=1 a=off    b=l1keep c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v74  )  // future=0 a=l1keep b=l1keep c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v75  )  // future=1 a=l1keep b=l1keep c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v76  )  // future=0 a=l1strm b=l1keep c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v77  )  // future=1 a=l1strm b=l1keep c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v80  )  // future=0 a=off    b=l1strm c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v81  )  // future=1 a=off    b=l1strm c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v82  )  // future=0 a=l1keep b=l1strm c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v83  )  // future=1 a=l1keep b=l1strm c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v84  )  // future=0 a=l1strm b=l1strm c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v85  )  // future=1 a=l1strm b=l1strm c=0 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v96  )  // future=0 a=off    b=off    c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v97  )  // future=1 a=off    b=off    c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v98  )  // future=0 a=l1keep b=off    c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v99  )  // future=1 a=l1keep b=off    c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v100 )  // future=0 a=l1strm b=off    c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v101 )  // future=1 a=l1strm b=off    c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v104 )  // future=0 a=off    b=l1keep c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v105 )  // future=1 a=off    b=l1keep c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v106 )  // future=0 a=l1keep b=l1keep c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v107 )  // future=1 a=l1keep b=l1keep c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v108 )  // future=0 a=l1strm b=l1keep c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v109 )  // future=1 a=l1strm b=l1keep c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v112 )  // future=0 a=off    b=l1strm c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v113 )  // future=1 a=off    b=l1strm c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v114 )  // future=0 a=l1keep b=l1strm c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v115 )  // future=1 a=l1keep b=l1strm c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v116 )  // future=0 a=l1strm b=l1strm c=1 cnext=1
GEMM_UKR_PROT( float,   s, gemm_armsme_2Vx2Vx4_v117 )  // future=1 a=l1strm b=l1strm c=1 cnext=1
