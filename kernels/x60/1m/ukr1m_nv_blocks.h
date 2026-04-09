#ifndef UKR1M_NV_BLOCKS_H
#define UKR1M_NV_BLOCKS_H

// --- Base Primitives ---
#define VIDX_PTR_1(id)

#define VIDX_PTR_2_0
#define VIDX_PTR_2_1 2
#define VIDX_PTR_2_2 
#define VIDX_PTR_2_3 2
#define VIDX_PTR_2_4
#define VIDX_PTR_2_5 2
#define VIDX_PTR_2_6
#define VIDX_PTR_2_7 2
#define VIDX_PTR_2_8 
#define VIDX_PTR_2_9 2
#define VIDX_PTR_2_10
#define VIDX_PTR_2_11 2
#define VIDX_PTR_2_12
#define VIDX_PTR_2_13 2
#define VIDX_PTR_2_14 
#define VIDX_PTR_2_15 2

#define VIDX_PTR_2(id)\
    VIDX_PTR_2_ ##id

#define VIDX_PTR_3_0
#define VIDX_PTR_3_1 2
#define VIDX_PTR_3_2 3
#define VIDX_PTR_3_3 
#define VIDX_PTR_3_4 2
#define VIDX_PTR_3_5 3
#define VIDX_PTR_3_6
#define VIDX_PTR_3_7 2
#define VIDX_PTR_3_8 3
#define VIDX_PTR_3_9 
#define VIDX_PTR_3_10 2
#define VIDX_PTR_3_11 3
#define VIDX_PTR_3_12
#define VIDX_PTR_3_13 2
#define VIDX_PTR_3_14 3
#define VIDX_PTR_3_15 

#define VIDX_PTR_3(id)\
    VIDX_PTR_3_ ##id

#define VIDX_PTR(NPTRS, id)\
    VIDX_PTR_ ##NPTRS(id)

#define ADVANCE_PTR(base, id, stride)\
    "add %[" STR(base) STR(id) "], %[" STR(base) STR(id) "], %[" STR(stride) "]\n\t"

#define ADVANCE_2_PTR(base, id1, id2, stride)\
    ADVANCE_PTR(base, id1, stride)\
    ADVANCE_PTR(base, id2, stride)

#define ADVANCE_3_PTR(base, id1, id2, id3, stride)\
    ADVANCE_PTR(base, id1, stride)\
    ADVANCE_PTR(base, id2, stride)\
    ADVANCE_PTR(base, id3, stride)

#define VLOAD_P_1(I1, NPTRS) \
    VLOADX("v" STR(I1), "%[xptr" STR(VIDX_PTR(NPTRS,I1)) "]") 

#define VLOAD_P_2(I1, I2, NPTRS) \
    VLOADX("v" STR(I1), "%[xptr" STR(VIDX_PTR(NPTRS,I1)) "]") \
    VLOADX("v" STR(I2), "%[xptr" STR(VIDX_PTR(NPTRS,I2)) "]") 

#define VLOAD_P_3(I1, I2, I3, NPTRS) \
    VLOADX("v" STR(I1), "%[xptr" STR(VIDX_PTR(NPTRS,I1)) "]") \
    VLOADX("v" STR(I2), "%[xptr" STR(VIDX_PTR(NPTRS,I2)) "]") \
    VLOADX("v" STR(I3), "%[xptr" STR(VIDX_PTR(NPTRS,I3)) "]")

#define VSTORE_P_1(I1) \
    VSTOREY("v" STR(I1), "%[yptr" STR(VIDX_PTR(1,I1)) "]")

#define VSTORE_P_2(I1, I2) \
    VSTOREY("v" STR(I1), "%[yptr" STR(VIDX_PTR(2,I1)) "]") \
    VSTOREY("v" STR(I2), "%[yptr" STR(VIDX_PTR(2,I2)) "]")

#define VSTORE_P_3(I1, I2, I3) \
    VSTOREY("v" STR(I1), "%[yptr" STR(VIDX_PTR(3,I1)) "]") \
    VSTOREY("v" STR(I2), "%[yptr" STR(VIDX_PTR(3,I2)) "]") \
    VSTOREY("v" STR(I3), "%[yptr" STR(VIDX_PTR(3,I3)) "]")

#define VCOMP_P_1(I1) \
    VTRANSFORM("v" STR(I1), "v" STR(I1))

#define VCOMP_P_2(I1, I2) \
    VTRANSFORM("v" STR(I1), "v" STR(I1)) \
    VTRANSFORM("v" STR(I2), "v" STR(I2))

#define VCOMP_P_3(I1, I2, I3) \
    VTRANSFORM("v" STR(I1), "v" STR(I1)) \
    VTRANSFORM("v" STR(I2), "v" STR(I2)) \
    VTRANSFORM("v" STR(I3), "v" STR(I3))


#define VLOAD_COMP_P_1(L1, C1, NPTRS) \
    VLOAD_P_1(L1, NPTRS) \
    VCOMP_P_1(C1) \
    ADVANCE_PTR(xptr,\
        VIDX_PTR(NPTRS, L1), \
        xvstride)

#define VLOAD_COMP_P_2(L1, L2, C1, C2, NPTRS) \
    VLOAD_P_2(L1, L2, NPTRS) \
    VCOMP_P_2(C1, C2) \
    ADVANCE_2_PTR(xptr,\
        VIDX_PTR(NPTRS, L1),\
        VIDX_PTR(NPTRS, L2),\
        xvstride)

#define VLOAD_COMP_P_3(L1, L2, L3, C1, C2, C3, NPTRS) \
    VLOAD_P_3(L1, L2, L3, NPTRS) \
    VCOMP_P_3(C1, C2, C3) \
    ADVANCE_3_PTR(xptr, \
        VIDX_PTR(NPTRS, L1),\
        VIDX_PTR(NPTRS, L2),\
        VIDX_PTR(NPTRS, L3),\
        xvstride)

// --- Helper Dispatchers ---
#define PRELOADBLOCK_I(dist, nptrs) PRELOADBLOCK_##dist##_##nptrs
#define PRELOADBLOCK(dist, nptrs) PRELOADBLOCK_I(dist, nptrs)

#define BODYBLOCK_I(cdim, dist) BODYBLOCK_##cdim##_##dist
#define BODYBLOCK(cdim, dist) BODYBLOCK_I(cdim, dist)

#define STOREBLOCK_I(cdim) STOREBLOCK_##cdim
#define STOREBLOCK(cdim) STOREBLOCK_I(cdim)

// --- Prologue Blocks ---

