#include "ukr1m_multiple_variants.h"

#include "ukr1m_nv_blocks.h"

DECLARE_EXTRA_PTRS

uint64_t xvstride;
uint64_t yvstride;
uint64_t counter;
uint64_t unroll;
uint64_t xfinoff;
uint64_t yfinoff;
uint64_t yptrprefetch;
__asm__ (
    PREPARE_SCALAR
    "vsetvli %[vlen], %[vlen], e" SIZEBITS ", m1, ta, ma\n\t"

    /* unroll = vlen */
    MAKEUNROLL("%[vlen]", "1", ID)
    "divu %[counter], %[n], %[unroll]\n\t"
    /* put remainder back into n */
    "remu %[n], %[n], %[unroll]\n\t"

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
    "li %[yptrprefetch], " STR(CDIM) "\n\t"
    "mul %[yptrprefetch], %[yptrprefetch], %[yvstride]\n\t"
    "sub %[yfinoff], %[yfinoff], %[yptrprefetch]\n\t"


    LDIMFIXUP(ADJUST_STRIDE("%[yvstride]"))
    LDIMFIXUP(ADJUST_STRIDE("%[xvstride]"))

    "beq %[counter], zero, ." LABELPREFIX "fullvend%=\n\t"

    PRELOADBLOCK(PRELOAD_DIST, NPTRS)

    "add %[counter], %[counter], -1\n\t"
    "beq %[counter], zero, ." LABELPREFIX "fullvepilogue%=\n\t"

    "." LABELPREFIX "fullvloop%=:\n\t"

        "mul %[yptrprefetch], %[vlen], %[ystride1]\n\t"
        "add %[yptrprefetch], %[yptr], %[yptrprefetch]\n\t"
        "prefetch.w 0(%[yptrprefetch])\n\t"

        BODYBLOCK(CDIM, PRELOAD_DIST)
        STOREBLOCK(CDIM)

        REWIND_PTRS


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
        "li %[yptrprefetch], " STR(CDIM) "\n\t"
        "mul %[yptrprefetch], %[yptrprefetch], %[yvstride]\n\t"
        "sub %[yfinoff], %[yfinoff], %[yptrprefetch]\n\t"

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
      [yptrprefetch] "=r" (yptrprefetch)
    : [scalarptr] "r" (scalarptr)
    : "f0",
      "v0", "v1", "v2", "v3",
      "v4", "v5", "v6", "v7",
      "v8", "v9", "v10", "v11",
      "v12", "v13", "v14", "v15"
);
