/*=============================================================================
  bli_sgemm_armsme_2Vx2Vx4_body.h

  The micro-kernel body, included once per prefetch variant by bli_sgemm_armsme_2Vx2Vx4.S.
  Everything here is parameterised by:

      UKR_VARIANT   the variant number (see bli_kernels_armsme.h)
      UKR_SUFFIX    the token appended to the symbol and to every local label

  The UKR_PF_* policies below are derived from UKR_VARIANT, so the variant
  number is the single source of truth.  Nothing in this file may be included
  more than once per #define of UKR_SUFFIX.

  NOTE: .arch, the sme_ukr_macros.h include and SME_DT_SETUP live in the .S,
  not here -- GNU as rejects a second definition of an existing .macro.
=============================================================================*/

#define UKR_CAT2(a,b,c) a##b##c
#define UKR_CAT(a,b,c)  UKR_CAT2(a,b,c)
#define L(n)            UKR_CAT(.L, UKR_SUFFIX, _##n)
#define SYM(base)       UKR_CAT(base, _, UKR_SUFFIX)

#define UKR_PF_FUTURE   ((UKR_VARIANT >> 0) & 1)
#define UKR_PF_C        ((UKR_VARIANT >> 5) & 1)
#define UKR_PF_CNEXT    ((UKR_VARIANT >> 6) & 1)

#if   ((UKR_VARIANT >> 1) & 3) == 1
#define UKR_PF_A        SME_PF_L1KEEP
#elif ((UKR_VARIANT >> 1) & 3) == 2
#define UKR_PF_A        SME_PF_L1STRM
#elif ((UKR_VARIANT >> 1) & 3) == 3
#define UKR_PF_A        SME_PF_L2KEEP
#else
#define UKR_PF_A        SME_PF_OFF
#endif

#if   ((UKR_VARIANT >> 3) & 3) == 1
#define UKR_PF_B        SME_PF_L1KEEP
#elif ((UKR_VARIANT >> 3) & 3) == 2
#define UKR_PF_B        SME_PF_L1STRM
#elif ((UKR_VARIANT >> 3) & 3) == 3
#define UKR_PF_B        SME_PF_L2KEEP
#else
#define UKR_PF_B        SME_PF_OFF
#endif


    .global SYM(bli_sgemm_armsme_2Vx2Vx4)
    .type   SYM(bli_sgemm_armsme_2Vx2Vx4), %function

SYM(bli_sgemm_armsme_2Vx2Vx4):

    // ===== FUNC INTRO =======================================================
    UKR_SME_INIT s          // spill, rs_c/cs_c, PSTATE, p0, x14=SVL_S

    // ===== SIZE / VARIANT DISPATCH ==========================================
    lsl x11, x14, #1        // MR = 2*SVL_S
    cmp x0, x11
    b.lt    L(tail)
    lsl x11, x14, #UKR_NR_LOG2  // NR = 2*SVL_S
    cmp x1, x11
    b.lt    L(tail)          // m<MR or n<NR -> predicated tail kernel
#if !SME_FA64
    cmp x10, #.LSME_ES      // cs_c == 1 ?
    b.eq    L(main)
    cmp x9, #.LSME_ES       // rs_c == 1 ?
    b.eq    L(main)
    b   L(tail)          // general stride w/o FA64: use tail path
#endif

    // ===== MAIN LOOP ========================================================
L(main):
#if UKR_PF_FUTURE
    SME_PF_FUTURE UKR_NR_LOG2       // next A/B panel -> L2, TLB warm
#endif
    // The epilogue's branch conditions are computed here because x3/x6 are
    // reused as ZA pointers later; none of x0/x16/x17/z24/z25 is touched by
    // SME_KLOOP.
    SME_LD1R 24, x3, s      // alpha  (only the Z-register paths use it)
    SME_LD1R 25, x6, s      // beta
    SME_BETA_ZERO_TEST 16, x6, s    // x16 == 0 <=> beta  == +-0.0
    SME_FP_ONE_TEST    17, x6, s    // x17 == 0 <=> beta  == 1.0
    SME_FP_ONE_TEST     0, x3, s    // x0  == 0 <=> alpha == 1.0   (m is dead)

#if UKR_PF_C
    adrp    x15, bli_armsme_pf_c_dist   // hot/cooldown split point
    ldr     w15, [x15, #:lo12:bli_armsme_pf_c_dist]
#endif
#if UKR_PF_CNEXT
    SME_PF_CNEXT UKR_NBLK, UKR_NR_LOG2  // next ir tile -> L2
#endif
    SME_KLOOP s, UKR_NBLK, UKR_NK, UKR_PF_A, UKR_PF_B, UKR_PF_C, UKR_NR_LOG2

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
    cbnz    x0,  L(store_gen)        // alpha != 1
    cbnz    x16, L(store_gen)        // beta  != 0
    SME_IF_STRIDED L(store_gen)      // general stride
    cmp x10, #.LSME_ES
    b.ne    L(store_za_cm)
    SME_ZA_ROWS 0, s, UKR_NBLK      // ZA -> C, horizontal slices
    b   L(finalize)
L(store_za_cm):
    SME_ZA_COLS 0, s, UKR_NBLK      // ZA -> C, vertical slices
    b   L(finalize)

L(store_gen):
    cmp x10, #.LSME_ES      // ---- cs_c == 1: row-major C ----------
    b.eq    L(store_rm)
#if SME_FA64
    cmp x9, #.LSME_ES       // ---- rs_c == 1: col-major C ----------
    b.eq    L(store_cm)
    b   L(store_gs)      // ---- general stride ------------------
#else
    b   L(store_cm)
#endif

L(store_rm):
    cbz x16, L(store_rm_b0)
    cbz x17, L(store_rm_b1)
    SME_STORE_ROWS 0, 0, s, UKR_NBLK
    b   L(finalize)
L(store_rm_b0):
    SME_STORE_ROWS 0, 1, s, UKR_NBLK
    b   L(finalize)
L(store_rm_b1):
    SME_STORE_ROWS 0, 2, s, UKR_NBLK
    b   L(finalize)

L(store_cm):
    cbz x16, L(store_cm_b0)
    cbz x17, L(store_cm_b1)
    SME_STORE_COLS 0, s, UKR_NBLK
    b   L(finalize)
L(store_cm_b0):
    SME_STORE_COLS 1, s, UKR_NBLK
    b   L(finalize)
L(store_cm_b1):
    SME_STORE_COLS 2, s, UKR_NBLK
    b   L(finalize)

#if SME_FA64
L(store_gs):
    SME_GS_OFFSETS UKR_NBLK, s
    cbz x16, L(store_gs_b0)
    SME_STORE_ROWS 1, 0, s, UKR_NBLK
    b   L(finalize)
L(store_gs_b0):
    SME_STORE_ROWS 1, 1, s, UKR_NBLK
    b   L(finalize)
#endif

    // ===== TAIL =============================================================
L(tail):                 // SME_TAIL zeroes tile 0 per subtile, so
                    // it needs no accumulator clear from here
    SME_TAIL s, UKR_NBLK

    // ===== FINALIZE / FUNC OUTRO ============================================
L(finalize):
    UKR_SME_DEINIT          // PSTATE, restore d8-d15, pop
    ret

    .size   SYM(bli_sgemm_armsme_2Vx2Vx4), .-SYM(bli_sgemm_armsme_2Vx2Vx4)

#undef UKR_CAT2
#undef UKR_CAT
#undef L
#undef SYM
#undef UKR_PF_FUTURE
#undef UKR_PF_A
#undef UKR_PF_B
#undef UKR_PF_C
#undef UKR_PF_CNEXT
