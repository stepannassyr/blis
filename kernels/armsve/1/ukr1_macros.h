/*
 * ukr1_macros.h -- SVE instruction-level primitives.
 *
 * Counterpart of kernels/rviv/1/ukr1_macros.h.
 *
 * Datatype parameterisation (defined by the including .c file):
 *   ESZ    ".d" | ".s"     SVE element size suffix
 *   MSUF   "d"  | "w"      memory-op mnemonic suffix (ld1d/ld1w, st4d/st4w)
 *   SHIFT  "3"  | "2"      log2(sizeof(element)), for `lsl #SHIFT`
 *   ESIZE  "8"  | "4"      sizeof(element)
 *   INCE   "incd" | "incw" scalar increment by VL elements
 *   CNTE   "cntd" | "cntw" VL in elements
 *   RPFX   ""   | "w"      GPR name prefix for INDEX (%[x] vs %w[x])
 *   GXTW   "lsl"| "uxtw"   gather/scatter offset extension
 *
 * Note on GXTW: 32-bit gathers take 32-bit lane offsets, so the largest
 * element offset an index vector can express is 2^32-1, i.e. (VL-1)*stride
 * must fit in 32 bits.  That is never a practical constraint for packm.
 *
 * Fixed register assignment (all Z/P registers are clobbered by the kernels):
 *   z0 .. z15   data
 *   z24         zip scratch          (transpose path)
 *   z25         broadcast kappa
 *   z26         x gather index vector
 *   z27         y scatter index vector
 *   p0          governing predicate  (whilelt, or ptrue on the NVLEN path)
 *   p7          ptrue
 *
 * Unlike RVV there is no LMUL: SVE has 32 fixed-width Z registers, so the
 * whole LMUL_/LMID_ layer of the RVV port disappears.  Conversely SVE has no
 * strided load/store, so every non-unit-stride access becomes a
 * gather/scatter driven by an index vector built once with INDEX.
 */

#ifndef UKR1_SVE_MACROS_H
#define UKR1_SVE_MACROS_H

#define STR_I(x) #x
#define STR(x) STR_I(x)

/* ------------------------------------------------------------------ */
/* predicates and loop control                                         */
/* ------------------------------------------------------------------ */

#define PTRUE_ALL           "ptrue p7" ESZ "\n\t"
#define PTRUE_P0            "ptrue p0" ESZ "\n\t"
#define WHILELT_P0(i, n)    "whilelt p0" ESZ ", " i ", " n "\n\t"

/* ------------------------------------------------------------------ */
/* kappa                                                               */
/* ------------------------------------------------------------------ */

/* broadcast *kappa into z25 (one instruction; RVV needed an f0 scalar reg) */
#define PREPARE_SCALAR_BCAST(pred) \
    "ld1r" MSUF " {z25" ESZ "}, " pred "/z, [%[scalarptr]]\n\t"

#define VFMUL_KAPPA(z) \
    "fmul z" STR(z) ESZ ", z" STR(z) ESZ ", z25" ESZ "\n\t"

/* ------------------------------------------------------------------ */
/* index vectors for gather / scatter                                  */
/* ------------------------------------------------------------------ */

/* z26 = [0, s, 2s, ...] in element units; used as `[base, z26, lsl #SHIFT]` */
#define PREPARE_XINDEX(nm)  "index z26" ESZ ", #0, %" RPFX "[" nm "]\n\t"
#define PREPARE_YINDEX(nm)  "index z27" ESZ ", #0, %" RPFX "[" nm "]\n\t"

/* ------------------------------------------------------------------ */
/* transpose path (cdim <= 16, vector runs along n)                    */
/* ------------------------------------------------------------------ */

/* row i lives at  x + i*inca ; rows 0..7 off %[xp], rows 8..15 off %[xb2] */
#define NV_XBASE_0 "%[xp]"
#define NV_XBASE_1 "%[xb2]"
#define NV_XBASE_I(b) NV_XBASE_ ## b
#define NV_XBASE(b) NV_XBASE_I(b)

#define NV_XOFF_0
#define NV_XOFF_1 ", %[xo1], lsl #" SHIFT
#define NV_XOFF_2 ", %[xo2], lsl #" SHIFT
#define NV_XOFF_3 ", %[xo3], lsl #" SHIFT
#define NV_XOFF_4 ", %[xo4], lsl #" SHIFT
#define NV_XOFF_5 ", %[xo5], lsl #" SHIFT
#define NV_XOFF_6 ", %[xo6], lsl #" SHIFT
#define NV_XOFF_7 ", %[xo7], lsl #" SHIFT
#define NV_XOFF_I(o) NV_XOFF_ ## o
#define NV_XOFF(o) NV_XOFF_I(o)

