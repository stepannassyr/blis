// ---------------------------------------------------------
// 1. Core Preprocessor Evaluation & Stringification
// ---------------------------------------------------------
#define STR_I(x) #x
#define STR(x) STR_I(x)

#define PASTE3_I(a, b, c) a ## _ ## b ## _ ## c
#define PASTE3(a, b, c) PASTE3_I(a, b, c)

// ADD(i, d) maps to ADD_i_d
#define ADD(i, d) PASTE3(ADD, i, d)

// MOD_EVAL evaluates the sum before passing it to the MOD table
#define MOD_EVAL(sum, max) PASTE3(MOD, sum, max)
#define ADD_MOD(i, dist, max) MOD_EVAL(ADD(i, dist), max)

// ---------------------------------------------------------
// 2. Addition Lookup Table (I: 0..15, DIST: 0..8)
// ---------------------------------------------------------
// I = 0
#define ADD_0_0 0
#define ADD_0_1 1
#define ADD_0_2 2
#define ADD_0_3 3
#define ADD_0_4 4
#define ADD_0_5 5
#define ADD_0_6 6
#define ADD_0_7 7
#define ADD_0_8 8
// I = 1
#define ADD_1_0 1
#define ADD_1_1 2
#define ADD_1_2 3
#define ADD_1_3 4
#define ADD_1_4 5
#define ADD_1_5 6
#define ADD_1_6 7
#define ADD_1_7 8
#define ADD_1_8 9
// I = 2
#define ADD_2_0 2
#define ADD_2_1 3
#define ADD_2_2 4
#define ADD_2_3 5
#define ADD_2_4 6
#define ADD_2_5 7
#define ADD_2_6 8
#define ADD_2_7 9
#define ADD_2_8 10
// I = 3
#define ADD_3_0 3
#define ADD_3_1 4
#define ADD_3_2 5
#define ADD_3_3 6
#define ADD_3_4 7
#define ADD_3_5 8
#define ADD_3_6 9
#define ADD_3_7 10
#define ADD_3_8 11
// I = 4
#define ADD_4_0 4
#define ADD_4_1 5
#define ADD_4_2 6
#define ADD_4_3 7
#define ADD_4_4 8
#define ADD_4_5 9
#define ADD_4_6 10
#define ADD_4_7 11
#define ADD_4_8 12
// I = 5
#define ADD_5_0 5
#define ADD_5_1 6
#define ADD_5_2 7
#define ADD_5_3 8
#define ADD_5_4 9
#define ADD_5_5 10
#define ADD_5_6 11
#define ADD_5_7 12
#define ADD_5_8 13
// I = 6
#define ADD_6_0 6
#define ADD_6_1 7
#define ADD_6_2 8
#define ADD_6_3 9
#define ADD_6_4 10
#define ADD_6_5 11
#define ADD_6_6 12
#define ADD_6_7 13
#define ADD_6_8 14
// I = 7
#define ADD_7_0 7
#define ADD_7_1 8
#define ADD_7_2 9
#define ADD_7_3 10
#define ADD_7_4 11
#define ADD_7_5 12
#define ADD_7_6 13
#define ADD_7_7 14
#define ADD_7_8 15
// I = 8
#define ADD_8_0 8
#define ADD_8_1 9
#define ADD_8_2 10
#define ADD_8_3 11
#define ADD_8_4 12
#define ADD_8_5 13
#define ADD_8_6 14
#define ADD_8_7 15
#define ADD_8_8 16
// I = 9
#define ADD_9_0 9
#define ADD_9_1 10
#define ADD_9_2 11
#define ADD_9_3 12
#define ADD_9_4 13
#define ADD_9_5 14
#define ADD_9_6 15
#define ADD_9_7 16
#define ADD_9_8 17
// I = 10
#define ADD_10_0 10
#define ADD_10_1 11
#define ADD_10_2 12
#define ADD_10_3 13
#define ADD_10_4 14
#define ADD_10_5 15
#define ADD_10_6 16
#define ADD_10_7 17
#define ADD_10_8 18
// I = 11
#define ADD_11_0 11
#define ADD_11_1 12
#define ADD_11_2 13
#define ADD_11_3 14
#define ADD_11_4 15
#define ADD_11_5 16
#define ADD_11_6 17
#define ADD_11_7 18
#define ADD_11_8 19
// I = 12
#define ADD_12_0 12
#define ADD_12_1 13
#define ADD_12_2 14
#define ADD_12_3 15
#define ADD_12_4 16
#define ADD_12_5 17
#define ADD_12_6 18
#define ADD_12_7 19
#define ADD_12_8 20
// I = 13
#define ADD_13_0 13
#define ADD_13_1 14
#define ADD_13_2 15
#define ADD_13_3 16
#define ADD_13_4 17
#define ADD_13_5 18
#define ADD_13_6 19
#define ADD_13_7 20
#define ADD_13_8 21
// I = 14
#define ADD_14_0 14
#define ADD_14_1 15
#define ADD_14_2 16
#define ADD_14_3 17
#define ADD_14_4 18
#define ADD_14_5 19
#define ADD_14_6 20
#define ADD_14_7 21
#define ADD_14_8 22
// I = 15
#define ADD_15_0 15
#define ADD_15_1 16
#define ADD_15_2 17
#define ADD_15_3 18
#define ADD_15_4 19
#define ADD_15_5 20
#define ADD_15_6 21
#define ADD_15_7 22
#define ADD_15_8 23