#define PRELOADBLOCK_0 /* empty */

// NPTRS == 1
#define PRELOADBLOCK_1_1\
    VLOAD_P_1(0, 1)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_2_1\
    PRELOADBLOCK_1_1\
    VLOAD_P_1(1, 1)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_3_1\
    PRELOADBLOCK_2_1\
    VLOAD_P_1(2, 1)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_4_1\
    PRELOADBLOCK_3_1\
    VLOAD_P_1(3, 1)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_5_1\
    PRELOADBLOCK_4_1\
    VLOAD_P_1(4, 1)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_6_1\
    PRELOADBLOCK_5_1\
    VLOAD_P_1(5, 1)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_7_1\
    PRELOADBLOCK_6_1\
    VLOAD_P_1(6, 1)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_8_1\
    PRELOADBLOCK_7_1\
    VLOAD_P_1(7, 1)\
    ADVANCE_PTR(xptr, , xvstride)

// NPTRS == 2
#define PRELOADBLOCK_1_2\
    VLOAD_P_1(0, 2)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_2_2\
    VLOAD_P_2(0, 1, 2)\
    ADVANCE_2_PTR(xptr, , 2, xvstride)
#define PRELOADBLOCK_3_2\
    PRELOADBLOCK_2_2\
    VLOAD_P_1(2, 2)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_4_2\
    PRELOADBLOCK_2_2\
    VLOAD_P_2(2, 3, 2)\
    ADVANCE_2_PTR(xptr, , 2, xvstride)
#define PRELOADBLOCK_5_2\
    PRELOADBLOCK_4_2\
    VLOAD_P_1(4, 2)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_6_2\
    PRELOADBLOCK_4_2\
    VLOAD_P_2(4, 5, 2)\
    ADVANCE_2_PTR(xptr, , 2, xvstride)
#define PRELOADBLOCK_7_2\
    PRELOADBLOCK_6_2\
    VLOAD_P_1(6, 2)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_8_2\
    PRELOADBLOCK_6_2\
    VLOAD_P_2(6, 7, 2)\
    ADVANCE_2_PTR(xptr, , 2, xvstride)

// NPTRS == 3
#define PRELOADBLOCK_1_3\
    VLOAD_P_1(0, 3)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_2_3\
    VLOAD_P_2(0, 1, 3)\
    ADVANCE_2_PTR(xptr, , 2, xvstride)
#define PRELOADBLOCK_3_3\
    VLOAD_P_3(0, 1, 2, 3)\
    ADVANCE_3_PTR(xptr, , 2, 3, xvstride)
#define PRELOADBLOCK_4_3\
    PRELOADBLOCK_3_3\
    VLOAD_P_1(3, 3)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_5_3\
    PRELOADBLOCK_3_3\
    VLOAD_P_2(3, 4, 3)\
    ADVANCE_2_PTR(xptr, , 2, xvstride)
#define PRELOADBLOCK_6_3\
    PRELOADBLOCK_3_3\
    VLOAD_P_3(3, 4, 5, 3)\
    ADVANCE_3_PTR(xptr, ,2 , 3, xvstride)
#define PRELOADBLOCK_7_3\
    PRELOADBLOCK_6_3\
    VLOAD_P_1(6, 3)\
    ADVANCE_PTR(xptr, , xvstride)
#define PRELOADBLOCK_8_3\
    PRELOADBLOCK_6_3\
    VLOAD_P_2(6, 7, 3)\
    ADVANCE_2_PTR(xptr, , 2, xvstride)


// --- Store Blocks ---
#define STOREBLOCK_1\
    VSTORE_P_1(0)\
    ADVANCE_PTR(yptr, ,yvstride)

#define STOREBLOCK_2\
    VSTORE_P_2(0, 1)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)

#define STOREBLOCK_3\
    VSTORE_P_3(0, 1, 2)\
    ADVANCE_3_PTR(yptr, , 2, 3, yvstride)

#define STOREBLOCK_4\
    VSTORE_P_2(0, 1)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)\
    VSTORE_P_2(2, 3)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)

#define STOREBLOCK_5\
    VSTORE_P_1(0)\
    ADVANCE_PTR(yptr, ,yvstride)\
    VSTORE_P_1(1)\
    ADVANCE_PTR(yptr, ,yvstride)\
    VSTORE_P_1(2)\
    ADVANCE_PTR(yptr, ,yvstride)\
    VSTORE_P_1(3)\
    ADVANCE_PTR(yptr, ,yvstride)\
    VSTORE_P_1(4)\
    ADVANCE_PTR(yptr, ,yvstride)

#define STOREBLOCK_6\
    STOREBLOCK_3\
    VSTORE_P_3(3, 4, 5)\
    ADVANCE_3_PTR(yptr, , 2, 3, yvstride)

#define STOREBLOCK_7\
    STOREBLOCK_5\
    VSTORE_P_1(5)\
    ADVANCE_PTR(yptr, ,yvstride)\
    VSTORE_P_1(6)\
    ADVANCE_PTR(yptr, ,yvstride)

#define STOREBLOCK_8\
    VSTORE_P_2(0, 1)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)\
    VSTORE_P_2(2, 3)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)\
    VSTORE_P_2(4, 5)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)\
    VSTORE_P_2(6, 7)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)

#define STOREBLOCK_9\
    STOREBLOCK_6\
    VSTORE_P_3(6, 7, 8)\
    ADVANCE_3_PTR(yptr, , 2, 3, yvstride)

#define STOREBLOCK_10\
    VSTORE_P_2(0, 1)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)\
    VSTORE_P_2(2, 3)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)\
    VSTORE_P_2(4, 5)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)\
    VSTORE_P_2(6, 7)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)\
    VSTORE_P_2(8, 9)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)

#define STOREBLOCK_11\
    STOREBLOCK_7\
    VSTORE_P_1(7)\
    ADVANCE_PTR(yptr, ,yvstride)\
    VSTORE_P_1(8)\
    ADVANCE_PTR(yptr, ,yvstride)\
    VSTORE_P_1(9)\
    ADVANCE_PTR(yptr, ,yvstride)\
    VSTORE_P_1(10)\
    ADVANCE_PTR(yptr, ,yvstride)

#define STOREBLOCK_12\
    STOREBLOCK_9\
    VSTORE_P_3(9, 10, 11)\
    ADVANCE_3_PTR(yptr, , 2, 3, yvstride)

#define STOREBLOCK_13\
    STOREBLOCK_11\
    VSTORE_P_1(11)\
    ADVANCE_PTR(yptr, ,yvstride)\
    VSTORE_P_1(12)\
    ADVANCE_PTR(yptr, ,yvstride)

