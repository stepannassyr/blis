
#define COMPUTEBLOCK\
        VLOADX("v0","%[xptr]")\
        VLOADX("v1","%[xptr2]")\
        VTRANSFORM("v0", "v0")\
        VTRANSFORM("v1", "v1")\
        "add %[xptr], %[xptr], %[xvstride]\n\t"\
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"\
\
        VLOADX("v2","%[xptr]")\
        VLOADX("v3","%[xptr2]")\
        VTRANSFORM("v2", "v2")\
        VTRANSFORM("v3", "v3")\
        "add %[xptr], %[xptr], %[xvstride]\n\t"\
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"\
\
        VLOADX("v4","%[xptr]")\
        VLOADX("v5","%[xptr2]")\
        VTRANSFORM("v4", "v4")\
        VTRANSFORM("v5", "v5")\
        "add %[xptr], %[xptr], %[xvstride]\n\t"\
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"\
\
        VLOADX("v6","%[xptr]")\
        VLOADX("v7","%[xptr2]")\
        VTRANSFORM("v6", "v6")\
        VTRANSFORM("v7", "v7")\
        "add %[xptr], %[xptr], %[xvstride]\n\t"\
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"\
\
        VLOADX("v8","%[xptr]")\
        VLOADX("v9","%[xptr2]")\
        VTRANSFORM("v8", "v8")\
        VTRANSFORM("v9", "v9")\
        "add %[xptr], %[xptr], %[xvstride]\n\t"\
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"\
\
        VLOADX("v10","%[xptr]")\
        VLOADX("v11","%[xptr2]")\
        VTRANSFORM("v10", "v10")\
        VTRANSFORM("v11", "v11")\
        "add %[xptr], %[xptr], %[xvstride]\n\t"\
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"\
\
        VLOADX("v12","%[xptr]")\
        VLOADX("v13","%[xptr2]")\
        VTRANSFORM("v12", "v12")\
        VTRANSFORM("v13", "v13")\
        "add %[xptr], %[xptr], %[xvstride]\n\t"\
        "add %[xptr2], %[xptr2], %[xvstride]\n\t"\
\
        VSTOREY("v0","%[yptr]")\
        VSTOREY("v1","%[yptr2]")\
        "add %[yptr], %[yptr], %[yvstride]\n\t"\
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"\
\
        VSTOREY("v2","%[yptr]")\
        VSTOREY("v3","%[yptr2]")\
        "add %[yptr], %[yptr], %[yvstride]\n\t"\
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"\
\
        VSTOREY("v4","%[yptr]")\
        VSTOREY("v5","%[yptr2]")\
        "add %[yptr], %[yptr], %[yvstride]\n\t"\
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"\
\
        VSTOREY("v6","%[yptr]")\
        VSTOREY("v7","%[yptr2]")\
        "add %[yptr], %[yptr], %[yvstride]\n\t"\
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"\
\
        VSTOREY("v8","%[yptr]")\
        VSTOREY("v9","%[yptr2]")\
        "add %[yptr], %[yptr], %[yvstride]\n\t"\
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"\
\
        VSTOREY("v10","%[yptr]")\
        VSTOREY("v11","%[yptr2]")\
        "add %[yptr], %[yptr], %[yvstride]\n\t"\
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"\
\
        VSTOREY("v12","%[yptr]")\
        VSTOREY("v13","%[yptr2]")\
        "add %[yptr], %[yptr], %[yvstride]\n\t"\
        "add %[yptr2], %[yptr2], %[yvstride]\n\t"

