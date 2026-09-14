/*
 * ukr1m_nvlen_template.h -- packing kernel for cdim == NVLEN * VL.
 *
 * Counterpart of kernels/rviv/1/ukr1_4u{1,2,3,4}vmx.h.  The RVV port needed
 * four separate files because LMUL let it trade register groups against
 * register count; SVE has no LMUL, so one template parameterised by NVLEN
 * covers every case and the whole lmul_utils.h layer disappears.
 *
 * The vector direction runs along cdim, so the store into the packed panel
 * is always contiguous and the predicate is always all-true.  Only the
 * j (panel length) loop needs a remainder, and it is a plain scalar count.
 *
 * Expects (macros):  NVLEN, LABELPREFIX, NVLEN_LOAD_V, NVLEN_STORE_V,
 *                    NVLEN_XFORM_V, NVLEN_XADV, NVLEN_YADV,
 *                    NVLEN_PREPARE_SCALAR, NVLEN_PREPARE_XIDX
 * Expects (C vars):  x, y, n, vlen, inca, lda, ldp
 */

{
    uint64_t cnt;
    uint64_t nrem = n;
    uint64_t xt __attribute__((unused));

    const uint64_t xvi1 __attribute__((unused)) = 1 * vlen * inca;
    const uint64_t xvi2 __attribute__((unused)) = 2 * vlen * inca;
    const uint64_t xvi3 __attribute__((unused)) = 3 * vlen * inca;

    __asm__ volatile
    (
        PTRUE_P0
        NVLEN_PREPARE_SCALAR
        NVLEN_PREPARE_XIDX

        /* cnt = n / 4 ; nrem = n % 4 */
        "lsr %[cnt], %[nrem], #2\n\t"
        "and %[nrem], %[nrem], #3\n\t"
        "cbz %[cnt], .L" LABELPREFIX "tail%=\n\t"

        ".L" LABELPREFIX "loop%=:\n\t"

            NVLEN_PREFETCH_X
            NVLEN_BODY(NVLEN)
            NVLEN_STOREBLOCK(NVLEN)

            "subs %[cnt], %[cnt], #1\n\t"
            "b.ne .L" LABELPREFIX "loop%=\n\t"

        ".L" LABELPREFIX "tail%=:\n\t"
        "cbz %[nrem], .L" LABELPREFIX "end%=\n\t"

        ".L" LABELPREFIX "tloop%=:\n\t"

            NVLEN_TAIL(NVLEN)
            NVLEN_XADV
            NVLEN_YADV

            "subs %[nrem], %[nrem], #1\n\t"
            "b.ne .L" LABELPREFIX "tloop%=\n\t"

        ".L" LABELPREFIX "end%=:\n\t"

        : [xp] "+r" (x), [yp] "+r" (y),
          [cnt] "=&r" (cnt), [nrem] "+r" (nrem), [xt] "=&r" (xt),
          [dummy] "+m" (*(char(*)[]) y)
        : [lda] "r" (lda), [ldp] "r" (ldp),
          [inca] "r" (inca),
          [xvi1] "r" (xvi1), [xvi2] "r" (xvi2), [xvi3] "r" (xvi3),
          [scalarptr] "r" (scalarptr)
        : "cc", "memory",
          "p0", "p7",
          "z0",  "z1",  "z2",  "z3",  "z4",  "z5",  "z6",  "z7",
          "z8",  "z9",  "z10", "z11", "z12", "z13", "z14", "z15",
          "z25", "z26"
    );
}