#define STOREBLOCK_14\
    STOREBLOCK_10\
    VSTORE_P_2(10, 11)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)\
    VSTORE_P_2(12, 13)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)

#define STOREBLOCK_15\
    STOREBLOCK_9\
    VSTORE_P_3(9, 10, 11)\
    ADVANCE_3_PTR(yptr, , 2, 3, yvstride)\
    VSTORE_P_3(12, 13, 14)\
    ADVANCE_3_PTR(yptr, , 2, 3, yvstride)

#define STOREBLOCK_16\
    STOREBLOCK_14\
    VSTORE_P_2(14, 15)\
    ADVANCE_2_PTR(yptr, , 2, yvstride)

// --- Main Body Blocks (CDIM up to 16, DIST up to 8) ---
// Note: PRELOAD_DIST must be <= CDIM. 

// DIST = 0
#define BODYBLOCK_1_0\
    VLOAD_COMP_P_1(0, 0, 1)

#define BODYBLOCK_2_0\
    VLOAD_COMP_P_2(0, 1, 0, 1, 2)

#define BODYBLOCK_3_0\
    VLOAD_COMP_P_3(0, 1, 2, 0, 1, 2, 3)

#define BODYBLOCK_4_0\
    VLOAD_COMP_P_2(0, 1, 0, 1, 2)\
    VLOAD_COMP_P_2(2, 3, 2, 3, 2)

#define BODYBLOCK_5_0\
    VLOAD_COMP_P_1(0, 0, 1)\
    VLOAD_COMP_P_1(1, 1, 1)\
    VLOAD_COMP_P_1(2, 2, 1)\
    VLOAD_COMP_P_1(3, 3, 1)\
    VLOAD_COMP_P_1(4, 4, 1)

#define BODYBLOCK_6_0\
    BODYBLOCK_3_0\
    VLOAD_COMP_P_3(3, 4, 5, 3, 4, 5, 3)

#define BODYBLOCK_7_0\
    BODYBLOCK_5_0\
    VLOAD_COMP_P_1(5, 5, 1)\
    VLOAD_COMP_P_1(6, 6, 1)

#define BODYBLOCK_8_0\
    BODYBLOCK_4_0\
    VLOAD_COMP_P_2(4, 5, 4, 5, 2)\
    VLOAD_COMP_P_2(6, 7, 6, 7, 2)

#define BODYBLOCK_9_0\
    BODYBLOCK_6_0\
    VLOAD_COMP_P_3(6, 7, 8, 6, 7, 8, 3)

#define BODYBLOCK_10_0\
    VLOAD_COMP_P_2(0, 1, 0, 1, 2)\
    VLOAD_COMP_P_2(2, 3, 2, 3, 2)\
    VLOAD_COMP_P_2(4, 5, 4, 5, 2)\
    VLOAD_COMP_P_2(6, 7, 6, 7, 2)\
    VLOAD_COMP_P_2(8, 9, 8, 9, 2)

#define BODYBLOCK_11_0\
    BODYBLOCK_7_0\
    VLOAD_COMP_P_1(7,  7 , 1)\
    VLOAD_COMP_P_1(8,  8 , 1)\
    VLOAD_COMP_P_1(9,  9 , 1)\
    VLOAD_COMP_P_1(10, 10, 1)

#define BODYBLOCK_12_0\
    BODYBLOCK_9_0\
    VLOAD_COMP_P_3(9, 10, 11, 9, 10, 11, 3)

#define BODYBLOCK_13_0\
    BODYBLOCK_11_0\
    VLOAD_COMP_P_1(11, 11, 1)\
    VLOAD_COMP_P_1(12, 12, 1)

#define BODYBLOCK_14_0\
    BODYBLOCK_10_0\
    VLOAD_COMP_P_2(10, 11, 10, 11, 2)\
    VLOAD_COMP_P_2(12, 13, 12, 13, 2)

#define BODYBLOCK_15_0\
    BODYBLOCK_12_0\
    VLOAD_COMP_P_3(12, 13, 14, 12, 13, 14, 3)

#define BODYBLOCK_16_0\
    BODYBLOCK_14_0\
    VLOAD_COMP_P_2(14, 15, 14, 15, 2)

// DIST = 1
#define BODYBLOCK_1_1\
    VCOMP_P_1(0)

#define BODYBLOCK_2_1\
    VLOAD_P_1(1, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 1), xvstride)\
    VCOMP_P_2(0, 1)

#define BODYBLOCK_3_1\
    VLOAD_P_2(1, 2, 3)\
    ADVANCE_2_PTR(xptr,\
        VIDX_PTR(3, 1),\
        VIDX_PTR(3, 2),\
        xvstride)\
    VCOMP_P_3(0, 1, 2)

#define BODYBLOCK_4_1\
    VLOAD_COMP_P_2(1, 2, 0, 1, 2)\
    VLOAD_P_1(3, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 3), xvstride)\
    VCOMP_P_2(2, 3)

#define BODYBLOCK_5_1\
    VLOAD_COMP_P_1(1, 0, 1)\
    VLOAD_COMP_P_1(2, 1, 1)\
    VLOAD_COMP_P_1(3, 2, 1)\
    VLOAD_COMP_P_1(4, 3, 1)\
    VCOMP_P_1(4)

#define BODYBLOCK_6_1\
    VLOAD_COMP_P_3(1,2,3, 0,1,2, 3)\
    VLOAD_P_2(4, 5, 3)\
    ADVANCE_2_PTR(xptr,\
        VIDX_PTR(3, 4),\
        VIDX_PTR(3, 5),\
        xvstride)\
    VCOMP_P_3(3, 4, 5)

#define BODYBLOCK_7_1\
    VLOAD_COMP_P_1(1, 0, 1)\
    VLOAD_COMP_P_1(2, 1, 1)\
    VLOAD_COMP_P_1(3, 2, 1)\
    VLOAD_COMP_P_1(4, 3, 1)\
    VLOAD_COMP_P_1(5, 3, 1)\
    VLOAD_COMP_P_1(6, 3, 1)\
    VCOMP_P_1(6)

#define BODYBLOCK_8_1\
    VLOAD_COMP_P_2(1,2, 0,1, 2)\
    VLOAD_COMP_P_2(3,4, 2,3, 2)\
    VLOAD_COMP_P_2(5,6, 4,5, 2)\
    VLOAD_P_1(7, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 7), xvstride)\
    VCOMP_P_2(6, 7)

