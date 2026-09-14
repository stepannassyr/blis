/*
 * ukr1m_nv_template.h -- "transpose" packing kernel, cdim <= NV_CDIM_MAX.
 *
 * Counterpart of kernels/rviv/1m/ukr1m_nv_template.h.
 *
 * The vector direction runs along n (the panel length), so cdim may be any
 * small integer.  Register i holds  a[i*inca + (j..j+VL)*lda], i.e. one row
 * of the micropanel, and the packed panel wants p[i + j*ldp] -- a transpose.
 *
 * Differences from the RVV original:
 *
 *  - Predication replaces the duplicated tail loop.  `whilelt` produces the
 *    governing predicate for both the full and the partial final iteration,
 *    so there is exactly one loop body instead of two, and none of the
 *    stride/ldim recomputation the RVV tail needed.
 *
 *  - Row addressing uses AArch64 scalar+scalar / scalar+immediate forms
 *    instead of one live pointer per row, so there is no PRELOAD_DIST /
 *    NPTRS / REWIND_PTRS machinery: %[xp] only ever advances by VL*lda once
 *    per block and never has to be rewound.
 *
 *  - The store is either a scatter (general ldp) or, when ldp == cdim and
 *    cdim = 2^k*{1,2,3,4}, a zip network followed by ST{2,3,4}x interleaving
 *    stores, which are fully contiguous.  See ukr1m_nv_blocks.h.
 *
 * Expects (macros):  CDIM, LABELPREFIX, NV_LOAD_ROW, NV_XFORM_ROW,
 *                    NV_STOREBLOCK, NV_PREPARE_SCALAR, NV_PREPARE_XIDX,
 *                    NV_PREPARE_YIDX
 * Expects (C vars):  x, y, n, vlen, inca, lda, ldp
 */

{
    uint64_t j;
    uint64_t xt, yt;
    uint64_t xb2 __attribute__((unused));

    const uint64_t xo1 __attribute__((unused)) = 1 * inca;
    const uint64_t xo2 __attribute__((unused)) = 2 * inca;
    const uint64_t xo3 __attribute__((unused)) = 3 * inca;
    const uint64_t xo4 __attribute__((unused)) = 4 * inca;
    const uint64_t xo5 __attribute__((unused)) = 5 * inca;
    const uint64_t xo6 __attribute__((unused)) = 6 * inca;
    const uint64_t xo7 __attribute__((unused)) = 7 * inca;
    const uint64_t xo8 __attribute__((unused)) = 8 * inca;

    const uint64_t xstep = vlen * lda;   /* x advance per VL-block of n */
    const uint64_t ystep = vlen * ldp;   /* y advance per VL-block of n */

    __asm__ volatile
    (
        PTRUE_ALL
        NV_PREPARE_SCALAR
        NV_PREPARE_XIDX
        NV_PREPARE_YIDX

        "mov %[j], #0\n\t"
        WHILELT_P0("%[j]", "%[n]")
        "b.none .L" LABELPREFIX "end%=\n\t"

        ".L" LABELPREFIX "loop%=:\n\t"

            NV_XB2_SETUP(CDIM)
            NV_PREFETCH_Y

            NV_LOADS(CDIM)
            NV_STOREBLOCK

            "add %[xp], %[xp], %[xstep], lsl #" SHIFT "\n\t"
            "add %[yp], %[yp], %[ystep], lsl #" SHIFT "\n\t"

            INCE " %[j]\n\t"
            WHILELT_P0("%[j]", "%[n]")
            "b.first .L" LABELPREFIX "loop%=\n\t"

        ".L" LABELPREFIX "end%=:\n\t"

        : [xp] "+r" (x), [yp] "+r" (y),
          [j] "=&r" (j), [xt] "=&r" (xt), [yt] "=&r" (yt),
          NV_OPS_XB2(CDIM)
          [dummy] "+m" (*(char(*)[]) y)
        : [n] "r" (n),
          NV_OPS_XO(CDIM)
          [xstep] "r" (xstep), [ystep] "r" (ystep),
          [lda] "r" (lda), [ldp] "r" (ldp),
          [scalarptr] "r" (scalarptr)
        : "cc", "memory",
          "p0", "p1", "p2", "p3", "p4", "p7",
          "z0",  "z1",  "z2",  "z3",  "z4",  "z5",  "z6",  "z7",
          "z8",  "z9",  "z10", "z11", "z12", "z13", "z14", "z15",
          "z24", "z25", "z26", "z27"
    );
}
