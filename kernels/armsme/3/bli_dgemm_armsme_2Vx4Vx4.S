/*=============================================================================
  bli_dgemm_armsme_2Vx4Vx4.S

  BLIS dgemm micro-kernel for Arm SME.   C := beta*C + alpha*A*B

      MR = 2*SVL_D, NR = 4*SVL_D, k-unroll 4, all eight .D tiles.

  Everything below the prologue/dispatch is generated from sme_ukr_macros.h;
  see that file for the tile layout, the register convention and the slice
  unrolling rules.

  Requires FEAT_SME + FEAT_SME_F64F64.  See the header for SME_FA64.

  ARG REGISTERS (BLIS gemm ukr signature)
    x0 m  x1 n  x2 k  x3 &alpha  x4 a  x5 b  x6 &beta  x7 c
    [sp,#0] rs_c  [sp,#8] cs_c  [sp,#16] data  [sp,#24] cntx
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
	stp	d8,  d9,  [sp, #-64]!	// SMSTART/SMSTOP zero Z, so spill d8-d15
	stp	d10, d11, [sp, #16]
	stp	d12, d13, [sp, #32]
	stp	d14, d15, [sp, #48]

	ldr	x9,  [sp, #64]		// rs_c
	ldr	x10, [sp, #72]		// cs_c
	lsl	x9,  x9,  #.LSME_ESH	// -> bytes
	lsl	x10, x10, #.LSME_ESH

	smstart				// PSTATE.SM=1, PSTATE.ZA=1 (ZA zeroed)
	ptrue	p0.d
	SME_CNT	x14, d			// x14 = SVL_D

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
	zero	{za}			// (redundant after SMSTART, kept explicit)
	SME_KLOOP d, UKR_NBLK, UKR_NK

	// ===== SCALE + STOREBLOCK ===============================================
	SME_LD1R 24, x3, d		// alpha
	SME_LD1R 25, x6, d		// beta
	SME_BETA_ZERO_TEST 16, x6, d	// x16 == 0 <=> beta == +-0.0

	cmp	x10, #.LSME_ES		// ---- cs_c == 1: row-major C ----------
	b.eq	.Lstore_rm
#if SME_FA64
	cmp	x9, #.LSME_ES		// ---- rs_c == 1: col-major C ----------
	b.eq	.Lstore_cm
	b	.Lstore_gs		// ---- general stride ------------------
#else
	b	.Lstore_cm		// rs_c == 1 guaranteed by the dispatch
					// above (general stride went to .Ltail)
#endif

.Lstore_rm:				// covers both "nostride" (rs_c==NR) and "rs"
	cbz	x16, .Lstore_rm_b0
	SME_STORE_ROWS 0, 0, d, UKR_NBLK
	b	.Lfinalize
.Lstore_rm_b0:
	SME_STORE_ROWS 0, 1, d, UKR_NBLK
	b	.Lfinalize

.Lstore_cm:
	cbz	x16, .Lstore_cm_b0
	SME_STORE_COLS 0, d, UKR_NBLK
	b	.Lfinalize
.Lstore_cm_b0:
	SME_STORE_COLS 1, d, UKR_NBLK
	b	.Lfinalize

#if SME_FA64
.Lstore_gs:				// scalar+vector addressing (needs FA64)
	SME_GS_OFFSETS UKR_NBLK, d
	cbz	x16, .Lstore_gs_b0
	SME_STORE_ROWS 1, 0, d, UKR_NBLK
	b	.Lfinalize
.Lstore_gs_b0:
	SME_STORE_ROWS 1, 1, d, UKR_NBLK
	b	.Lfinalize
#endif

	// ===== TAIL =============================================================
.Ltail:
	SME_TAIL d, UKR_NBLK

	// ===== FINALIZE / FUNC OUTRO ============================================
.Lfinalize:
	smstop				// PSTATE.SM=0, PSTATE.ZA=0
	ldp	d8,  d9,  [sp, #0]
	ldp	d10, d11, [sp, #16]
	ldp	d12, d13, [sp, #32]
	ldp	d14, d15, [sp, #48]
	add	sp, sp, #64
	ret

	.size	bli_dgemm_armsme_2Vx4Vx4, .-bli_dgemm_armsme_2Vx4Vx4
	.section	.note.GNU-stack,"",%progbits