// ---------------------------------------------------------
// 3. Modulo Lookup Table (SUM: 0..23, MAX: 2,4,6,8,10,12,14,16)
// ---------------------------------------------------------
// MAX = 2
#define MOD_0_2 0
#define MOD_1_2 1
#define MOD_2_2 0
#define MOD_3_2 1
#define MOD_4_2 0
#define MOD_5_2 1
#define MOD_6_2 0
#define MOD_7_2 1
#define MOD_8_2 0
#define MOD_9_2 1
#define MOD_10_2 0
#define MOD_11_2 1
#define MOD_12_2 0
#define MOD_13_2 1
#define MOD_14_2 0
#define MOD_15_2 1
#define MOD_16_2 0
#define MOD_17_2 1
#define MOD_18_2 0
#define MOD_19_2 1
#define MOD_20_2 0
#define MOD_21_2 1
#define MOD_22_2 0
#define MOD_23_2 1

// MAX = 4
#define MOD_0_4 0
#define MOD_1_4 1
#define MOD_2_4 2
#define MOD_3_4 3
#define MOD_4_4 0
#define MOD_5_4 1
#define MOD_6_4 2
#define MOD_7_4 3
#define MOD_8_4 0
#define MOD_9_4 1
#define MOD_10_4 2
#define MOD_11_4 3
#define MOD_12_4 0
#define MOD_13_4 1
#define MOD_14_4 2
#define MOD_15_4 3
#define MOD_16_4 0
#define MOD_17_4 1
#define MOD_18_4 2
#define MOD_19_4 3
#define MOD_20_4 0
#define MOD_21_4 1
#define MOD_22_4 2
#define MOD_23_4 3

// MAX = 6
#define MOD_0_6 0
#define MOD_1_6 1
#define MOD_2_6 2
#define MOD_3_6 3
#define MOD_4_6 4
#define MOD_5_6 5
#define MOD_6_6 0
#define MOD_7_6 1
#define MOD_8_6 2
#define MOD_9_6 3
#define MOD_10_6 4
#define MOD_11_6 5
#define MOD_12_6 0
#define MOD_13_6 1
#define MOD_14_6 2
#define MOD_15_6 3
#define MOD_16_6 4
#define MOD_17_6 5
#define MOD_18_6 0
#define MOD_19_6 1
#define MOD_20_6 2
#define MOD_21_6 3
#define MOD_22_6 4
#define MOD_23_6 5