#define BODYBLOCK_9_1\
    VLOAD_COMP_P_3(1,2,3, 0,1,2, 3)\
    VLOAD_COMP_P_3(4,5,6, 3,4,5, 3)\
    VLOAD_P_2(7, 8, 3)\
    ADVANCE_2_PTR(xptr,\
        VIDX_PTR(3, 7),\
        VIDX_PTR(3, 8),\
        xvstride)\
    VCOMP_P_3(6, 7, 8)

#define BODYBLOCK_10_1\
    VLOAD_COMP_P_2(1,2, 0,1, 2)\
    VLOAD_COMP_P_2(3,4, 2,3, 2)\
    VLOAD_COMP_P_2(5,6, 4,5, 2)\
    VLOAD_COMP_P_2(7,8, 6,7, 2)\
    VLOAD_P_1(9, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 9), xvstride)\
    VCOMP_P_1(9)

#define BODYBLOCK_11_1\
    VLOAD_COMP_P_1(1, 0, 1)\
    VLOAD_COMP_P_1(2, 1, 1)\
    VLOAD_COMP_P_1(3, 2, 1)\
    VLOAD_COMP_P_1(4, 3, 1)\
    VLOAD_COMP_P_1(5, 4, 1)\
    VLOAD_COMP_P_1(6, 5, 1)\
    VLOAD_COMP_P_1(7, 6, 1)\
    VLOAD_COMP_P_1(8, 7, 1)\
    VLOAD_COMP_P_1(9, 8, 1)\
    VLOAD_COMP_P_1(10, 9, 1)\
    VCOMP_P_1(10)

#define BODYBLOCK_12_1\
    VLOAD_COMP_P_3(1,2,3, 0,1,2, 3)\
    VLOAD_COMP_P_3(4,5,6, 3,4,5, 3)\
    VLOAD_COMP_P_3(7,8,9, 6,7,8, 3)\
    VLOAD_P_2(10, 11, 3)\
    ADVANCE_2_PTR(xptr,\
        VIDX_PTR(3, 10),\
        VIDX_PTR(3, 11),\
        xvstride)\
    VCOMP_P_3(9, 10, 11)

#define BODYBLOCK_13_1\
    VLOAD_COMP_P_1(1, 0, 1)\
    VLOAD_COMP_P_1(2, 1, 1)\
    VLOAD_COMP_P_1(3, 2, 1)\
    VLOAD_COMP_P_1(4, 3, 1)\
    VLOAD_COMP_P_1(5, 4, 1)\
    VLOAD_COMP_P_1(6, 5, 1)\
    VLOAD_COMP_P_1(7, 6, 1)\
    VLOAD_COMP_P_1(8, 7, 1)\
    VLOAD_COMP_P_1(9, 8, 1)\
    VLOAD_COMP_P_1(10, 9, 1)\
    VLOAD_COMP_P_1(11, 10, 1)\
    VLOAD_COMP_P_1(12, 11, 1)\
    VCOMP_P_1(12)

#define BODYBLOCK_14_1\
    VLOAD_COMP_P_2(1,2, 0,1, 2)\
    VLOAD_COMP_P_2(3,4, 2,3, 2)\
    VLOAD_COMP_P_2(5,6, 4,5, 2)\
    VLOAD_COMP_P_2(7,8, 6,7, 2)\
    VLOAD_COMP_P_2(9,10, 8,9, 2)\
    VLOAD_COMP_P_2(11,12, 10,11, 2)\
    VLOAD_P_1(13, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 13), xvstride)\
    VCOMP_P_1(13)

#define BODYBLOCK_15_1\
    VLOAD_COMP_P_3(1,2,3,    0,1,2,   3)\
    VLOAD_COMP_P_3(4,5,6,    3,4,5,   3)\
    VLOAD_COMP_P_3(7,8,9,    6,7,8,   3)\
    VLOAD_COMP_P_3(10,11,12, 9,10,11, 3)\
    VLOAD_P_2(13, 14, 3)\
    ADVANCE_2_PTR(xptr,\
        VIDX_PTR(3, 13),\
        VIDX_PTR(3, 14),\
        xvstride)\
    VCOMP_P_3(12, 13, 14)

#define BODYBLOCK_16_1\
    VLOAD_COMP_P_2(1,2, 0,1, 2)\
    VLOAD_COMP_P_2(3,4, 2,3, 2)\
    VLOAD_COMP_P_2(5,6, 4,5, 2)\
    VLOAD_COMP_P_2(7,8, 6,7, 2)\
    VLOAD_COMP_P_2(9,10, 8,9, 2)\
    VLOAD_COMP_P_2(11,12, 10,11, 2)\
    VLOAD_COMP_P_2(13,14, 12,13, 2)\
    VLOAD_P_1(15, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 15), xvstride)\
    VCOMP_P_2(14, 15)

// ==========================================
// DIST = 2
// ==========================================
#define BODYBLOCK_2_2\
    VCOMP_P_2(0, 1)

#define BODYBLOCK_3_2\
    VLOAD_P_1(2, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 2), xvstride)\
    VCOMP_P_3(0, 1, 2)

#define BODYBLOCK_4_2\
    VLOAD_COMP_P_2(2, 3, 0, 1, 2)\
    VCOMP_P_2(2, 3)

#define BODYBLOCK_5_2\
    VLOAD_COMP_P_1(2, 0, 1) \
    VLOAD_COMP_P_1(3, 1, 1) \
    VLOAD_COMP_P_1(4, 2, 1) \
    VCOMP_P_1(3) \
    VCOMP_P_1(4)

#define BODYBLOCK_6_2\
    VLOAD_COMP_P_3(2, 3, 4, 0, 1, 2, 3)\
    VLOAD_P_1(5, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 5), xvstride)\
    VCOMP_P_3(3, 4, 5)

#define BODYBLOCK_7_2\
    VLOAD_COMP_P_1(2, 0, 1) \
    VLOAD_COMP_P_1(3, 1, 1) \
    VLOAD_COMP_P_1(4, 2, 1) \
    VLOAD_COMP_P_1(5, 3, 1) \
    VLOAD_COMP_P_1(6, 4, 1) \
    VCOMP_P_1(5) \
    VCOMP_P_1(6)

#define BODYBLOCK_8_2\
    VLOAD_COMP_P_2(2, 3, 0, 1, 2)\
    VLOAD_COMP_P_2(4, 5, 2, 3, 2)\
    VLOAD_COMP_P_2(6, 7, 4, 5, 2)\
    VCOMP_P_2(6, 7)