uint64_t xptr2;
uint64_t yptr2;
uint64_t xvstride;
uint64_t yvstride;
uint64_t counter;
uint64_t unroll;
uint64_t xfinoff;
uint64_t yfinoff;
uint64_t yptrprefetch;
__asm__ (
    PREPARE_SCALAR
    "vsetvli %[vlen], %[vlen], e64, m1, ta, ma\n\t"

    // unroll = vlen
    MAKEUNROLL("%[vlen]", "1", ID)
    "divu %[counter], %[n], %[unroll]\n\t"
    // put remainder back into n
    "remu %[n], %[n], %[unroll]\n\t"

    PREPARE_STRIDEX("%[xvstride]", "%[xstride1]", SIZESHIFT)
    PREPARE_STRIDEY("%[yvstride]", "%[ystride1]", SIZESHIFT)

    PREPARE_LDIMX("%[xvstride]", "%[ldimx]", SIZESHIFT)
    PREPARE_LDIMY("%[yvstride]", "%[ldimy]", SIZESHIFT)


    "add %[xptr2], %[xptr], %[xvstride]\n\t"
    "add %[yptr2], %[yptr], %[yvstride]\n\t"


    // ystride1 should have nr*sizeof(scalar) at this point
    "srli %[xfinoff], %[ystride1], " SIZESHIFT "\n\t"
    // nr*lda
    "mul %[xfinoff], %[xfinoff], %[xvstride]\n\t"

    // nr*lda-vlen
    "slli %[yfinoff], %[vlen], " SIZESHIFT "\n\t"
    "sub %[xfinoff], %[xfinoff], %[yfinoff]\n\t"

    // vlen-1
    "add %[yfinoff], %[vlen], -1\n\t"
    // (vlen-1)*nr
    "mul %[yfinoff], %[yfinoff], %[ystride1]\n\t"

    LDIMFIXUP("slli %[yvstride], %[yvstride], 1\n\t")
    LDIMFIXUP("slli %[xvstride], %[xvstride], 1\n\t")
    

    "beq %[counter], zero, ." LABELPREFIX "fullvend%=\n\t"
    "." LABELPREFIX "fullvloop%=:\n\t"

        "add %[yptrprefetch], %[yptr], %[yfinoff]\n\t"
        "add %[yptrprefetch], %[yptrprefetch], %[ystride1]\n\t"
        "prefetch.w 0(%[yptrprefetch])\n\t"

        COMPUTEBLOCK

        "sub %[xptr], %[xptr], %[xfinoff]\n\t"
        "sub %[xptr2], %[xptr2], %[xfinoff]\n\t"

        "add %[yptr], %[yptr], %[yfinoff]\n\t"
        "add %[yptr2], %[yptr2], %[yfinoff]\n\t"

        "add %[counter], %[counter], -1\n\t"
        "bnez %[counter], ." LABELPREFIX "fullvloop%=\n\t"

    "." LABELPREFIX "fullvend%=:\n\t"
    "mv %[counter], %[n]\n\t" // nleft
    "beq %[counter], zero, ." LABELPREFIX "end%=\n\t"
    "." LABELPREFIX "loop%=:\n\t"

        "vsetvli %[vlen], %[counter], e64, m1, ta, ma\n\t"
        PREPARE_LDIMX("%[xvstride]", "%[ldimx]", SIZESHIFT)
        PREPARE_LDIMY("%[yvstride]", "%[ldimy]", SIZESHIFT)

        "add %[xptr2], %[xptr], %[xvstride]\n\t"
        "add %[yptr2], %[yptr], %[yvstride]\n\t"

        // ystride1 should have nr*sizeof(scalar) at this point
        "srli %[xfinoff], %[ystride1], " SIZESHIFT "\n\t"
        // nr*lda
        "mul %[xfinoff], %[xfinoff], %[xvstride]\n\t"

        // nr*lda-vlen
        "slli %[yfinoff], %[vlen], " SIZESHIFT "\n\t"
        "sub %[xfinoff], %[xfinoff], %[yfinoff]\n\t"

        // vlen-1
        "add %[yfinoff], %[vlen], -1\n\t"
        // (vlen-1)*nr
        "mul %[yfinoff], %[yfinoff], %[ystride1]\n\t"
    
        LDIMFIXUP("slli %[yvstride], %[yvstride], 1\n\t")
        LDIMFIXUP("slli %[xvstride], %[xvstride], 1\n\t")

        COMPUTEBLOCK

        "sub %[xptr], %[xptr], %[xfinoff]\n\t"
        "sub %[xptr2], %[xptr2], %[xfinoff]\n\t"

        "add %[yptr], %[yptr], %[yfinoff]\n\t"
        "add %[yptr2], %[yptr2], %[yfinoff]\n\t"

        "sub %[counter], %[counter], %[vlen]\n\t"
        "bnez %[counter], ." LABELPREFIX "loop%=\n\t"
    "." LABELPREFIX "end%=:\n\t"
    : [dummy_y] "+m"(*(double(*)[])y),
      [xptr] "+r" (x), [xptr2] "=r" (xptr2), [yptr] "+r" (y), [yptr2] "=r" (yptr2),
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
