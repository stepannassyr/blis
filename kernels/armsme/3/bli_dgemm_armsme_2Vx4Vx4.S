/*=============================================================================
  bli_dgemm_armsme_2Vx4Vx4.S

  BLIS dgemm micro-kernel for Arm SME.   C := beta*C + alpha*A*B

      MR = 2*SVL_D, NR = 4*SVL_D, k-unroll 4, all eight .D tiles.

  Everything below the prologue/dispatch is generated from sme_ukr_macros.h;
  see that file for the tile layout, the register convention and the slice
  unrolling rules.

  Requires FEAT_SME + FEAT_SME_F64F64.  See the header for SME_FA64.

  PSTATE.SM / PSTATE.ZA ownership is decided by the BLIS_SME_HOIST_* build
  variant and handled entirely inside UKR_SME_INIT / UKR_SME_DEINIT.  Those
  two macros also own the d8-d15 spill and the rs_c / cs_c stack-argument
  loads, since the argument offsets depend on the spill size.

  ARG REGISTERS (BLIS gemm ukr signature)
    x0 m  x1 n  x2 k  x3 &alpha  x4 a  x5 b  x6 &beta  x7 c
    [sp,#0] rs_c  [sp,#8] cs_c  [sp,#16] data  [sp,#24] cntx
  After UKR_SME_INIT the same four sit at .LSME_SPILL + {0,8,16,24}.
=============================================================================*/

#ifndef SME_FA64
#define SME_FA64 0
#endif

#define UKR_NBLK    2       /* NR = 2*nblk*SVL                    */
#define UKR_NK      4       /* k-unroll                           */
#define UKR_NR_LOG2 2       /* NR = SVL << UKR_NR_LOG2            */

    /* binutils < 2.39 spells this "+sme-f64" */
    .arch   armv9-a+sme+sme-f64f64

#include "sme_ukr_macros.h"

    SME_DT_SETUP d

    .text

/* One instantiation per prefetch variant.  36 of the 64 bit patterns are
   built; the rest are reserved (pf policy 3 = L2KEEP for A/B).  See
   bli_kernels_armsme.h for the encoding and bli_cntx_init_armsme.c for
   the BLIS_ARMSME_UKR_VARIANT lookup. */

#define UKR_VARIANT 0
#define UKR_SUFFIX  v0
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 1
#define UKR_SUFFIX  v1
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 2
#define UKR_SUFFIX  v2
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 3
#define UKR_SUFFIX  v3
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 4
#define UKR_SUFFIX  v4
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 5
#define UKR_SUFFIX  v5
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 8
#define UKR_SUFFIX  v8
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 9
#define UKR_SUFFIX  v9
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 10
#define UKR_SUFFIX  v10
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 11
#define UKR_SUFFIX  v11
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 12
#define UKR_SUFFIX  v12
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 13
#define UKR_SUFFIX  v13
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 16
#define UKR_SUFFIX  v16
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 17
#define UKR_SUFFIX  v17
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 18
#define UKR_SUFFIX  v18
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 19
#define UKR_SUFFIX  v19
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 20
#define UKR_SUFFIX  v20
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 21
#define UKR_SUFFIX  v21
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 32
#define UKR_SUFFIX  v32
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 33
#define UKR_SUFFIX  v33
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 34
#define UKR_SUFFIX  v34
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 35
#define UKR_SUFFIX  v35
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 36
#define UKR_SUFFIX  v36
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 37
#define UKR_SUFFIX  v37
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 40
#define UKR_SUFFIX  v40
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 41
#define UKR_SUFFIX  v41
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 42
#define UKR_SUFFIX  v42
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 43
#define UKR_SUFFIX  v43
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 44
#define UKR_SUFFIX  v44
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 45
#define UKR_SUFFIX  v45
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 48
#define UKR_SUFFIX  v48
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 49
#define UKR_SUFFIX  v49
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 50
#define UKR_SUFFIX  v50
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 51
#define UKR_SUFFIX  v51
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 52
#define UKR_SUFFIX  v52
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 53
#define UKR_SUFFIX  v53
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 64
#define UKR_SUFFIX  v64
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 65
#define UKR_SUFFIX  v65
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 66
#define UKR_SUFFIX  v66
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 67
#define UKR_SUFFIX  v67
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 68
#define UKR_SUFFIX  v68
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 69
#define UKR_SUFFIX  v69
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 72
#define UKR_SUFFIX  v72
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 73
#define UKR_SUFFIX  v73
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 74
#define UKR_SUFFIX  v74
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 75
#define UKR_SUFFIX  v75
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 76
#define UKR_SUFFIX  v76
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 77
#define UKR_SUFFIX  v77
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 80
#define UKR_SUFFIX  v80
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 81
#define UKR_SUFFIX  v81
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 82
#define UKR_SUFFIX  v82
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 83
#define UKR_SUFFIX  v83
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 84
#define UKR_SUFFIX  v84
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 85
#define UKR_SUFFIX  v85
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 96
#define UKR_SUFFIX  v96
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 97
#define UKR_SUFFIX  v97
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 98
#define UKR_SUFFIX  v98
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 99
#define UKR_SUFFIX  v99
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 100
#define UKR_SUFFIX  v100
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 101
#define UKR_SUFFIX  v101
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 104
#define UKR_SUFFIX  v104
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 105
#define UKR_SUFFIX  v105
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 106
#define UKR_SUFFIX  v106
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 107
#define UKR_SUFFIX  v107
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 108
#define UKR_SUFFIX  v108
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 109
#define UKR_SUFFIX  v109
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 112
#define UKR_SUFFIX  v112
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 113
#define UKR_SUFFIX  v113
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 114
#define UKR_SUFFIX  v114
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 115
#define UKR_SUFFIX  v115
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 116
#define UKR_SUFFIX  v116
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

#define UKR_VARIANT 117
#define UKR_SUFFIX  v117
#include "bli_dgemm_armsme_2Vx4Vx4_body.h"
#undef UKR_SUFFIX
#undef UKR_VARIANT

/* Variant 0 is the plain kernel; keep the historical symbol as an alias so
   the _pf wrappers in bli_gemm_armsme.c and any external reference still
   resolve. */
    .global bli_dgemm_armsme_2Vx4Vx4
    .set    bli_dgemm_armsme_2Vx4Vx4, bli_dgemm_armsme_2Vx4Vx4_v0

    .section    .note.GNU-stack,"",%progbits