#define BODYBLOCK_9_2\
    VLOAD_COMP_P_3(2, 3, 4, 0, 1, 2, 3)\
    VLOAD_COMP_P_3(5, 6, 7, 3, 4, 5, 3)\
    VLOAD_P_1(8, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 8), xvstride)\
    VCOMP_P_3(6, 7, 8)

#define BODYBLOCK_10_2\
    VLOAD_COMP_P_2(2, 3, 0, 1, 2)\
    VLOAD_COMP_P_2(4, 5, 2, 3, 2)\
    VLOAD_COMP_P_2(6, 7, 4, 5, 2)\
    VLOAD_COMP_P_2(8, 9, 6, 7, 2)\
    VCOMP_P_2(8, 9)

#define BODYBLOCK_11_2\
    VLOAD_COMP_P_1(2, 0, 1) \
    VLOAD_COMP_P_1(3, 1, 1) \
    VLOAD_COMP_P_1(4, 2, 1) \
    VLOAD_COMP_P_1(5, 3, 1) \
    VLOAD_COMP_P_1(6, 4, 1) \
    VLOAD_COMP_P_1(7, 5, 1) \
    VLOAD_COMP_P_1(8, 6, 1) \
    VLOAD_COMP_P_1(9, 7, 1) \
    VLOAD_COMP_P_1(10, 8, 1) \
    VCOMP_P_1(9) \
    VCOMP_P_1(10)

#define BODYBLOCK_12_2\
    VLOAD_COMP_P_3(2, 3, 4, 0, 1, 2, 3)\
    VLOAD_COMP_P_3(5, 6, 7, 3, 4, 5, 3)\
    VLOAD_COMP_P_3(8, 9, 10, 6, 7, 8, 3)\
    VLOAD_P_1(11, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 11), xvstride)\
    VCOMP_P_3(9, 10, 11)

#define BODYBLOCK_13_2\
    BODYBLOCK_11_2 \
    VLOAD_COMP_P_1(11, 9, 1) \
    VLOAD_COMP_P_1(12, 10, 1) \
    VCOMP_P_1(11) \
    VCOMP_P_1(12)

#define BODYBLOCK_14_2\
    BODYBLOCK_10_2 \
    VLOAD_COMP_P_2(10, 11, 8, 9, 2)\
    VLOAD_COMP_P_2(12, 13, 10, 11, 2)\
    VCOMP_P_2(12, 13)

#define BODYBLOCK_15_2\
    BODYBLOCK_12_2 \
    VLOAD_COMP_P_3(11, 12, 13, 9, 10, 11, 3)\
    VLOAD_P_1(14, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 14), xvstride)\
    VCOMP_P_3(12, 13, 14)

#define BODYBLOCK_16_2\
    BODYBLOCK_14_2 \
    VLOAD_COMP_P_2(14, 15, 12, 13, 2)\
    VCOMP_P_2(14, 15)

// ==========================================
// DIST = 3
// ==========================================
#define BODYBLOCK_3_3\
    VCOMP_P_3(0, 1, 2)

#define BODYBLOCK_4_3\
    VLOAD_P_1(3, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 3), xvstride)\
    VCOMP_P_2(0, 1)\
    VCOMP_P_2(2, 3)

#define BODYBLOCK_5_3\
    VLOAD_COMP_P_1(3, 0, 1)\
    VLOAD_COMP_P_1(4, 1, 1)\
    VCOMP_P_1(2)\
    VCOMP_P_1(3)\
    VCOMP_P_1(4)

#define BODYBLOCK_6_3\
    VLOAD_COMP_P_3(3, 4, 5, 0, 1, 2, 3)\
    VCOMP_P_3(3, 4, 5)

#define BODYBLOCK_7_3\
    VLOAD_COMP_P_1(3, 0, 1)\
    VLOAD_COMP_P_1(4, 1, 1)\
    VLOAD_COMP_P_1(5, 2, 1)\
    VLOAD_COMP_P_1(6, 3, 1)\
    VCOMP_P_1(4)\
    VCOMP_P_1(5)\
    VCOMP_P_1(6)

#define BODYBLOCK_8_3\
    VLOAD_COMP_P_2(3, 4, 0, 1, 2)\
    VLOAD_COMP_P_2(5, 6, 2, 3, 2)\
    VLOAD_P_1(7, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 7), xvstride)\
    VCOMP_P_2(4, 5)\
    VCOMP_P_2(6, 7)

#define BODYBLOCK_9_3\
    VLOAD_COMP_P_3(3, 4, 5, 0, 1, 2, 3)\
    VLOAD_COMP_P_3(6, 7, 8, 3, 4, 5, 3)\
    VCOMP_P_3(6, 7, 8)

#define BODYBLOCK_10_3\
    VLOAD_COMP_P_2(3, 4, 0, 1, 2)\
    VLOAD_COMP_P_2(5, 6, 2, 3, 2)\
    VLOAD_COMP_P_2(7, 8, 4, 5, 2)\
    VLOAD_P_1(9, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 9), xvstride)\
    VCOMP_P_2(6, 7)\
    VCOMP_P_2(8, 9)

#define BODYBLOCK_11_3\
    BODYBLOCK_7_3 \
    VLOAD_COMP_P_1(7, 4, 1)\
    VLOAD_COMP_P_1(8, 5, 1)\
    VLOAD_COMP_P_1(9, 6, 1)\
    VLOAD_COMP_P_1(10, 7, 1)\
    VCOMP_P_1(8)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)

#define BODYBLOCK_12_3\
    BODYBLOCK_9_3 \
    VLOAD_COMP_P_3(9, 10, 11, 6, 7, 8, 3)\
    VCOMP_P_3(9, 10, 11)

#define BODYBLOCK_13_3\
    BODYBLOCK_11_3 \
    VLOAD_COMP_P_1(11, 8, 1)\
    VLOAD_COMP_P_1(12, 9, 1)\
    VCOMP_P_1(10)\
    VCOMP_P_1(11)\
    VCOMP_P_1(12)

#define BODYBLOCK_14_3\
    BODYBLOCK_10_3 \
    VLOAD_COMP_P_2(10, 11, 6, 7, 2)\
    VLOAD_COMP_P_2(12, 13, 8, 9, 2)\
    VCOMP_P_2(10, 11)\
    VCOMP_P_2(12, 13)

#define BODYBLOCK_15_3\
    BODYBLOCK_12_3 \
    VLOAD_COMP_P_3(12, 13, 14, 9, 10, 11, 3)\
    VCOMP_P_3(12, 13, 14)

