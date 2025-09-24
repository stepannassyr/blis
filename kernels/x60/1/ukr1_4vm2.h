uint64_t yptr2;
uint64_t xvstride;
uint64_t yvstride;
uint64_t counter;
uint64_t unroll;
__asm__ (
    PREPARE_SCALAR
    // LMUL=2
    "slli %[vlen], %[vlen], 1\n\t"
    "vsetvli %[vlen], %[vlen], e" SIZEBITS ", m" LMUL "\n\t"
    
    // unroll = 4*vlen (vlen was *2 for LMUL=2 before)
    MAKEUNROLL("%[vlen]","2", SHIFT)
    "divu %[counter], %[n], %[unroll]\n\t"
    // put remainder back into n
    "remu %[n], %[n], %[unroll]\n\t"

    PREPARE_STRIDEX("%[xvstride]", "%[xstride1]", SIZESHIFT)
    PREPARE_STRIDEY("%[yvstride]", "%[ystride1]", SIZESHIFT)

    "add %[yptr2], %[yptr], 0\n\t"

    PREPARE_LDIMX("%[xvstride]", "%[ldimx]", SIZESHIFT)
    PREPARE_LDIMY("%[yvstride]", "%[ldimy]", SIZESHIFT)


    "beq %[counter], zero, ." LABELPREFIX "4vm2end%=\n\t"
    "." LABELPREFIX "4vm2loop%=:\n\t"

        VLOADX("v0","%[xptr]")
        VLOADY(VXTOY("v0","v2"),"%[yptr]")
        VTRANSFORM(VXTOY("v0", "v2"), "v0")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"

        VLOADX("v4","%[xptr]")
        VLOADY(VXTOY("v4","v6"),"%[yptr]")
        VTRANSFORM(VXTOY("v4", "v6"), "v4")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"

        VLOADX("v8","%[xptr]")
        VLOADY(VXTOY("v8","v10"),"%[yptr]")
        VTRANSFORM(VXTOY("v8", "v10"), "v8")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"

        VLOADX("v12","%[xptr]")
        VLOADY(VXTOY("v12","v14"),"%[yptr]")
        VTRANSFORM(VXTOY("v12", "v14"), "v12")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        "add %[yptr], %[yptr], %[yvstride]\n\t"

        VSTOREY(VXTOY("v0","v2"),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VSTOREY(VXTOY("v4","v6"),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VSTOREY(VXTOY("v8","v10"),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"
        VSTOREY(VXTOY("v12","v14"),"%[yptr2]")
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"

        "add %[counter], %[counter], -1\n\t"
        "bnez %[counter], ." LABELPREFIX "4vm2loop%=\n\t"

    "." LABELPREFIX "4vm2end%=:\n\t"
    "add %[counter], %[n], 0\n\t" // nleft
    "beq %[counter], zero, ." LABELPREFIX "end%=\n\t"
    "." LABELPREFIX "loop%=:\n\t"

        TAILPREPARE

        VLOADX("v0","%[xptr]")
        VLOADY(VXTOY("v0","v2"),"%[yptr]")
        "add %[xptr], %[xptr], %[xvstride]\n\t"
        VTRANSFORM(VXTOY("v0", "v2"), "v0")
        VSTOREY(VXTOY("v0","v2"),"%[yptr]")
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
