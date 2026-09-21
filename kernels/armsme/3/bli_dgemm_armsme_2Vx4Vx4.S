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

#define UKR_NBLK	2		/* NR = 2*nblk*SVL                    */
#define UKR_NK		4		/* k-unroll                           */
#define UKR_NR_LOG2	2		/* NR = SVL << UKR_NR_LOG2            */

	/* binutils < 2.39 spells this "+sme-f64" */
	.arch	armv9-a+sme+sme-f64f64

#include "sme_ukr_macros.h"

	SME_DT_SETUP d

	.text
	.global	bli_dgemm_armsme_2Vx4Vx4
	.type	bli_dgemm_armsme_2Vx4Vx4, %function

bli_dgemm_armsme_2Vx4Vx4:

	// ===== FUNC INTRO =======================================================
	UKR_SME_INIT d			// spill, rs_c/cs_c, PSTATE, p0, x14=SVL_D

	// ===== SIZE / VARIANT DISPATCH ==========================================
	lsl	x11, x14, #1		// MR = 2*SVL_D
	cmp	x0, x11
	b.lt	.Ltail
	lsl	x11, x14, #UKR_NR_LOG2	// NR = 4*SVL_D
	cmp	x1, x11
	b.lt	.Ltail			// m<MR or n<NR -> predicated tail kernel
#if !SME_FA64
	cmp	x10, #.LSME_ES		// cs_c == 1 ?
	b.eq	.Lmain
	cmp	x9, #.LSME_ES		// rs_c == 1 ?
	b.eq	.Lmain
	b	.Ltail			// general stride w/o FA64: use tail path
#endif

	// ===== MAIN LOOP ========================================================
.Lmain:
	// Everything the epilogue branches on is decided BEFORE the k-loop, so
	// that the ZA preload and the store agree.  None of x0/x16/x17/z24/z25
	// is touched by SME_KLOOP.
	SME_LD1R 24, x3, d		// alpha  (only the Z-register paths use it)
	SME_LD1R 25, x6, d		// beta
	SME_BETA_ZERO_TEST 16, x6, d	// x16 == 0 <=> beta  == +-0.0
	SME_FP_ONE_TEST    17, x6, d	// x17 == 0 <=> beta  == 1.0
	SME_FP_ONE_TEST     0, x3, d	// x0  == 0 <=> alpha == 1.0   (m is dead)

	// ---- preload C into ZA iff alpha==1 && beta==1 && unit stride ---------
	cbnz	x0,  .Lkloop
	cbnz	x17, .Lkloop
	SME_IF_STRIDED .Lkloop
	stp	x4, x5, [sp, #-16]!		// a/b: x3..x6 are the ZA pointers
	cmp	x10, #.LSME_ES
	b.ne	1f
	SME_ZA_ROWS 1, d, UKR_NBLK		// C -> ZA, horizontal slices
	b	2f
1:	SME_ZA_COLS 1, d, UKR_NBLK		// C -> ZA, vertical slices
2:	ldp	x4, x5, [sp], #16

.Lkloop:
	SME_KLOOP d, UKR_NBLK, UKR_NK

	// ===== SCALE + STOREBLOCK ===============================================
	cbnz	x0, .Lstore_gen			// alpha != 1 -> Z-register epilogue
	SME_IF_STRIDED .Lstore_gen
	cbz	x16, .Lstore_za			// beta == 0: ZA already holds acc
	cbnz	x17, .Lstore_gen		// beta != 1: needs a real scale
.Lstore_za:					// beta == 1: C was preloaded
	cmp	x10, #.LSME_ES
	b.ne	3f
	SME_ZA_ROWS 0, d, UKR_NBLK		// ZA -> C, no Z registers at all
	b	.Lfinalize
3:	SME_ZA_COLS 0, d, UKR_NBLK
	b	.Lfinalize

.Lstore_gen:
	cmp	x10, #.LSME_ES		// ---- cs_c == 1: row-major C ----------
	b.eq	.Lstore_rm
#if SME_FA64
	cmp	x9, #.LSME_ES		// ---- rs_c == 1: col-major C ----------
	b.eq	.Lstore_cm
	b	.Lstore_gs		// ---- general stride ------------------
#else
	b	.Lstore_cm
#endif

.Lstore_rm:
	cbz	x16, .Lstore_rm_b0
	cbz	x17, .Lstore_rm_b1
	SME_STORE_ROWS 0, 0, d, UKR_NBLK
	b	.Lfinalize
.Lstore_rm_b0:
	SME_STORE_ROWS 0, 1, d, UKR_NBLK
	b	.Lfinalize
.Lstore_rm_b1:
	SME_STORE_ROWS 0, 2, d, UKR_NBLK
	b	.Lfinalize

.Lstore_cm:
	cbz	x16, .Lstore_cm_b0
	cbz	x17, .Lstore_cm_b1
	SME_STORE_COLS 0, d, UKR_NBLK
	b	.Lfinalize
.Lstore_cm_b0:
	SME_STORE_COLS 1, d, UKR_NBLK
	b	.Lfinalize
.Lstore_cm_b1:
	SME_STORE_COLS 2, d, UKR_NBLK
	b	.Lfinalize

#if SME_FA64
.Lstore_gs:
	SME_GS_OFFSETS UKR_NBLK, d
	cbz	x16, .Lstore_gs_b0
	SME_STORE_ROWS 1, 0, d, UKR_NBLK
	b	.Lfinalize
.Lstore_gs_b0:
	SME_STORE_ROWS 1, 1, d, UKR_NBLK
	b	.Lfinalize
#endif

	// ===== TAIL =============================================================
.Ltail:					// SME_TAIL zeroes tile 0 per subtile, so
					// it needs no accumulator clear from here
	SME_TAIL d, UKR_NBLK

	// ===== FINALIZE / FUNC OUTRO ============================================
.Lfinalize:
	UKR_SME_DEINIT			// PSTATE, restore d8-d15, pop
	ret

	.size	bli_dgemm_armsme_2Vx4Vx4, .-bli_dgemm_armsme_2Vx4Vx4
	.section	.note.GNU-stack,"",%progbits