/* xb2 = xp + 8*inca, only needed when cdim > 8 */
#define NV_SETUP_XB2_YES "add %[xb2], %[xp], %[xo8], lsl #" SHIFT "\n\t"
#define NV_SETUP_XB2_NO

/* --- contiguous row load (lda == 1) --------------------------------- */
#define NV_LOAD_ROW_C(z, b, o) \
    "ld1" MSUF " {z" STR(z) ESZ "}, p0/z, [" NV_XBASE(b) NV_XOFF(o) "]\n\t"

/* --- gathered row load (lda != 1) ----------------------------------- */
/* gather has no scalar+scalar form, so materialise the base in %[xt]    */
#define NV_MKGBASE_0(b)
#define NV_MKGBASE_1(b) "add %[xt], " NV_XBASE(b) ", %[xo1], lsl #" SHIFT "\n\t"
#define NV_MKGBASE_2(b) "add %[xt], " NV_XBASE(b) ", %[xo2], lsl #" SHIFT "\n\t"
#define NV_MKGBASE_3(b) "add %[xt], " NV_XBASE(b) ", %[xo3], lsl #" SHIFT "\n\t"
#define NV_MKGBASE_4(b) "add %[xt], " NV_XBASE(b) ", %[xo4], lsl #" SHIFT "\n\t"
#define NV_MKGBASE_5(b) "add %[xt], " NV_XBASE(b) ", %[xo5], lsl #" SHIFT "\n\t"
#define NV_MKGBASE_6(b) "add %[xt], " NV_XBASE(b) ", %[xo6], lsl #" SHIFT "\n\t"
#define NV_MKGBASE_7(b) "add %[xt], " NV_XBASE(b) ", %[xo7], lsl #" SHIFT "\n\t"
#define NV_MKGBASE_I(o, b) NV_MKGBASE_ ## o(b)
#define NV_MKGBASE(o, b) NV_MKGBASE_I(o, b)

#define NV_GREF_0(b) NV_XBASE(b)
#define NV_GREF_1(b) "%[xt]"
#define NV_GREF_2(b) "%[xt]"
#define NV_GREF_3(b) "%[xt]"
#define NV_GREF_4(b) "%[xt]"
#define NV_GREF_5(b) "%[xt]"
#define NV_GREF_6(b) "%[xt]"
#define NV_GREF_7(b) "%[xt]"
#define NV_GREF_I(o, b) NV_GREF_ ## o(b)
#define NV_GREF(o, b) NV_GREF_I(o, b)

#define NV_LOAD_ROW_G(z, b, o) \
    NV_MKGBASE(o, b) \
    "ld1" MSUF " {z" STR(z) ESZ "}, p0/z, [" NV_GREF(o, b) ", z26" ESZ ", " GXTW " #" SHIFT "]\n\t"

/* --- scatter store (general ldp) ------------------------------------ */
#define NV_MKYBASE_0
#define NV_MKYBASE_N(i) "add %[yt], %[yp], #(" STR(i) " * " ESIZE ")\n\t"

#define NV_STORE_SCATTER_ROW_0 \
    "st1" MSUF " {z0" ESZ "}, p0, [%[yp], z27" ESZ ", " GXTW " #" SHIFT "]\n\t"
#define NV_STORE_SCATTER_ROW_N(i) \
    NV_MKYBASE_N(i) \
    "st1" MSUF " {z" STR(i) ESZ "}, p0, [%[yt], z27" ESZ ", " GXTW " #" SHIFT "]\n\t"

/* --- interleaved store (ldp == cdim): zip network + ST{2,3,4}x ------ */
/* in-place zip of a register pair, one scratch register                */
#define NV_ZIP_PAIR(a, b) \
    "zip2 z24" ESZ ", z" STR(a) ESZ ", z" STR(b) ESZ "\n\t" \
    "zip1 z" STR(a) ESZ ", z" STR(a) ESZ ", z" STR(b) ESZ "\n\t" \
    "mov  z" STR(b) ".d, z24.d\n\t"   /* full-reg copy: .d is the only\n                                          unpredicated MOV form */

/* Per-group predicates.
 *
 * ST{2,3,4}x predication is per lane, and each active lane stores `base`
 * consecutive elements.  Before the zip stages lane k is column j0+k, so p0
 * would be right; *after* them a lane no longer maps to a column, and the
 * valid output of a partial block is simply the first nb*cdim elements.
 * Dividing by `base`, group g must store lanes with  g*VL + lane < nb*2^k,
 * i.e. whilelt(g*VL, (n-j) << k).  For k == 0 (cdim in {1,2,3,4}) this
 * degenerates to p0 and no setup is emitted.
 */
