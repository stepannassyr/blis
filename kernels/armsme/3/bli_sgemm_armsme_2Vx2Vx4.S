/*=============================================================================
  bli_sgemm_armsme_2Vx2Vx4.S

  BLIS sgemm micro-kernel for Arm SME.   C := beta*C + alpha*A*B

      MR = 2*SVL_S, NR = 2*SVL_S, k-unroll 4, all four .S tiles.

  Everything below the prologue/dispatch is generated from sme_ukr_macros.h;
  see that file for the tile layout, the register convention and the slice
  unrolling rules.

  Requires FEAT_SME only (fmopa .s is non-widening base SME).  See the header for SME_FA64.

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

#define UKR_NBLK    1       /* NR = 2*nblk*SVL                    */
#define UKR_NK      4       /* k-unroll                           */
#define UKR_NR_LOG2 1       /* NR = SVL << UKR_NR_LOG2            */

    .arch   armv9-a+sme

#include "sme_ukr_macros.h"

    SME_DT_SETUP s

    .text
    .global bli_sgemm_armsme_2Vx2Vx4
    .type   bli_sgemm_armsme_2Vx2Vx4, %function

bli_sgemm_armsme_2Vx2Vx4:

    // ===== FUNC INTRO =======================================================
    UKR_SME_INIT s          // spill, rs_c/cs_c, PSTATE, p0, x14=SVL_S

    // ===== SIZE / VARIANT DISPATCH ==========================================
    lsl x11, x14, #1        // MR = 2*SVL_S
    cmp x0, x11
    b.lt    .Ltail
    lsl x11, x14, #UKR_NR_LOG2  // NR = 2*SVL_S
    cmp x1, x11
    b.lt    .Ltail          // m<MR or n<NR -> predicated tail kernel
#if !SME_FA64
    cmp x10, #.LSME_ES      // cs_c == 1 ?
    b.eq    .Lmain
    cmp x9, #.LSME_ES       // rs_c == 1 ?
    b.eq    .Lmain
    b   .Ltail          // general stride w/o FA64: use tail path
#endif

    // ===== MAIN LOOP ========================================================
.Lmain:
    // The epilogue's branch conditions are computed here because x3/x6 are
    // reused as ZA pointers later; none of x0/x16/x17/z24/z25 is touched by
    // SME_KLOOP.
    SME_LD1R 24, x3, s      // alpha  (only the Z-register paths use it)
    SME_LD1R 25, x6, s      // beta
    SME_BETA_ZERO_TEST 16, x6, s    // x16 == 0 <=> beta  == +-0.0
    SME_FP_ONE_TEST    17, x6, s    // x17 == 0 <=> beta  == 1.0
    SME_FP_ONE_TEST     0, x3, s    // x0  == 0 <=> alpha == 1.0   (m is dead)

    SME_KLOOP s, UKR_NBLK, UKR_NK

    // ===== SCALE + STOREBLOCK ===============================================
    // ZA-direct store, for alpha==1 && beta==0 && unit stride only: ZA holds
    // the finished result, so it goes to memory with no Z register in the path.
    //
    // There is deliberately no beta==1 counterpart.  That would require
    // preloading C into ZA before the k-loop, and the k-loop evicts C from L1
    // long before the store, so the cache lines get paid for twice.
    // SME_ZA_ROWS/COLS still take a dir argument and dir=1 (C -> ZA) is verified
    // working if this is ever worth retrying on
    // hardware with a larger L1 or a cheaper ZA port.
    cbnz    x0,  .Lstore_gen        // alpha != 1
    cbnz    x16, .Lstore_gen        // beta  != 0
    SME_IF_STRIDED .Lstore_gen      // general stride
    cmp x10, #.LSME_ES
    b.ne    3f
    SME_ZA_ROWS 0, s, UKR_NBLK      // ZA -> C, horizontal slices
    b   .Lfinalize
3:  SME_ZA_COLS 0, s, UKR_NBLK      // ZA -> C, vertical slices
    b   .Lfinalize

.Lstore_gen:
    cmp x10, #.LSME_ES      // ---- cs_c == 1: row-major C ----------
    b.eq    .Lstore_rm
#if SME_FA64
    cmp x9, #.LSME_ES       // ---- rs_c == 1: col-major C ----------
    b.eq    .Lstore_cm
    b   .Lstore_gs      // ---- general stride ------------------
#else
    b   .Lstore_cm
#endif

.Lstore_rm:
    cbz x16, .Lstore_rm_b0
    cbz x17, .Lstore_rm_b1
    SME_STORE_ROWS 0, 0, s, UKR_NBLK
    b   .Lfinalize
.Lstore_rm_b0:
    SME_STORE_ROWS 0, 1, s, UKR_NBLK
    b   .Lfinalize
.Lstore_rm_b1:
    SME_STORE_ROWS 0, 2, s, UKR_NBLK
    b   .Lfinalize

.Lstore_cm:
    cbz x16, .Lstore_cm_b0
    cbz x17, .Lstore_cm_b1
    SME_STORE_COLS 0, s, UKR_NBLK
    b   .Lfinalize
.Lstore_cm_b0:
    SME_STORE_COLS 1, s, UKR_NBLK
    b   .Lfinalize
.Lstore_cm_b1:
    SME_STORE_COLS 2, s, UKR_NBLK
    b   .Lfinalize

#if SME_FA64
.Lstore_gs:
    SME_GS_OFFSETS UKR_NBLK, s
    cbz x16, .Lstore_gs_b0
    SME_STORE_ROWS 1, 0, s, UKR_NBLK
    b   .Lfinalize
.Lstore_gs_b0:
    SME_STORE_ROWS 1, 1, s, UKR_NBLK
    b   .Lfinalize
#endif

    // ===== TAIL =============================================================
.Ltail:                 // SME_TAIL zeroes tile 0 per subtile, so
                    // it needs no accumulator clear from here
    SME_TAIL s, UKR_NBLK

    // ===== FINALIZE / FUNC OUTRO ============================================
.Lfinalize:
    UKR_SME_DEINIT          // PSTATE, restore d8-d15, pop
    ret

    .size   bli_sgemm_armsme_2Vx2Vx4, .-bli_sgemm_armsme_2Vx2Vx4
    .section    .note.GNU-stack,"",%progbits