#define BODYBLOCK_16_3\
    BODYBLOCK_14_3 \
    VLOAD_COMP_P_2(14, 15, 10, 11, 2)\
    VCOMP_P_2(12, 13)\
    VCOMP_P_2(14, 15)

// ==========================================
// DIST = 4
// ==========================================
#define BODYBLOCK_4_4\
    VCOMP_P_2(0, 1)\
    VCOMP_P_2(2, 3)

#define BODYBLOCK_5_4\
    VLOAD_COMP_P_1(4, 0, 1)\
    VCOMP_P_1(1)\
    VCOMP_P_1(2)\
    VCOMP_P_1(3)\
    VCOMP_P_1(4)

#define BODYBLOCK_6_4\
    VLOAD_P_2(4, 5, 3)\
    ADVANCE_2_PTR(xptr, VIDX_PTR(3, 4), VIDX_PTR(3, 5), xvstride)\
    VCOMP_P_3(0, 1, 2)\
    VCOMP_P_3(3, 4, 5)

#define BODYBLOCK_7_4\
    VLOAD_COMP_P_1(4, 0, 1)\
    VLOAD_COMP_P_1(5, 1, 1)\
    VLOAD_COMP_P_1(6, 2, 1)\
    VCOMP_P_1(3)\
    VCOMP_P_1(4)\
    VCOMP_P_1(5)\
    VCOMP_P_1(6)

#define BODYBLOCK_8_4\
    VLOAD_COMP_P_2(4, 5, 0, 1, 2)\
    VLOAD_COMP_P_2(6, 7, 2, 3, 2)\
    VCOMP_P_2(4, 5)\
    VCOMP_P_2(6, 7)

#define BODYBLOCK_9_4\
    VLOAD_COMP_P_3(4, 5, 6, 0, 1, 2, 3)\
    VLOAD_P_2(7, 8, 3)\
    ADVANCE_2_PTR(xptr, VIDX_PTR(3, 7), VIDX_PTR(3, 8), xvstride)\
    VCOMP_P_3(3, 4, 5)\
    VCOMP_P_3(6, 7, 8)

#define BODYBLOCK_10_4\
    VLOAD_COMP_P_2(4, 5, 0, 1, 2)\
    VLOAD_COMP_P_2(6, 7, 2, 3, 2)\
    VLOAD_COMP_P_2(8, 9, 4, 5, 2)\
    VCOMP_P_2(6, 7)\
    VCOMP_P_2(8, 9)

#define BODYBLOCK_11_4\
    BODYBLOCK_7_4 \
    VLOAD_COMP_P_1(7, 3, 1)\
    VLOAD_COMP_P_1(8, 4, 1)\
    VLOAD_COMP_P_1(9, 5, 1)\
    VLOAD_COMP_P_1(10, 6, 1)\
    VCOMP_P_1(7)\
    VCOMP_P_1(8)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)

#define BODYBLOCK_12_4\
    BODYBLOCK_9_4 \
    VLOAD_COMP_P_3(7, 8, 9, 3, 4, 5, 3)\
    VLOAD_P_2(10, 11, 3)\
    ADVANCE_2_PTR(xptr, VIDX_PTR(3, 10), VIDX_PTR(3, 11), xvstride)\
    VCOMP_P_3(6, 7, 8)\
    VCOMP_P_3(9, 10, 11)

#define BODYBLOCK_13_4\
    BODYBLOCK_11_4 \
    VLOAD_COMP_P_1(11, 7, 1)\
    VLOAD_COMP_P_1(12, 8, 1)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)\
    VCOMP_P_1(11)\
    VCOMP_P_1(12)

#define BODYBLOCK_14_4\
    BODYBLOCK_10_4 \
    VLOAD_COMP_P_2(10, 11, 6, 7, 2)\
    VLOAD_COMP_P_2(12, 13, 8, 9, 2)\
    VCOMP_P_2(10, 11)\
    VCOMP_P_2(12, 13)

#define BODYBLOCK_15_4\
    BODYBLOCK_12_4 \
    VLOAD_COMP_P_3(10, 11, 12, 6, 7, 8, 3)\
    VLOAD_P_2(13, 14, 3)\
    ADVANCE_2_PTR(xptr, VIDX_PTR(3, 13), VIDX_PTR(3, 14), xvstride)\
    VCOMP_P_3(9, 10, 11)\
    VCOMP_P_3(12, 13, 14)

#define BODYBLOCK_16_4\
    BODYBLOCK_14_4 \
    VLOAD_COMP_P_2(14, 15, 10, 11, 2)\
    VCOMP_P_2(12, 13)\
    VCOMP_P_2(14, 15)

// ==========================================
// DIST = 5
// ==========================================
#define BODYBLOCK_5_5\
    VCOMP_P_1(0)\
    VCOMP_P_1(1)\
    VCOMP_P_1(2)\
    VCOMP_P_1(3)\
    VCOMP_P_1(4)

#define BODYBLOCK_6_5\
    VLOAD_P_1(5, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 5), xvstride)\
    VCOMP_P_3(0, 1, 2)\
    VCOMP_P_3(3, 4, 5)

#define BODYBLOCK_7_5\
    VLOAD_COMP_P_1(5, 0, 1)\
    VLOAD_COMP_P_1(6, 1, 1)\
    VCOMP_P_1(2)\
    VCOMP_P_1(3)\
    VCOMP_P_1(4)\
    VCOMP_P_1(5)\
    VCOMP_P_1(6)

#define BODYBLOCK_8_5\
    VLOAD_COMP_P_2(5, 6, 0, 1, 2)\
    VLOAD_P_1(7, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 7), xvstride)\
    VCOMP_P_2(2, 3)\
    VCOMP_P_2(4, 5)\
    VCOMP_P_2(6, 7)

#define BODYBLOCK_9_5\
    VLOAD_COMP_P_3(5, 6, 7, 0, 1, 2, 3)\
    VLOAD_P_1(8, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 8), xvstride)\
    VCOMP_P_3(3, 4, 5)\
    VCOMP_P_3(6, 7, 8)

#define BODYBLOCK_10_5\
    VLOAD_COMP_P_2(5, 6, 0, 1, 2)\
    VLOAD_COMP_P_2(7, 8, 2, 3, 2)\
    VLOAD_P_1(9, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 9), xvstride)\
    VCOMP_P_2(4, 5)\
    VCOMP_P_2(6, 7)\
    VCOMP_P_2(8, 9)