#define NV_ILV_PRED_INIT(K) \
    "sub %[yt], %[n], %[j]\n\t" \
    "lsl %[yt], %[yt], #" STR(K) "\n\t" \
    "mov %[xt], xzr\n\t"
#define NV_ILV_PRED_FIRST(pg) \
    "whilelt p" STR(pg) ESZ ", %[xt], %[yt]\n\t"
#define NV_ILV_PRED_NEXT(pg) \
    INCE " %[xt]\n\t" \
    "whilelt p" STR(pg) ESZ ", %[xt], %[yt]\n\t"

#define NV_STORE_ILV1(z0, pg, off) \
    "st1" MSUF " {z" STR(z0) ESZ "}, p" STR(pg) ", [%[yp], #" STR(off) ", mul vl]\n\t"
#define NV_STORE_ILV2(z0, z1, pg, off) \
    "st2" MSUF " {z" STR(z0) ESZ " - z" STR(z1) ESZ "}, p" STR(pg) ", [%[yp], #" STR(off) ", mul vl]\n\t"
#define NV_STORE_ILV3(z0, z1, z2, pg, off) \
    "st3" MSUF " {z" STR(z0) ESZ " - z" STR(z2) ESZ "}, p" STR(pg) ", [%[yp], #" STR(off) ", mul vl]\n\t"
#define NV_STORE_ILV4(z0, z1, z2, z3, pg, off) \
    "st4" MSUF " {z" STR(z0) ESZ " - z" STR(z3) ESZ "}, p" STR(pg) ", [%[yp], #" STR(off) ", mul vl]\n\t"

/* ------------------------------------------------------------------ */
/* cdim == NVLEN*VL path (vector runs along cdim)                      */
/* ------------------------------------------------------------------ */

/* contiguous (inca == 1): vector k of a column is at  xp + k*VL        */
#define NVLEN_LOAD_V_C(z, k) \
    "ld1" MSUF " {z" STR(z) ESZ "}, p0/z, [%[xp], #" STR(k) ", mul vl]\n\t"

/* gathered (inca != 1): base of vector k is  xp + k*VL*inca            */
#define NVLEN_MKGBASE_0
#define NVLEN_MKGBASE_1 "add %[xt], %[xp], %[xvi1], lsl #" SHIFT "\n\t"
#define NVLEN_MKGBASE_2 "add %[xt], %[xp], %[xvi2], lsl #" SHIFT "\n\t"
#define NVLEN_MKGBASE_3 "add %[xt], %[xp], %[xvi3], lsl #" SHIFT "\n\t"
#define NVLEN_MKGBASE_I(k) NVLEN_MKGBASE_ ## k
#define NVLEN_MKGBASE(k) NVLEN_MKGBASE_I(k)

#define NVLEN_GREF_0 "%[xp]"
#define NVLEN_GREF_1 "%[xt]"
#define NVLEN_GREF_2 "%[xt]"
#define NVLEN_GREF_3 "%[xt]"
#define NVLEN_GREF_I(k) NVLEN_GREF_ ## k
#define NVLEN_GREF(k) NVLEN_GREF_I(k)

#define NVLEN_LOAD_V_G(z, k) \
    NVLEN_MKGBASE(k) \
    "ld1" MSUF " {z" STR(z) ESZ "}, p0/z, [" NVLEN_GREF(k) ", z26" ESZ ", " GXTW " #" SHIFT "]\n\t"

/* the packed panel is always unit-stride in the cdim direction */
#define NVLEN_STORE_V_C(z, k) \
    "st1" MSUF " {z" STR(z) ESZ "}, p0, [%[yp], #" STR(k) ", mul vl]\n\t"

#define NVLEN_XADV_I "add %[xp], %[xp], %[lda], lsl #" SHIFT "\n\t"
#define NVLEN_YADV_I "add %[yp], %[yp], %[ldp], lsl #" SHIFT "\n\t"

/* ------------------------------------------------------------------ */
/* prefetch                                                            */
/* ------------------------------------------------------------------ */

#ifndef PACKM_SVE_PREFETCH
#define PACKM_SVE_PREFETCH 1
#endif

#if PACKM_SVE_PREFETCH
#define NV_PREFETCH_Y \
    "add %[yt], %[yp], %[ystep], lsl #" SHIFT "\n\t" \
    "prf" MSUF " pstl1keep, p7, [%[yt]]\n\t"
#define NVLEN_PREFETCH_X \
    "prf" MSUF " pldl1keep, p0, [%[xp], #4, mul vl]\n\t"
#else
#define NV_PREFETCH_Y
#define NVLEN_PREFETCH_X
#endif

#endif /* UKR1_SVE_MACROS_H */
