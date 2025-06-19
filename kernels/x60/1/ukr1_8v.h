uint64_t xptr2;
uint64_t yptr2;
uint64_t xvstride;
uint64_t yvstride;
uint64_t counter;
uint64_t unroll;
__asm__ (
    PREPARE_SCALAR
    "vsetvli %[vlen], %[vlen], e" SIZEBITS ", m" LMUL ", ta, ma\n\t"


    // unroll = 8*vlen
    MAKEUNROLL("%[vlen]","3", SHIFT)
    "divu %[counter], %[n], %[unroll]\n\t"
    // put remainder back into n
    "remu %[n], %[n], %[unroll]\n\t"

    PREPARE_STRIDEX("%[xvstride]", "%[xstride1]", SIZESHIFT)
    PREPARE_STRIDEY("%[yvstride]", "%[ystride1]", SIZESHIFT)

    "add %[xptr2], %[xptr], %[xvstride]\n\t"
    "add %[yptr2], %[yptr], %[yvstride]\n\t"

    PREPARE_LDIMX("%[xvstride]", "%[ldimx]", SIZESHIFT)
    PREPARE_LDIMY("%[yvstride]", "%[ldimy]", SIZESHIFT)

    LDIMFIXUP("slli %[yvstride], %[yvstride], 1\n\t")
    LDIMFIXUP("slli %[xvstride], %[xvstride], 1\n\t")

    "beq %[counter], zero, ." LABELPREFIX "8end%=\n\t"
    "." LABELPREFIX "8loop%=:\n\t"

        VLOADX("v0","%[xptr]")
        VLOADX("v1","%[xptr2]")
        VLOADY(VXTOY("v0","v2"),"%[yptr]")
        VLOADY(VXTOY("v1","v3"),"%[yptr2]")
        VTRANSFORM(VXTOY("v0", "v2"), "v0")
        VTRANSFORM(VXTOY("v1", "v3"), "v1")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"
        VSTOREY(VXTOY("v0","v2"),"%[yptr]")
        VSTOREY(VXTOY("v1","v3"),"%[yptr2]")

        VLOADX("v4","%[xptr]")
        VLOADX("v5","%[xptr2]")
        "add %[yptr], %[yptr], %[yvstride]\n\t"
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VLOADY(VXTOY("v4","v6"),"%[yptr]")
        VLOADY(VXTOY("v5","v7"),"%[yptr2]")
        VTRANSFORM(VXTOY("v4", "v6"), "v4")
        VTRANSFORM(VXTOY("v5", "v7"), "v5")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"
        VSTOREY(VXTOY("v4","v6"),"%[yptr]")
        VSTOREY(VXTOY("v5","v7"),"%[yptr2]")

        VLOADX("v8","%[xptr]")
        VLOADX("v9","%[xptr2]")
        "add %[yptr], %[yptr], %[yvstride]\n\t"
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VLOADY(VXTOY("v8","v10"),"%[yptr]")
        VLOADY(VXTOY("v9","v11"),"%[yptr2]")
        VTRANSFORM(VXTOY("v8", "v10"), "v8")
        VTRANSFORM(VXTOY("v9", "v11"), "v9")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"
        VSTOREY(VXTOY("v8","v10"),"%[yptr]")
        VSTOREY(VXTOY("v9","v11"),"%[yptr2]")

        VLOADX("v12","%[xptr]")
        VLOADX("v13","%[xptr2]")
        "add %[yptr], %[yptr], %[yvstride]\n\t"
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VLOADY(VXTOY("v12","v14"),"%[yptr]")
        VLOADY(VXTOY("v13","v15"),"%[yptr2]")
        VTRANSFORM(VXTOY("v12", "v14"), "v12")
        VTRANSFORM(VXTOY("v13", "v15"), "v13")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"
        VSTOREY(VXTOY("v12","v14"),"%[yptr]")
        VSTOREY(VXTOY("v13","v15"),"%[yptr2]")

        "add %[yptr], %[yptr], %[yvstride]\n\t"
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"

        "add %[counter], %[counter], -1\n\t"
        "bnez %[counter], ." LABELPREFIX "8loop%=\n\t"

    "." LABELPREFIX "8end%=:\n\t"
    "add %[counter], %[n], 0\n\t" // nleft
    "beq %[counter], zero, ." LABELPREFIX "end%=\n\t"
    "." LABELPREFIX "loop%=:\n\t"
        //"vsetvli %[vlen], %[counter], e" SIZEBITS ", m" LMUL "\n\t"
        //PREPARE_LDIMX("%[xvstride]", I_LDIMX, SIZESHIFT)
        //PREPARE_LDIMY("%[yvstride]", I_LDIMY, SIZESHIFT)
        TAILPREPARE

        VLOADX("v0","%[xptr]")
        VLOADY(VXTOY("v0","v2"),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        VTRANSFORM(VXTOY("v0", "v2"), "v0")
        VSTOREY(VXTOY("v0","v2"),"%[yptr]")
        "add %[yptr], %[yptr], %[yvstride]\n\t"
        //"sub %[counter], %[counter], %[vlen]\n\t"
        TAILDECREMENT
        "bnez %[counter], ." LABELPREFIX "loop%=\n\t"
    "." LABELPREFIX "end%=:\n\t"
    : [dummy_y] "+m"(*(double(*)[])y),
      [xptr] "+r" (x), [xptr2] "=r" (xptr2), [yptr] "+r" (y), [yptr2] "=r" (yptr2),
      [xvstride] "=r" (xvstride), [yvstride] "=r" (yvstride),
      [counter] "=r" (counter), [n] "+r" (n),
      [vlen] "+r" (vlen), [unroll] "=r" (unroll),
      [ystride1] "+r" (ystride1), [xstride1] "+r" (xstride1),
      [ldimx] "+r" (ldimx), [ldimy] "+r" (ldimy)
    : [scalarptr] "r" (scalarptr)
    : "v0", "v1", "v2", "v3",
      "v4", "v5", "v6", "v7",
      "v8", "v9", "v10", "v11",
      "v12", "v13", "v14", "v15"
);
