#include "ukr1m_multiple_variants.h"

#include "ukr1m_nv_blocks.h"

DECLARE_EXTRA_PTRS

uint64_t xvstride;
uint64_t yvstride;
uint64_t counter;
uint64_t unroll;
uint64_t xfinoff;
uint64_t yfinoff;
// pft: scratch -- setup arithmetic, the source prefetch's address (PFV); once
// the block count is known, unroll holds that prefetch's offset. GCC allows 30
// asm operands, and this asm has them all.
uint64_t pft;
__asm__ (
    PREPARE_SCALAR
    "vsetvli %[vlen], %[vlen], e" SIZEBITS ", m1, ta, ma\n\t"

    /* unroll = vlen */
    MAKEUNROLL("%[vlen]", "1", ID)
    "divu %[counter], %[n], %[unroll]\n\t"
    /* put remainder back into n */
    "remu %[n], %[n], %[unroll]\n\t"
    PFV_INIT

    PREPARE_STRIDEX("%[xvstride]", "%[xstride1]", SIZESHIFT)
    PREPARE_STRIDEY("%[yvstride]", "%[ystride1]", SIZESHIFT)

    PREPARE_LDIMX("%[xvstride]", "%[ldimx]", SIZESHIFT)
    PREPARE_LDIMY("%[yvstride]", "%[ldimy]", SIZESHIFT)


    INIT_EXTRA_PTRS

    "li %[xfinoff], " STR(CDIM) "\n\t"
    "mul %[xfinoff], %[xfinoff], %[xvstride]\n\t"

    CALC_VOFFSET
    "sub %[xfinoff], %[xfinoff], %[yfinoff]\n\t"

    
    "mul %[yfinoff], %[vlen], %[ystride1]\n\t"
    "li %[pft], " STR(CDIM) "\n\t"
    "mul %[pft], %[pft], %[yvstride]\n\t"
    "sub %[yfinoff], %[yfinoff], %[pft]\n\t"


    LDIMFIXUP(ADJUST_STRIDE("%[yvstride]"))
    LDIMFIXUP(ADJUST_STRIDE("%[xvstride]"))

    "beq %[counter], zero, ." LABELPREFIX "fullvend%=\n\t"

    PRELOADBLOCK(PRELOAD_DIST, NPTRS)

    "add %[counter], %[counter], -1\n\t"
    "beq %[counter], zero, ." LABELPREFIX "fullvepilogue%=\n\t"

    "." LABELPREFIX "fullvloop%=:\n\t"

#if RVIV_PACKM_PF_DEST
        "mul %[pft], %[vlen], %[ystride1]\n\t"
        "add %[pft], %[yptr], %[pft]\n\t"
        "prefetch.w 0(%[pft])\n\t"
#endif

        BODYBLOCK(CDIM, PRELOAD_DIST)
        STOREBLOCK(CDIM)

        REWIND_PTRS

        PFV_TOGGLE      // the next block: every other one prefetches
        PRELOADBLOCK(PRELOAD_DIST, NPTRS)

        "add %[counter], %[counter], -1\n\t"
        "bnez %[counter], ." LABELPREFIX "fullvloop%=\n\t"

    "." LABELPREFIX "fullvepilogue%=:\n\t"
        /* EPILOGUE: Compute the final vectors of the last row */
        BODYBLOCK(CDIM, PRELOAD_DIST)
        STOREBLOCK(CDIM)

        REWIND_PTRS

    "." LABELPREFIX "fullvend%=:\n\t"
    "mv %[counter], %[n]\n\t" /* nleft */ 
    "beq %[counter], zero, ." LABELPREFIX "end%=\n\t"
    "." LABELPREFIX "loop%=:\n\t"

        "vsetvli %[vlen], %[counter], e" SIZEBITS ", m1, ta, ma\n\t"
        PREPARE_LDIMX("%[xvstride]", "%[ldimx]", SIZESHIFT)
        PREPARE_LDIMY("%[yvstride]", "%[ldimy]", SIZESHIFT)

        INIT_EXTRA_PTRS


        "li %[xfinoff], " STR(CDIM) "\n\t"
        "mul %[xfinoff], %[xfinoff], %[xvstride]\n\t"

        CALC_VOFFSET
        "sub %[xfinoff], %[xfinoff], %[yfinoff]\n\t"

        
        "mul %[yfinoff], %[vlen], %[ystride1]\n\t"
        "li %[pft], " STR(CDIM) "\n\t"
        "mul %[pft], %[pft], %[yvstride]\n\t"
        "sub %[yfinoff], %[yfinoff], %[pft]\n\t"

        LDIMFIXUP(ADJUST_STRIDE("%[yvstride]"))
        LDIMFIXUP(ADJUST_STRIDE("%[xvstride]"))

        BODYBLOCK(CDIM,0)
        STOREBLOCK(CDIM)

        REWIND_PTRS

        "sub %[counter], %[counter], %[vlen]\n\t"
        "bnez %[counter], ." LABELPREFIX "loop%=\n\t"
    "." LABELPREFIX "end%=:\n\t"
    : [dummy_y] "+m"(*(double(*)[])y),
      CLOBBER_PTRS
      [xvstride] "=r" (xvstride), [yvstride] "=r" (yvstride),
      [counter] "=r" (counter), [n] "+r" (n),
      [vlen] "+r" (vlen), [unroll] "=r" (unroll),
      [ystride1] "+r" (ystride1), [xstride1] "+r" (xstride1),
      [ldimx] "+r" (ldimx), [ldimy] "+r" (ldimy),
      [xfinoff] "=r" (xfinoff), [yfinoff] "=r" (yfinoff),
      [pft] "=&r" (pft)
    : [scalarptr] "r" (scalarptr)
    : "f0",
      "v0", "v1", "v2", "v3",
      "v4", "v5", "v6", "v7",
      "v8", "v9", "v10", "v11",
      "v12", "v13", "v14", "v15"
);