#define BODYBLOCK_11_5\
    BODYBLOCK_7_5 \
    VLOAD_COMP_P_1(7, 2, 1)\
    VLOAD_COMP_P_1(8, 3, 1)\
    VLOAD_COMP_P_1(9, 4, 1)\
    VLOAD_COMP_P_1(10, 5, 1)\
    VCOMP_P_1(6)\
    VCOMP_P_1(7)\
    VCOMP_P_1(8)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)

#define BODYBLOCK_12_5\
    BODYBLOCK_9_5 \
    VLOAD_COMP_P_3(8, 9, 10, 3, 4, 5, 3)\
    VLOAD_P_1(11, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 11), xvstride)\
    VCOMP_P_3(6, 7, 8)\
    VCOMP_P_3(9, 10, 11)

#define BODYBLOCK_13_5\
    BODYBLOCK_11_5 \
    VLOAD_COMP_P_1(11, 6, 1)\
    VLOAD_COMP_P_1(12, 7, 1)\
    VCOMP_P_1(8)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)\
    VCOMP_P_1(11)\
    VCOMP_P_1(12)

#define BODYBLOCK_14_5\
    BODYBLOCK_10_5 \
    VLOAD_COMP_P_2(9, 10, 4, 5, 2)\
    VLOAD_COMP_P_2(11, 12, 6, 7, 2)\
    VLOAD_P_1(13, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 13), xvstride)\
    VCOMP_P_2(8, 9)\
    VCOMP_P_2(10, 11)\
    VCOMP_P_2(12, 13)

#define BODYBLOCK_15_5\
    BODYBLOCK_12_5 \
    VLOAD_COMP_P_3(11, 12, 13, 6, 7, 8, 3)\
    VLOAD_P_1(14, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 14), xvstride)\
    VCOMP_P_3(9, 10, 11)\
    VCOMP_P_3(12, 13, 14)

#define BODYBLOCK_16_5\
    BODYBLOCK_14_5 \
    VLOAD_COMP_P_2(13, 14, 8, 9, 2)\
    VLOAD_P_1(15, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 15), xvstride)\
    VCOMP_P_2(10, 11)\
    VCOMP_P_2(12, 13)\
    VCOMP_P_2(14, 15)

// ==========================================
// DIST = 6
// ==========================================
#define BODYBLOCK_6_6\
    VCOMP_P_3(0, 1, 2)\
    VCOMP_P_3(3, 4, 5)

#define BODYBLOCK_7_6\
    VLOAD_COMP_P_1(6, 0, 1)\
    VCOMP_P_1(1)\
    VCOMP_P_1(2)\
    VCOMP_P_1(3)\
    VCOMP_P_1(4)\
    VCOMP_P_1(5)\
    VCOMP_P_1(6)

#define BODYBLOCK_8_6\
    VLOAD_COMP_P_2(6, 7, 0, 1, 2)\
    VCOMP_P_2(2, 3)\
    VCOMP_P_2(4, 5)\
    VCOMP_P_2(6, 7)

#define BODYBLOCK_9_6\
    VLOAD_COMP_P_3(6, 7, 8, 0, 1, 2, 3)\
    VCOMP_P_3(3, 4, 5)\
    VCOMP_P_3(6, 7, 8)

#define BODYBLOCK_10_6\
    VLOAD_COMP_P_2(6, 7, 0, 1, 2)\
    VLOAD_COMP_P_2(8, 9, 2, 3, 2)\
    VCOMP_P_2(4, 5)\
    VCOMP_P_2(6, 7)\
    VCOMP_P_2(8, 9)

#define BODYBLOCK_11_6\
    BODYBLOCK_7_6 \
    VLOAD_COMP_P_1(7, 1, 1)\
    VLOAD_COMP_P_1(8, 2, 1)\
    VLOAD_COMP_P_1(9, 3, 1)\
    VLOAD_COMP_P_1(10, 4, 1)\
    VCOMP_P_1(5)\
    VCOMP_P_1(6)\
    VCOMP_P_1(7)\
    VCOMP_P_1(8)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)

#define BODYBLOCK_12_6\
    BODYBLOCK_9_6 \
    VLOAD_COMP_P_3(9, 10, 11, 3, 4, 5, 3)\
    VCOMP_P_3(6, 7, 8)\
    VCOMP_P_3(9, 10, 11)

#define BODYBLOCK_13_6\
    BODYBLOCK_11_6 \
    VLOAD_COMP_P_1(11, 5, 1)\
    VLOAD_COMP_P_1(12, 6, 1)\
    VCOMP_P_1(7)\
    VCOMP_P_1(8)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)\
    VCOMP_P_1(11)\
    VCOMP_P_1(12)

#define BODYBLOCK_14_6\
    BODYBLOCK_10_6 \
    VLOAD_COMP_P_2(10, 11, 4, 5, 2)\
    VLOAD_COMP_P_2(12, 13, 6, 7, 2)\
    VCOMP_P_2(8, 9)\
    VCOMP_P_2(10, 11)\
    VCOMP_P_2(12, 13)

#define BODYBLOCK_15_6\
    BODYBLOCK_12_6 \
    VLOAD_COMP_P_3(12, 13, 14, 6, 7, 8, 3)\
    VCOMP_P_3(9, 10, 11)\
    VCOMP_P_3(12, 13, 14)

#define BODYBLOCK_16_6\
    BODYBLOCK_14_6 \
    VLOAD_COMP_P_2(14, 15, 8, 9, 2)\
    VCOMP_P_2(10, 11)\
    VCOMP_P_2(12, 13)\
    VCOMP_P_2(14, 15)

// ==========================================
// DIST = 7
// ==========================================
#define BODYBLOCK_7_7\
    VCOMP_P_1(0)\
    VCOMP_P_1(1)\
    VCOMP_P_1(2)\
    VCOMP_P_1(3)\
    VCOMP_P_1(4)\
    VCOMP_P_1(5)\
    VCOMP_P_1(6)

#define BODYBLOCK_8_7\
    VLOAD_P_1(7, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 7), xvstride)\
    VCOMP_P_2(0, 1)\
    VCOMP_P_2(2, 3)\
    VCOMP_P_2(4, 5)\
    VCOMP_P_2(6, 7)

#define BODYBLOCK_9_7\
    VLOAD_P_2(7, 8, 3)\
    ADVANCE_2_PTR(xptr, VIDX_PTR(3, 7), VIDX_PTR(3, 8), xvstride)\
    VCOMP_P_3(0, 1, 2)\
    VCOMP_P_3(3, 4, 5)\
    VCOMP_P_3(6, 7, 8)