// MAX = 8
#define MOD_0_8 0
#define MOD_1_8 1
#define MOD_2_8 2
#define MOD_3_8 3
#define MOD_4_8 4
#define MOD_5_8 5
#define MOD_6_8 6
#define MOD_7_8 7
#define MOD_8_8 0
#define MOD_9_8 1
#define MOD_10_8 2
#define MOD_11_8 3
#define MOD_12_8 4
#define MOD_13_8 5
#define MOD_14_8 6
#define MOD_15_8 7
#define MOD_16_8 0
#define MOD_17_8 1
#define MOD_18_8 2
#define MOD_19_8 3
#define MOD_20_8 4
#define MOD_21_8 5
#define MOD_22_8 6
#define MOD_23_8 7

// MAX = 10
#define MOD_0_10 0
#define MOD_1_10 1
#define MOD_2_10 2
#define MOD_3_10 3
#define MOD_4_10 4
#define MOD_5_10 5
#define MOD_6_10 6
#define MOD_7_10 7
#define MOD_8_10 8
#define MOD_9_10 9
#define MOD_10_10 0
#define MOD_11_10 1
#define MOD_12_10 2
#define MOD_13_10 3
#define MOD_14_10 4
#define MOD_15_10 5
#define MOD_16_10 6
#define MOD_17_10 7
#define MOD_18_10 8
#define MOD_19_10 9
#define MOD_20_10 0
#define MOD_21_10 1
#define MOD_22_10 2
#define MOD_23_10 3

// MAX = 12
#define MOD_0_12 0
#define MOD_1_12 1
#define MOD_2_12 2
#define MOD_3_12 3
#define MOD_4_12 4
#define MOD_5_12 5
#define MOD_6_12 6
#define MOD_7_12 7
#define MOD_8_12 8
#define MOD_9_12 9
#define MOD_10_12 10
#define MOD_11_12 11
#define MOD_12_12 0
#define MOD_13_12 1
#define MOD_14_12 2
#define MOD_15_12 3
#define MOD_16_12 4
#define MOD_17_12 5
#define MOD_18_12 6
#define MOD_19_12 7
#define MOD_20_12 8
#define MOD_21_12 9
#define MOD_22_12 10
#define MOD_23_12 11

// MAX = 14
#define MOD_0_14 0
#define MOD_1_14 1
#define MOD_2_14 2
#define MOD_3_14 3
#define MOD_4_14 4
#define MOD_5_14 5
#define MOD_6_14 6
#define MOD_7_14 7
#define MOD_8_14 8
#define MOD_9_14 9
#define MOD_10_14 10
#define MOD_11_14 11
#define MOD_12_14 12
#define MOD_13_14 13
#define MOD_14_14 0
#define MOD_15_14 1
#define MOD_16_14 2
#define MOD_17_14 3
#define MOD_18_14 4
#define MOD_19_14 5
#define MOD_20_14 6
#define MOD_21_14 7
#define MOD_22_14 8
#define MOD_23_14 9

// MAX = 16
#define MOD_0_16 0
#define MOD_1_16 1
#define MOD_2_16 2
#define MOD_3_16 3
#define MOD_4_16 4
#define MOD_5_16 5
#define MOD_6_16 6
#define MOD_7_16 7
#define MOD_8_16 8
#define MOD_9_16 9
#define MOD_10_16 10
#define MOD_11_16 11
#define MOD_12_16 12
#define MOD_13_16 13
#define MOD_14_16 14
#define MOD_15_16 15
#define MOD_16_16 0
#define MOD_17_16 1
#define MOD_18_16 2
#define MOD_19_16 3
#define MOD_20_16 4
#define MOD_21_16 5
#define MOD_22_16 6
#define MOD_23_16 7


// ---------------------------------------------------------
// 4. Compute Block Implementation Macros
// ---------------------------------------------------------

