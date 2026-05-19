#include "../lmul_utils.h"

uint64_t yptr2;
uint64_t xvstride;
uint64_t yvstride;
uint64_t counter;
uint64_t unroll;
__asm__ (
    PREPARE_SCALAR
    LMUL_SHIFT("%[vlen]")
    "vsetvli %[vlen], %[vlen], e" SIZEBITS ", m" STR(LMUL) ", ta, ma\n\t"
    
    // unroll
    // level 1 : 4*vlen
    // level 1m: 4
    MAKEUNROLL("%[vlen]","2", SHIFT)
    "divu %[counter], %[n], %[unroll]\n\t"
    // put remainder back into n
    "remu %[n], %[n], %[unroll]\n\t"

    PREPARE_STRIDEX("%[xvstride]", "%[xstride1]", SIZESHIFT)
    PREPARE_STRIDEY("%[yvstride]", "%[ystride1]", SIZESHIFT)

    "add %[yptr2], %[yptr], 0\n\t"

    PREPARE_LDIMX("%[xvstride]", "%[ldimx]", SIZESHIFT)
    PREPARE_LDIMY("%[yvstride]", "%[ldimy]", SIZESHIFT)

    "beq %[counter], zero, ." LABELPREFIX "4u1vmxend%=\n\t"

    VLOADX("v" STR(LMID(0)) , "%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr]")
    "add %[xptr], %[xptr], %[xvstride]\n\t"
    "add %[yptr], %[yptr], %[yvstride]\n\t"

    VLOADX("v" STR(LMID(2)),"%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(2)),"v" STR(LMID(3))),"%[yptr]")
    "add %[xptr], %[xptr], %[xvstride]\n\t"
    "add %[yptr], %[yptr], %[yvstride]\n\t"

    VLOADX("v" STR(LMID(4)),"%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(4)),"v" STR(LMID(5))),"%[yptr]")
    "add %[xptr], %[xptr], %[xvstride]\n\t"
    "add %[yptr], %[yptr], %[yvstride]\n\t"

    VLOADX("v" STR(LMID(6)),"%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(6)),"v" STR(LMID(7))),"%[yptr]")
    "add %[xptr], %[xptr], %[xvstride]\n\t"
    "add %[yptr], %[yptr], %[yvstride]\n\t"


    "add %[counter], %[counter], -1\n\t"
    "beq %[counter], zero, ." LABELPREFIX "4u1vmxepilogue%=\n\t"

    "." LABELPREFIX "4u1vmxloop%=:\n\t"

        VTRANSFORM(VXTOY("v" STR(LMID(0)), "v" STR(LMID(1))), "v" STR(LMID(0)))
        VTRANSFORM(VXTOY("v" STR(LMID(2)), "v" STR(LMID(3))), "v" STR(LMID(2)))
        VTRANSFORM(VXTOY("v" STR(LMID(4)), "v" STR(LMID(5))), "v" STR(LMID(4)))
        VTRANSFORM(VXTOY("v" STR(LMID(6)), "v" STR(LMID(7))), "v" STR(LMID(6)))

        VSTOREY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(2)),"v" STR(LMID(3))),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(4)),"v" STR(LMID(5))),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(6)),"v" STR(LMID(7))),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"

        VLOADX("v" STR(LMID(0)), "%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"

        VLOADX("v" STR(LMID(2)),"%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(2)),"v" STR(LMID(3))),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"

        VLOADX("v" STR(LMID(4)),"%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(4)),"v" STR(LMID(5))),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"

        VLOADX("v" STR(LMID(6)),"%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(6)),"v" STR(LMID(7))),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"

        "add %[counter], %[counter], -1\n\t"
        "bnez %[counter], ." LABELPREFIX "4u1vmxloop%=\n\t"
    "." LABELPREFIX "4u1vmxepilogue%=:\n\t"

        VTRANSFORM(VXTOY("v" STR(LMID(0)), "v" STR(LMID(1))), "v" STR(LMID(0)))

        VTRANSFORM(VXTOY("v" STR(LMID(2)), "v" STR(LMID(3))), "v" STR(LMID(2)))

        VTRANSFORM(VXTOY("v" STR(LMID(4)), "v" STR(LMID(5))), "v" STR(LMID(4)))

        VTRANSFORM(VXTOY("v" STR(LMID(6)), "v" STR(LMID(7))), "v" STR(LMID(6)))

        VSTOREY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(2)),"v" STR(LMID(3))),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(4)),"v" STR(LMID(5))),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(6)),"v" STR(LMID(7))),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"

    "." LABELPREFIX "4u1vmxend%=:\n\t"
    "add %[counter], %[n], 0\n\t" // nleft
    "beq %[counter], zero, ." LABELPREFIX "end%=\n\t"
    "." LABELPREFIX "loop%=:\n\t"

        TAILPREPARE

        VLOADX("v" STR(LMID(0)),"%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        VTRANSFORM(VXTOY("v" STR(LMID(0)), "v" STR(LMID(1))), "v" STR(LMID(0)))
        VSTOREY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr]")
        "add %[yptr], %[yptr], %[yvstride]\n\t"

        TAILDECREMENT

        "bnez %[counter], ." LABELPREFIX "loop%=\n\t"
    "." LABELPREFIX "end%=:\n\t"
    : [dummy_y] "+m"(*(double(*)[])y),
      [xptr] "+r" (x), [yptr] "+r" (y), [yptr2] "=r" (yptr2),
      [xvstride] "=r" (xvstride), [yvstride] "=r" (yvstride),
      [counter] "=r" (counter), [n] "+r" (n),
      [vlen] "+r" (vlen), [unroll] "=r" (unroll),
      [ystride1] "+r" (ystride1), [xstride1] "+r" (xstride1),
      [ldimx] "+r" (ldimx), [ldimy] "+r" (ldimy)
    : [scalarptr] "r" (scalarptr)
    : "f0",
      "v0", "v1", "v2", "v3",
      "v4", "v5", "v6", "v7",
      "v8", "v9", "v10", "v11",
      "v12", "v13", "v14", "v15"
);