#define BODYBLOCK_10_7\
    VLOAD_COMP_P_2(7, 8, 0, 1, 2)\
    VLOAD_P_1(9, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 9), xvstride)\
    VCOMP_P_2(2, 3)\
    VCOMP_P_2(4, 5)\
    VCOMP_P_2(6, 7)\
    VCOMP_P_2(8, 9)

#define BODYBLOCK_11_7\
    VLOAD_COMP_P_1(7, 0, 1)\
    VLOAD_COMP_P_1(8, 1, 1)\
    VLOAD_COMP_P_1(9, 2, 1)\
    VLOAD_COMP_P_1(10, 3, 1)\
    VCOMP_P_1(4)\
    VCOMP_P_1(5)\
    VCOMP_P_1(6)\
    VCOMP_P_1(7)\
    VCOMP_P_1(8)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)

#define BODYBLOCK_12_7\
    VLOAD_COMP_P_3(7, 8, 9, 0, 1, 2, 3)\
    VLOAD_P_2(10, 11, 3)\
    ADVANCE_2_PTR(xptr, VIDX_PTR(3, 10), VIDX_PTR(3, 11), xvstride)\
    VCOMP_P_3(3, 4, 5)\
    VCOMP_P_3(6, 7, 8)\
    VCOMP_P_3(9, 10, 11)

#define BODYBLOCK_13_7\
    BODYBLOCK_11_7 \
    VLOAD_COMP_P_1(11, 4, 1)\
    VLOAD_COMP_P_1(12, 5, 1)\
    VCOMP_P_1(6)\
    VCOMP_P_1(7)\
    VCOMP_P_1(8)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)\
    VCOMP_P_1(11)\
    VCOMP_P_1(12)

#define BODYBLOCK_14_7\
    BODYBLOCK_10_7 \
    VLOAD_COMP_P_2(9, 10, 2, 3, 2)\
    VLOAD_COMP_P_2(11, 12, 4, 5, 2)\
    VLOAD_P_1(13, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 13), xvstride)\
    VCOMP_P_2(6, 7)\
    VCOMP_P_2(8, 9)\
    VCOMP_P_2(10, 11)\
    VCOMP_P_2(12, 13)

#define BODYBLOCK_15_7\
    BODYBLOCK_12_7 \
    VLOAD_COMP_P_3(10, 11, 12, 3, 4, 5, 3)\
    VLOAD_P_2(13, 14, 3)\
    ADVANCE_2_PTR(xptr, VIDX_PTR(3, 13), VIDX_PTR(3, 14), xvstride)\
    VCOMP_P_3(6, 7, 8)\
    VCOMP_P_3(9, 10, 11)\
    VCOMP_P_3(12, 13, 14)

#define BODYBLOCK_16_7\
    BODYBLOCK_14_7 \
    VLOAD_COMP_P_2(13, 14, 6, 7, 2)\
    VLOAD_P_1(15, 2)\
    ADVANCE_PTR(xptr, VIDX_PTR(2, 15), xvstride)\
    VCOMP_P_2(8, 9)\
    VCOMP_P_2(10, 11)\
    VCOMP_P_2(12, 13)\
    VCOMP_P_2(14, 15)

// ==========================================
// DIST = 8
// ==========================================
#define BODYBLOCK_8_8\
    VCOMP_P_2(0, 1)\
    VCOMP_P_2(2, 3)\
    VCOMP_P_2(4, 5)\
    VCOMP_P_2(6, 7)

#define BODYBLOCK_9_8\
    VLOAD_P_1(8, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 8), xvstride)\
    VCOMP_P_3(0, 1, 2)\
    VCOMP_P_3(3, 4, 5)\
    VCOMP_P_3(6, 7, 8)

#define BODYBLOCK_10_8\
    VLOAD_COMP_P_2(8, 9, 0, 1, 2)\
    VCOMP_P_2(2, 3)\
    VCOMP_P_2(4, 5)\
    VCOMP_P_2(6, 7)\
    VCOMP_P_2(8, 9)

#define BODYBLOCK_11_8\
    VLOAD_COMP_P_1(8, 0, 1)\
    VLOAD_COMP_P_1(9, 1, 1)\
    VLOAD_COMP_P_1(10, 2, 1)\
    VCOMP_P_1(3)\
    VCOMP_P_1(4)\
    VCOMP_P_1(5)\
    VCOMP_P_1(6)\
    VCOMP_P_1(7)\
    VCOMP_P_1(8)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)

#define BODYBLOCK_12_8\
    VLOAD_COMP_P_3(8, 9, 10, 0, 1, 2, 3)\
    VLOAD_P_1(11, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 11), xvstride)\
    VCOMP_P_3(3, 4, 5)\
    VCOMP_P_3(6, 7, 8)\
    VCOMP_P_3(9, 10, 11)

#define BODYBLOCK_13_8\
    BODYBLOCK_11_8 \
    VLOAD_COMP_P_1(11, 3, 1)\
    VLOAD_COMP_P_1(12, 4, 1)\
    VCOMP_P_1(5)\
    VCOMP_P_1(6)\
    VCOMP_P_1(7)\
    VCOMP_P_1(8)\
    VCOMP_P_1(9)\
    VCOMP_P_1(10)\
    VCOMP_P_1(11)\
    VCOMP_P_1(12)

#define BODYBLOCK_14_8\
    VLOAD_COMP_P_2(8, 9, 0, 1, 2)\
    VLOAD_COMP_P_2(10, 11, 2, 3, 2)\
    VLOAD_COMP_P_2(12, 13, 4, 5, 2)\
    VCOMP_P_2(6, 7)\
    VCOMP_P_2(8, 9)\
    VCOMP_P_2(10, 11)\
    VCOMP_P_2(12, 13)

#define BODYBLOCK_15_8\
    BODYBLOCK_12_8 \
    VLOAD_COMP_P_3(11, 12, 13, 3, 4, 5, 3)\
    VLOAD_P_1(14, 3)\
    ADVANCE_PTR(xptr, VIDX_PTR(3, 14), xvstride)\
    VCOMP_P_3(6, 7, 8)\
    VCOMP_P_3(9, 10, 11)\
    VCOMP_P_3(12, 13, 14)

#define BODYBLOCK_16_8\
    BODYBLOCK_14_8 \
    VLOAD_COMP_P_2(14, 15, 6, 7, 2)\
    VCOMP_P_2(8, 9)\
    VCOMP_P_2(10, 11)\
    VCOMP_P_2(12, 13)\
    VCOMP_P_2(14, 15)


#endif // UKR1M_NV_BLOCKS_H