#define VECx2_LD_TRANSFORM_ADD(XVL1, XVL2, XVT1, XVT2, XPTR1, XPTR2, STEPREG)\
    VLOADX(XVL1, XPTR1)\
    VLOADX(XVL2, XPTR2)\
    VTRANSFORM(XVT1, XVT1)\
    VTRANSFORM(XVT2, XVT2)\
    "add " XPTR1 ", " XPTR1 ", " STEPREG "\n\t"\
    "add " XPTR2 ", " XPTR2 ", " STEPREG "\n\t"

#define COMPUTEBLOCK_P_2(I1, I2, PRELOAD_DIST, MAX_REGS)\
    VECx2_LD_TRANSFORM_ADD("v" STR(I1), "v" STR(I2),\
            "v" STR(ADD_MOD(I1, PRELOAD_DIST, MAX_REGS)), \
            "v" STR(ADD_MOD(I2, PRELOAD_DIST, MAX_REGS)), \
            "%[xptr]", \
            "%[xptr2]", \
            "%[xvstride]")

#define COMPUTEBLOCK2(PRELOAD_DIST)\
    COMPUTEBLOCK_P_2(0,1,PRELOAD_DIST, 2)

#define COMPUTEBLOCK4(PRELOAD_DIST)\
    COMPUTEBLOCK_P_2(0,1,PRELOAD_DIST, 4)\
    COMPUTEBLOCK_P_2(2,3,PRELOAD_DIST, 4)

#define COMPUTEBLOCK6(PRELOAD_DIST)\
    COMPUTEBLOCK_P_2(0,1,PRELOAD_DIST, 6)\
    COMPUTEBLOCK_P_2(2,3,PRELOAD_DIST, 6)\
    COMPUTEBLOCK_P_2(4,5,PRELOAD_DIST, 6)

#define COMPUTEBLOCK8(PRELOAD_DIST)\
    COMPUTEBLOCK_P_2(0,1,PRELOAD_DIST, 8)\
    COMPUTEBLOCK_P_2(2,3,PRELOAD_DIST, 8)\
    COMPUTEBLOCK_P_2(4,5,PRELOAD_DIST, 8)\
    COMPUTEBLOCK_P_2(6,7,PRELOAD_DIST, 8)

#define COMPUTEBLOCK10(PRELOAD_DIST)\
    COMPUTEBLOCK_P_2(0,1,PRELOAD_DIST, 10)\
    COMPUTEBLOCK_P_2(2,3,PRELOAD_DIST, 10)\
    COMPUTEBLOCK_P_2(4,5,PRELOAD_DIST, 10)\
    COMPUTEBLOCK_P_2(6,7,PRELOAD_DIST, 10)\
    COMPUTEBLOCK_P_2(8,9,PRELOAD_DIST, 10)

#define COMPUTEBLOCK12(PRELOAD_DIST)\
    COMPUTEBLOCK_P_2(0,1,PRELOAD_DIST, 12)\
    COMPUTEBLOCK_P_2(2,3,PRELOAD_DIST, 12)\
    COMPUTEBLOCK_P_2(4,5,PRELOAD_DIST, 12)\
    COMPUTEBLOCK_P_2(6,7,PRELOAD_DIST, 12)\
    COMPUTEBLOCK_P_2(8,9,PRELOAD_DIST, 12)\
    COMPUTEBLOCK_P_2(10,11,PRELOAD_DIST, 12)

#define COMPUTEBLOCK14(PRELOAD_DIST)\
    COMPUTEBLOCK_P_2(0,1,PRELOAD_DIST, 14)\
    COMPUTEBLOCK_P_2(2,3,PRELOAD_DIST, 14)\
    COMPUTEBLOCK_P_2(4,5,PRELOAD_DIST, 14)\
    COMPUTEBLOCK_P_2(6,7,PRELOAD_DIST, 14)\
    COMPUTEBLOCK_P_2(8,9,PRELOAD_DIST, 14)\
    COMPUTEBLOCK_P_2(10,11,PRELOAD_DIST, 14)\
    COMPUTEBLOCK_P_2(12,13,PRELOAD_DIST, 14)

