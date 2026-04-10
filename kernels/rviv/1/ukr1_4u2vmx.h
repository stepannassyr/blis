#include "../lmul_utils.h"

uint64_t yptr_store;
uint64_t xvstride;
uint64_t xlstride;
uint64_t yvstride;
uint64_t ylstride;
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

    "add %[yptr_store], %[yptr], 0\n\t"

    PREPARE_LDIMX("%[xlstride]", "%[ldimx]", SIZESHIFT)
    PREPARE_LDIMY("%[ylstride]", "%[ldimy]", SIZESHIFT)

    "sub %[xlstride], %[xlstride], %[xvstride]\n\t"
    "sub %[ylstride], %[ylstride], %[yvstride]\n\t"

    "beq %[counter], zero, ." LABELPREFIX "4u2vmxend%=\n\t"

    VLOADX("v" STR(LMID(0)) , "%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr]")
    "add %[xptr], %[xptr], %[xvstride]\n\t"
    "add %[yptr], %[yptr], %[yvstride]\n\t"
    VLOADX("v" STR(LMID(2)) , "%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(2)),"v" STR(LMID(3))),"%[yptr]")
    "add %[xptr], %[xptr], %[xlstride]\n\t"
    "add %[yptr], %[yptr], %[xlstride]\n\t"

    VLOADX("v" STR(LMID(4)) , "%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(4)),"v" STR(LMID(5))),"%[yptr]")
    "add %[xptr], %[xptr], %[xvstride]\n\t"
    "add %[yptr], %[yptr], %[yvstride]\n\t"
    VLOADX("v" STR(LMID(6)) , "%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(6)),"v" STR(LMID(7))),"%[yptr]")
    "add %[xptr], %[xptr], %[xlstride]\n\t"
    "add %[yptr], %[yptr], %[xlstride]\n\t"

    VLOADX("v" STR(LMID(8)) , "%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(8)),"v" STR(LMID(9))),"%[yptr]")
    "add %[xptr], %[xptr], %[xvstride]\n\t"
    "add %[yptr], %[yptr], %[yvstride]\n\t"
    VLOADX("v" STR(LMID(10)) , "%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(10)),"v" STR(LMID(11))),"%[yptr]")
    "add %[xptr], %[xptr], %[xlstride]\n\t"
    "add %[yptr], %[yptr], %[xlstride]\n\t"

    VLOADX("v" STR(LMID(12)) , "%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(12)),"v" STR(LMID(13))),"%[yptr]")
    "add %[xptr], %[xptr], %[xvstride]\n\t"
    "add %[yptr], %[yptr], %[yvstride]\n\t"
    VLOADX("v" STR(LMID(14)) , "%[xptr]")
    VLOADY(VXTOY("v" STR(LMID(14)),"v" STR(LMID(15))),"%[yptr]")
    "add %[xptr], %[xptr], %[xlstride]\n\t"
    "add %[yptr], %[yptr], %[xlstride]\n\t"


    "add %[counter], %[counter], -1\n\t"
    "beq %[counter], zero, ." LABELPREFIX "4u2vmxepilogue%=\n\t"

    "." LABELPREFIX "4u2vmxloop%=:\n\t"

        VTRANSFORM(VXTOY("v" STR(LMID(0)), "v" STR(LMID(1))), "v" STR(LMID(0)))
        VTRANSFORM(VXTOY("v" STR(LMID(2)), "v" STR(LMID(3))), "v" STR(LMID(2)))
        VTRANSFORM(VXTOY("v" STR(LMID(4)), "v" STR(LMID(5))), "v" STR(LMID(4)))
        VTRANSFORM(VXTOY("v" STR(LMID(6)), "v" STR(LMID(7))), "v" STR(LMID(6)))
        VTRANSFORM(VXTOY("v" STR(LMID(8)), "v" STR(LMID(9))), "v" STR(LMID(8)))
        VTRANSFORM(VXTOY("v" STR(LMID(10)), "v" STR(LMID(11))), "v" STR(LMID(10)))
        VTRANSFORM(VXTOY("v" STR(LMID(12)), "v" STR(LMID(13))), "v" STR(LMID(12)))
        VTRANSFORM(VXTOY("v" STR(LMID(14)), "v" STR(LMID(15))), "v" STR(LMID(14)))

        VSTOREY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(2)),"v" STR(LMID(3))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[ylstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(4)),"v" STR(LMID(5))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(6)),"v" STR(LMID(7))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[ylstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(8)),"v" STR(LMID(9))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(10)),"v" STR(LMID(11))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[ylstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(12)),"v" STR(LMID(13))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(14)),"v" STR(LMID(15))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[ylstride]\n\t"


        VLOADX("v" STR(LMID(0)) , "%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"
        VLOADX("v" STR(LMID(2)) , "%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(2)),"v" STR(LMID(3))),"%[yptr]")
        "add %[xptr], %[xptr], %[xlstride]\n\t"
        "add %[yptr], %[yptr], %[xlstride]\n\t"

        VLOADX("v" STR(LMID(4)) , "%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(4)),"v" STR(LMID(5))),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"
        VLOADX("v" STR(LMID(6)) , "%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(6)),"v" STR(LMID(7))),"%[yptr]")
        "add %[xptr], %[xptr], %[xlstride]\n\t"
        "add %[yptr], %[yptr], %[xlstride]\n\t"

        VLOADX("v" STR(LMID(8)) , "%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(8)),"v" STR(LMID(9))),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"
        VLOADX("v" STR(LMID(10)) , "%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(10)),"v" STR(LMID(11))),"%[yptr]")
        "add %[xptr], %[xptr], %[xlstride]\n\t"
        "add %[yptr], %[yptr], %[xlstride]\n\t"

        VLOADX("v" STR(LMID(12)) , "%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(12)),"v" STR(LMID(13))),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"
        VLOADX("v" STR(LMID(14)) , "%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(14)),"v" STR(LMID(15))),"%[yptr]")
        "add %[xptr], %[xptr], %[xlstride]\n\t"
        "add %[yptr], %[yptr], %[xlstride]\n\t"

        "add %[counter], %[counter], -1\n\t"
        "bnez %[counter], ." LABELPREFIX "4u2vmxloop%=\n\t"
    "." LABELPREFIX "4u2vmxepilogue%=:\n\t"

        VTRANSFORM(VXTOY("v" STR(LMID(0)), "v" STR(LMID(1))), "v" STR(LMID(0)))
        VTRANSFORM(VXTOY("v" STR(LMID(2)), "v" STR(LMID(3))), "v" STR(LMID(2)))
        VTRANSFORM(VXTOY("v" STR(LMID(4)), "v" STR(LMID(5))), "v" STR(LMID(4)))
        VTRANSFORM(VXTOY("v" STR(LMID(6)), "v" STR(LMID(7))), "v" STR(LMID(6)))
        VTRANSFORM(VXTOY("v" STR(LMID(8)), "v" STR(LMID(9))), "v" STR(LMID(8)))
        VTRANSFORM(VXTOY("v" STR(LMID(10)), "v" STR(LMID(11))), "v" STR(LMID(10)))
        VTRANSFORM(VXTOY("v" STR(LMID(12)), "v" STR(LMID(13))), "v" STR(LMID(12)))
        VTRANSFORM(VXTOY("v" STR(LMID(14)), "v" STR(LMID(15))), "v" STR(LMID(14)))

        VSTOREY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(2)),"v" STR(LMID(3))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[ylstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(4)),"v" STR(LMID(5))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(6)),"v" STR(LMID(7))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[ylstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(8)),"v" STR(LMID(9))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(10)),"v" STR(LMID(11))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[ylstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(12)),"v" STR(LMID(13))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[yvstride]\n\t"
        VSTOREY(VXTOY("v" STR(LMID(14)),"v" STR(LMID(15))),"%[yptr_store]")
        "add %[yptr_store], %[yptr_store], %[ylstride]\n\t"

    "." LABELPREFIX "4u2vmxend%=:\n\t"
    "add %[counter], %[n], 0\n\t" // nleft
    "beq %[counter], zero, ." LABELPREFIX "1u2vmxend%=\n\t"
    "." LABELPREFIX "1u2vmxloop%=:\n\t"

        TAILPREPARE

        VLOADX("v" STR(LMID(0)),"%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        VTRANSFORM(VXTOY("v" STR(LMID(0)), "v" STR(LMID(1))), "v" STR(LMID(0)))
        VSTOREY(VXTOY("v" STR(LMID(0)),"v" STR(LMID(1))),"%[yptr_store]")
        "add %[yptr], %[yptr], %[yvstride]\n\t"
        "add %[yptr_store], %[yptr_store], %[yvstride]\n\t"

        VLOADX("v" STR(LMID(2)),"%[xptr]")
        VLOADY(VXTOY("v" STR(LMID(2)),"v" STR(LMID(3))),"%[yptr]")
        "add %[xptr], %[xptr], %[xlstride]\n\t"
        VTRANSFORM(VXTOY("v" STR(LMID(2)), "v" STR(LMID(3))), "v" STR(LMID(2)))
        VSTOREY(VXTOY("v" STR(LMID(2)),"v" STR(LMID(3))),"%[yptr_store]")
        "add %[yptr], %[yptr], %[ylstride]\n\t"
        "add %[yptr_store], %[yptr_store], %[ylstride]\n\t"

        TAILDECREMENT

        "bnez %[counter], ." LABELPREFIX "1u2vmxloop%=\n\t"
    "." LABELPREFIX "1u2vmxend%=:\n\t"
    : [dummy_y] "+m"(*(double(*)[])y),
      [xptr] "+r" (x),
      [yptr] "+r" (y),
      [yptr_store] "=r" (yptr_store),
      [xvstride] "=r" (xvstride), [yvstride] "=r" (yvstride),
      [xlstride] "=r" (xlstride), [ylstride] "=r" (ylstride),
      [counter] "=r" (counter), [n] "+r" (n),
      [vlen] "+r" (vlen), [unroll] "=r" (unroll),
      [ystride1] "+r" (ystride1), [xstride1] "+r" (xstride1),
      [ldimx] "+r" (ldimx), [ldimy] "+r" (ldimy)
    : [scalarptr] "r" (scalarptr)
    : "f0",
      "v0", "v1", "v2", "v3",
      "v4", "v5", "v6", "v7",
      "v8", "v9", "v10", "v11",
      "v12", "v13", "v14", "v15",
      "v16", "v17", "v18", "v19",
      "v20", "v21", "v22", "v23",
      "v24", "v25", "v26", "v27",
      "v28", "v29", "v30", "v31"
);