#define COMPUTEBLOCK16(PRELOAD_DIST)\
    COMPUTEBLOCK_P_2(0,1,PRELOAD_DIST, 16)\
    COMPUTEBLOCK_P_2(2,3,PRELOAD_DIST, 16)\
    COMPUTEBLOCK_P_2(4,5,PRELOAD_DIST, 16)\
    COMPUTEBLOCK_P_2(6,7,PRELOAD_DIST, 16)\
    COMPUTEBLOCK_P_2(8,9,PRELOAD_DIST, 16)\
    COMPUTEBLOCK_P_2(10,11,PRELOAD_DIST, 16)\
    COMPUTEBLOCK_P_2(12,13,PRELOAD_DIST, 16)\
    COMPUTEBLOCK_P_2(14,15,PRELOAD_DIST, 16)

#define PACK_NV(N, PRELOAD_DIST)\
uint64_t xptr2;\
uint64_t yptr2;\
uint64_t xvstride;\
uint64_t yvstride;\
uint64_t counter;\
uint64_t unroll;\
uint64_t xfinoff;\
uint64_t yfinoff;\
uint64_t yptrprefetch;\
__asm__ (\
    PREPARE_SCALAR\
    "vsetvli %[vlen], %[vlen], e" SIZEBITS ", m1, ta, ma\n\t"\
\
    /* unroll = vlen */\
    MAKEUNROLL("%[vlen]", "1", ID)\
    "divu %[counter], %[n], %[unroll]\n\t"\
    /* put remainder back into n */\
    "remu %[n], %[n], %[unroll]\n\t"\
\
    PREPARE_STRIDEX("%[xvstride]", "%[xstride1]", SIZESHIFT)\
    PREPARE_STRIDEY("%[yvstride]", "%[ystride1]", SIZESHIFT)\
\
    PREPARE_LDIMX("%[xvstride]", "%[ldimx]", SIZESHIFT)\
    PREPARE_LDIMY("%[yvstride]", "%[ldimy]", SIZESHIFT)\
\
\
    "add %[xptr2], %[xptr], %[xvstride]\n\t"\
    "add %[yptr2], %[yptr], %[yvstride]\n\t"\
\
\
    /* ystride1 should have nr*sizeof(scalar) at this point */\
    "srli %[xfinoff], %[ystride1], " SIZESHIFT "\n\t"\
    /* nr*lda */\
    "mul %[xfinoff], %[xfinoff], %[xvstride]\n\t"\
\
    /* nr*lda-vlen */\
    /* "slli %[yfinoff], %[vlen], " SIZESHIFT "\n\t" */\
    /* "mul %[yfinoff], %[vlen], %[xstride1]\n\t" */\
    CALC_VOFFSET\
    "sub %[xfinoff], %[xfinoff], %[yfinoff]\n\t"\
\
    /* vlen-1 */\
    "add %[yfinoff], %[vlen], -1\n\t"\
    /* (vlen-1)*nr */\
    "mul %[yfinoff], %[yfinoff], %[ystride1]\n\t"\
\
    LDIMFIXUP("slli %[yvstride], %[yvstride], 1\n\t")\
    LDIMFIXUP("slli %[xvstride], %[xvstride], 1\n\t")\
\
\
    "beq %[counter], zero, ." LABELPREFIX "fullvend%=\n\t"\
    "." LABELPREFIX "fullvloop%=:\n\t"\
\
        "add %[yptrprefetch], %[yptr], %[yfinoff]\n\t"\
        "add %[yptrprefetch], %[yptrprefetch], %[ystride1]\n\t"\
        "prefetch.w 0(%[yptrprefetch])\n\t"\
\
        COMPUTEBLOCK # N(PRELOAD_DIST)\
\
        "sub %[xptr], %[xptr], %[xfinoff]\n\t"\
        "sub %[xptr2], %[xptr2], %[xfinoff]\n\t"\
\
        "add %[yptr], %[yptr], %[yfinoff]\n\t"\
        "add %[yptr2], %[yptr2], %[yfinoff]\n\t"\
\
        "add %[counter], %[counter], -1\n\t"\
        "bnez %[counter], ." LABELPREFIX "fullvloop%=\n\t"\
\
    "." LABELPREFIX "fullvend%=:\n\t"\
    "mv %[counter], %[n]\n\t" /* nleft */ \
    "beq %[counter], zero, ." LABELPREFIX "end%=\n\t"\
    "." LABELPREFIX "loop%=:\n\t"\
\
        "vsetvli %[vlen], %[counter], e" SIZEBITS ", m1, ta, ma\n\t"\
        PREPARE_LDIMX("%[xvstride]", "%[ldimx]", SIZESHIFT)\
        PREPARE_LDIMY("%[yvstride]", "%[ldimy]", SIZESHIFT)\
\
        "add %[xptr2], %[xptr], %[xvstride]\n\t"\
        "add %[yptr2], %[yptr], %[yvstride]\n\t"\
\
        /* ystride1 should have nr*sizeof(scalar) at this point */\
        "srli %[xfinoff], %[ystride1], " SIZESHIFT "\n\t"\
        /* nr*lda */\
        "mul %[xfinoff], %[xfinoff], %[xvstride]\n\t"\
\
        /* nr*lda-vlen */\
        /* "slli %[yfinoff], %[vlen], " SIZESHIFT "\n\t" */\
        /* "mul %[yfinoff], %[vlen], %[xstride1]\n\t" */\
        CALC_VOFFSET\
        "sub %[xfinoff], %[xfinoff], %[yfinoff]\n\t"\
\
        /* vlen-1 */\
        "add %[yfinoff], %[vlen], -1\n\t"\
        /* (vlen-1)*nr */\
        "mul %[yfinoff], %[yfinoff], %[ystride1]\n\t"\
\
        LDIMFIXUP("slli %[yvstride], %[yvstride], 1\n\t")\
        LDIMFIXUP("slli %[xvstride], %[xvstride], 1\n\t")\
\
        COMPUTEBLOCK # N(PRELOAD_DIST)\
\
        "sub %[xptr], %[xptr], %[xfinoff]\n\t"\
        "sub %[xptr2], %[xptr2], %[xfinoff]\n\t"\
\
        "add %[yptr], %[yptr], %[yfinoff]\n\t"\
        "add %[yptr2], %[yptr2], %[yfinoff]\n\t"\
\
        "sub %[counter], %[counter], %[vlen]\n\t"\
        "bnez %[counter], ." LABELPREFIX "loop%=\n\t"\
    "." LABELPREFIX "end%=:\n\t"\
    : [dummy_y] "+m"(*(double(*)[])y),\
      [xptr] "+r" (x), [xptr2] "=r" (xptr2), [yptr] "+r" (y), [yptr2] "=r" (yptr2),\
      [xvstride] "=r" (xvstride), [yvstride] "=r" (yvstride),\
      [counter] "=r" (counter), [n] "+r" (n),\
      [vlen] "+r" (vlen), [unroll] "=r" (unroll),\
      [ystride1] "+r" (ystride1), [xstride1] "+r" (xstride1),\
      [ldimx] "+r" (ldimx), [ldimy] "+r" (ldimy),\
      [xfinoff] "=r" (xfinoff), [yfinoff] "=r" (yfinoff),\
      [yptrprefetch] "=r" (yptrprefetch)\
    : [scalarptr] "r" (scalarptr)\
    : "f0",\
      "v0", "v1", "v2", "v3",\
      "v4", "v5", "v6", "v7",\
      "v8", "v9", "v10", "v11",\
      "v12", "v13", "v14", "v15"\
);
