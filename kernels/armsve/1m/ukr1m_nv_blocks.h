/*
 * ukr1m_nv_blocks.h -- GENERATED, DO NOT EDIT.
 *
 * Regenerate with:  python3 tools/gen_ukr1m_nv_blocks.py > \
 *                       kernels/armsve/1m/ukr1m_nv_blocks.h
 *
 * Block macros for the SVE packm micro-kernels.  The primitives used here
 * (NV_LOAD_ROW, NV_XFORM_ROW, NV_STORE_SCATTER_ROW, NV_ZIP_PAIR,
 *  NV_STORE_ILV2/3/4, NVLEN_LOAD/STORE_V) are supplied by the including
 * translation unit and select between the kappa == 1 / kappa != 1 and
 * unit- / non-unit-stride variants.
 */

#ifndef UKR1M_NV_BLOCKS_H
#define UKR1M_NV_BLOCKS_H

/* ---------------- row loads (+ optional kappa scaling) -------------- */
/* NV_LOAD_ROW(zidx, baseidx, offidx): baseidx 0 -> %[xp], 1 -> %[xb2]   */

#define NV_LOADS_1 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_XFORM_ROW(0)

#define NV_LOADS_2 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1)

#define NV_LOADS_3 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2)

#define NV_LOADS_4 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3)

#define NV_LOADS_5 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4)

#define NV_LOADS_6 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5)

#define NV_LOADS_7 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_LOAD_ROW(6, 0, 6) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5) \
    NV_XFORM_ROW(6)

#define NV_LOADS_8 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_LOAD_ROW(6, 0, 6) \
    NV_LOAD_ROW(7, 0, 7) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5) \
    NV_XFORM_ROW(6) \
    NV_XFORM_ROW(7)

#define NV_LOADS_9 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_LOAD_ROW(6, 0, 6) \
    NV_LOAD_ROW(7, 0, 7) \
    NV_LOAD_ROW(8, 1, 0) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5) \
    NV_XFORM_ROW(6) \
    NV_XFORM_ROW(7) \
    NV_XFORM_ROW(8)

#define NV_LOADS_10 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_LOAD_ROW(6, 0, 6) \
    NV_LOAD_ROW(7, 0, 7) \
    NV_LOAD_ROW(8, 1, 0) \
    NV_LOAD_ROW(9, 1, 1) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5) \
    NV_XFORM_ROW(6) \
    NV_XFORM_ROW(7) \
    NV_XFORM_ROW(8) \
    NV_XFORM_ROW(9)

#define NV_LOADS_11 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_LOAD_ROW(6, 0, 6) \
    NV_LOAD_ROW(7, 0, 7) \
    NV_LOAD_ROW(8, 1, 0) \
    NV_LOAD_ROW(9, 1, 1) \
    NV_LOAD_ROW(10, 1, 2) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5) \
    NV_XFORM_ROW(6) \
    NV_XFORM_ROW(7) \
    NV_XFORM_ROW(8) \
    NV_XFORM_ROW(9) \
    NV_XFORM_ROW(10)

#define NV_LOADS_12 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_LOAD_ROW(6, 0, 6) \
    NV_LOAD_ROW(7, 0, 7) \
    NV_LOAD_ROW(8, 1, 0) \
    NV_LOAD_ROW(9, 1, 1) \
    NV_LOAD_ROW(10, 1, 2) \
    NV_LOAD_ROW(11, 1, 3) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5) \
    NV_XFORM_ROW(6) \
    NV_XFORM_ROW(7) \
    NV_XFORM_ROW(8) \
    NV_XFORM_ROW(9) \
    NV_XFORM_ROW(10) \
    NV_XFORM_ROW(11)

#define NV_LOADS_13 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_LOAD_ROW(6, 0, 6) \
    NV_LOAD_ROW(7, 0, 7) \
    NV_LOAD_ROW(8, 1, 0) \
    NV_LOAD_ROW(9, 1, 1) \
    NV_LOAD_ROW(10, 1, 2) \
    NV_LOAD_ROW(11, 1, 3) \
    NV_LOAD_ROW(12, 1, 4) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5) \
    NV_XFORM_ROW(6) \
    NV_XFORM_ROW(7) \
    NV_XFORM_ROW(8) \
    NV_XFORM_ROW(9) \
    NV_XFORM_ROW(10) \
    NV_XFORM_ROW(11) \
    NV_XFORM_ROW(12)

#define NV_LOADS_14 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_LOAD_ROW(6, 0, 6) \
    NV_LOAD_ROW(7, 0, 7) \
    NV_LOAD_ROW(8, 1, 0) \
    NV_LOAD_ROW(9, 1, 1) \
    NV_LOAD_ROW(10, 1, 2) \
    NV_LOAD_ROW(11, 1, 3) \
    NV_LOAD_ROW(12, 1, 4) \
    NV_LOAD_ROW(13, 1, 5) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5) \
    NV_XFORM_ROW(6) \
    NV_XFORM_ROW(7) \
    NV_XFORM_ROW(8) \
    NV_XFORM_ROW(9) \
    NV_XFORM_ROW(10) \
    NV_XFORM_ROW(11) \
    NV_XFORM_ROW(12) \
    NV_XFORM_ROW(13)

#define NV_LOADS_15 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_LOAD_ROW(6, 0, 6) \
    NV_LOAD_ROW(7, 0, 7) \
    NV_LOAD_ROW(8, 1, 0) \
    NV_LOAD_ROW(9, 1, 1) \
    NV_LOAD_ROW(10, 1, 2) \
    NV_LOAD_ROW(11, 1, 3) \
    NV_LOAD_ROW(12, 1, 4) \
    NV_LOAD_ROW(13, 1, 5) \
    NV_LOAD_ROW(14, 1, 6) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5) \
    NV_XFORM_ROW(6) \
    NV_XFORM_ROW(7) \
    NV_XFORM_ROW(8) \
    NV_XFORM_ROW(9) \
    NV_XFORM_ROW(10) \
    NV_XFORM_ROW(11) \
    NV_XFORM_ROW(12) \
    NV_XFORM_ROW(13) \
    NV_XFORM_ROW(14)

#define NV_LOADS_16 \
    NV_LOAD_ROW(0, 0, 0) \
    NV_LOAD_ROW(1, 0, 1) \
    NV_LOAD_ROW(2, 0, 2) \
    NV_LOAD_ROW(3, 0, 3) \
    NV_LOAD_ROW(4, 0, 4) \
    NV_LOAD_ROW(5, 0, 5) \
    NV_LOAD_ROW(6, 0, 6) \
    NV_LOAD_ROW(7, 0, 7) \
    NV_LOAD_ROW(8, 1, 0) \
    NV_LOAD_ROW(9, 1, 1) \
    NV_LOAD_ROW(10, 1, 2) \
    NV_LOAD_ROW(11, 1, 3) \
    NV_LOAD_ROW(12, 1, 4) \
    NV_LOAD_ROW(13, 1, 5) \
    NV_LOAD_ROW(14, 1, 6) \
    NV_LOAD_ROW(15, 1, 7) \
    NV_XFORM_ROW(0) \
    NV_XFORM_ROW(1) \
    NV_XFORM_ROW(2) \
    NV_XFORM_ROW(3) \
    NV_XFORM_ROW(4) \
    NV_XFORM_ROW(5) \
    NV_XFORM_ROW(6) \
    NV_XFORM_ROW(7) \
    NV_XFORM_ROW(8) \
    NV_XFORM_ROW(9) \
    NV_XFORM_ROW(10) \
    NV_XFORM_ROW(11) \
    NV_XFORM_ROW(12) \
    NV_XFORM_ROW(13) \
    NV_XFORM_ROW(14) \
    NV_XFORM_ROW(15)

/* ---------------- scatter stores (general ldp) ---------------------- */

#define NV_STORE_SCT_1 \
    NV_STORE_SCATTER_ROW_0

#define NV_STORE_SCT_2 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1)

#define NV_STORE_SCT_3 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2)

#define NV_STORE_SCT_4 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3)

#define NV_STORE_SCT_5 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4)

#define NV_STORE_SCT_6 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5)

#define NV_STORE_SCT_7 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5) \
    NV_STORE_SCATTER_ROW_N(6)

#define NV_STORE_SCT_8 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5) \
    NV_STORE_SCATTER_ROW_N(6) \
    NV_STORE_SCATTER_ROW_N(7)

#define NV_STORE_SCT_9 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5) \
    NV_STORE_SCATTER_ROW_N(6) \
    NV_STORE_SCATTER_ROW_N(7) \
    NV_STORE_SCATTER_ROW_N(8)

#define NV_STORE_SCT_10 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5) \
    NV_STORE_SCATTER_ROW_N(6) \
    NV_STORE_SCATTER_ROW_N(7) \
    NV_STORE_SCATTER_ROW_N(8) \
    NV_STORE_SCATTER_ROW_N(9)

#define NV_STORE_SCT_11 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5) \
    NV_STORE_SCATTER_ROW_N(6) \
    NV_STORE_SCATTER_ROW_N(7) \
    NV_STORE_SCATTER_ROW_N(8) \
    NV_STORE_SCATTER_ROW_N(9) \
    NV_STORE_SCATTER_ROW_N(10)

#define NV_STORE_SCT_12 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5) \
    NV_STORE_SCATTER_ROW_N(6) \
    NV_STORE_SCATTER_ROW_N(7) \
    NV_STORE_SCATTER_ROW_N(8) \
    NV_STORE_SCATTER_ROW_N(9) \
    NV_STORE_SCATTER_ROW_N(10) \
    NV_STORE_SCATTER_ROW_N(11)

#define NV_STORE_SCT_13 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5) \
    NV_STORE_SCATTER_ROW_N(6) \
    NV_STORE_SCATTER_ROW_N(7) \
    NV_STORE_SCATTER_ROW_N(8) \
    NV_STORE_SCATTER_ROW_N(9) \
    NV_STORE_SCATTER_ROW_N(10) \
    NV_STORE_SCATTER_ROW_N(11) \
    NV_STORE_SCATTER_ROW_N(12)

#define NV_STORE_SCT_14 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5) \
    NV_STORE_SCATTER_ROW_N(6) \
    NV_STORE_SCATTER_ROW_N(7) \
    NV_STORE_SCATTER_ROW_N(8) \
    NV_STORE_SCATTER_ROW_N(9) \
    NV_STORE_SCATTER_ROW_N(10) \
    NV_STORE_SCATTER_ROW_N(11) \
    NV_STORE_SCATTER_ROW_N(12) \
    NV_STORE_SCATTER_ROW_N(13)

#define NV_STORE_SCT_15 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5) \
    NV_STORE_SCATTER_ROW_N(6) \
    NV_STORE_SCATTER_ROW_N(7) \
    NV_STORE_SCATTER_ROW_N(8) \
    NV_STORE_SCATTER_ROW_N(9) \
    NV_STORE_SCATTER_ROW_N(10) \
    NV_STORE_SCATTER_ROW_N(11) \
    NV_STORE_SCATTER_ROW_N(12) \
    NV_STORE_SCATTER_ROW_N(13) \
    NV_STORE_SCATTER_ROW_N(14)

#define NV_STORE_SCT_16 \
    NV_STORE_SCATTER_ROW_0 \
    NV_STORE_SCATTER_ROW_N(1) \
    NV_STORE_SCATTER_ROW_N(2) \
    NV_STORE_SCATTER_ROW_N(3) \
    NV_STORE_SCATTER_ROW_N(4) \
    NV_STORE_SCATTER_ROW_N(5) \
    NV_STORE_SCATTER_ROW_N(6) \
    NV_STORE_SCATTER_ROW_N(7) \
    NV_STORE_SCATTER_ROW_N(8) \
    NV_STORE_SCATTER_ROW_N(9) \
    NV_STORE_SCATTER_ROW_N(10) \
    NV_STORE_SCATTER_ROW_N(11) \
    NV_STORE_SCATTER_ROW_N(12) \
    NV_STORE_SCATTER_ROW_N(13) \
    NV_STORE_SCATTER_ROW_N(14) \
    NV_STORE_SCATTER_ROW_N(15)

/* ---------------- interleaved stores (ldp == cdim) ------------------ */
/* cdim = 2^k * base;  k zip stages then cdim/base ST{base}x.          */

#define NV_HAS_ILV_1 1
/* 0 zip stage(s), 0 zip pair(s), 1 x ST1x */
#define NV_STORE_ILV_1 \
    NV_STORE_ILV1(0, 0, 0)

#define NV_HAS_ILV_2 1
/* 0 zip stage(s), 0 zip pair(s), 1 x ST2x */
#define NV_STORE_ILV_2 \
    NV_STORE_ILV2(0, 1, 0, 0)

#define NV_HAS_ILV_3 1
/* 0 zip stage(s), 0 zip pair(s), 1 x ST3x */
#define NV_STORE_ILV_3 \
    NV_STORE_ILV3(0, 1, 2, 0, 0)

#define NV_HAS_ILV_4 1
/* 0 zip stage(s), 0 zip pair(s), 1 x ST4x */
#define NV_STORE_ILV_4 \
    NV_STORE_ILV4(0, 1, 2, 3, 0, 0)

#define NV_HAS_ILV_5 0

#define NV_HAS_ILV_6 1
/* 1 zip stage(s), 3 zip pair(s), 2 x ST3x */
#define NV_STORE_ILV_6 \
    NV_ZIP_PAIR(0, 3) \
    NV_ZIP_PAIR(1, 4) \
    NV_ZIP_PAIR(2, 5) \
    NV_ILV_PRED_INIT(1) \
    NV_ILV_PRED_FIRST(1) \
    NV_STORE_ILV3(0, 1, 2, 1, 0) \
    NV_ILV_PRED_NEXT(2) \
    NV_STORE_ILV3(3, 4, 5, 2, 3)

#define NV_HAS_ILV_7 0

#define NV_HAS_ILV_8 1
/* 1 zip stage(s), 4 zip pair(s), 2 x ST4x */
#define NV_STORE_ILV_8 \
    NV_ZIP_PAIR(0, 4) \
    NV_ZIP_PAIR(1, 5) \
    NV_ZIP_PAIR(2, 6) \
    NV_ZIP_PAIR(3, 7) \
    NV_ILV_PRED_INIT(1) \
    NV_ILV_PRED_FIRST(1) \
    NV_STORE_ILV4(0, 1, 2, 3, 1, 0) \
    NV_ILV_PRED_NEXT(2) \
    NV_STORE_ILV4(4, 5, 6, 7, 2, 4)

#define NV_HAS_ILV_9 0

#define NV_HAS_ILV_10 0

#define NV_HAS_ILV_11 0

#define NV_HAS_ILV_12 1
/* 2 zip stage(s), 12 zip pair(s), 4 x ST3x */
#define NV_STORE_ILV_12 \
    NV_ZIP_PAIR(0, 6) \
    NV_ZIP_PAIR(1, 7) \
    NV_ZIP_PAIR(2, 8) \
    NV_ZIP_PAIR(3, 9) \
    NV_ZIP_PAIR(4, 10) \
    NV_ZIP_PAIR(5, 11) \
    NV_ZIP_PAIR(0, 3) \
    NV_ZIP_PAIR(1, 4) \
    NV_ZIP_PAIR(2, 5) \
    NV_ZIP_PAIR(6, 9) \
    NV_ZIP_PAIR(7, 10) \
    NV_ZIP_PAIR(8, 11) \
    NV_ILV_PRED_INIT(2) \
    NV_ILV_PRED_FIRST(1) \
    NV_STORE_ILV3(0, 1, 2, 1, 0) \
    NV_ILV_PRED_NEXT(2) \
    NV_STORE_ILV3(3, 4, 5, 2, 3) \
    NV_ILV_PRED_NEXT(3) \
    NV_STORE_ILV3(6, 7, 8, 3, 6) \
    NV_ILV_PRED_NEXT(4) \
    NV_STORE_ILV3(9, 10, 11, 4, 9)

#define NV_HAS_ILV_13 0

#define NV_HAS_ILV_14 0

#define NV_HAS_ILV_15 0

#define NV_HAS_ILV_16 1
/* 2 zip stage(s), 16 zip pair(s), 4 x ST4x */
#define NV_STORE_ILV_16 \
    NV_ZIP_PAIR(0, 8) \
    NV_ZIP_PAIR(1, 9) \
    NV_ZIP_PAIR(2, 10) \
    NV_ZIP_PAIR(3, 11) \
    NV_ZIP_PAIR(4, 12) \
    NV_ZIP_PAIR(5, 13) \
    NV_ZIP_PAIR(6, 14) \
    NV_ZIP_PAIR(7, 15) \
    NV_ZIP_PAIR(0, 4) \
    NV_ZIP_PAIR(1, 5) \
    NV_ZIP_PAIR(2, 6) \
    NV_ZIP_PAIR(3, 7) \
    NV_ZIP_PAIR(8, 12) \
    NV_ZIP_PAIR(9, 13) \
    NV_ZIP_PAIR(10, 14) \
    NV_ZIP_PAIR(11, 15) \
    NV_ILV_PRED_INIT(2) \
    NV_ILV_PRED_FIRST(1) \
    NV_STORE_ILV4(0, 1, 2, 3, 1, 0) \
    NV_ILV_PRED_NEXT(2) \
    NV_STORE_ILV4(4, 5, 6, 7, 2, 4) \
    NV_ILV_PRED_NEXT(3) \
    NV_STORE_ILV4(8, 9, 10, 11, 3, 8) \
    NV_ILV_PRED_NEXT(4) \
    NV_STORE_ILV4(12, 13, 14, 15, 4, 12)

/* ------- per-cdim asm operand lists (keeps register pressure down) -- */

#define NV_OPS_XO_1
#define NV_OPS_XB2_1
#define NV_XB2_SETUP_1 NV_SETUP_XB2_NO
#define NV_OPS_XO_2 [xo1] "r" (xo1),
#define NV_OPS_XB2_2
#define NV_XB2_SETUP_2 NV_SETUP_XB2_NO
#define NV_OPS_XO_3 [xo1] "r" (xo1), [xo2] "r" (xo2),
#define NV_OPS_XB2_3
#define NV_XB2_SETUP_3 NV_SETUP_XB2_NO
#define NV_OPS_XO_4 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3),
#define NV_OPS_XB2_4
#define NV_XB2_SETUP_4 NV_SETUP_XB2_NO
#define NV_OPS_XO_5 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4),
#define NV_OPS_XB2_5
#define NV_XB2_SETUP_5 NV_SETUP_XB2_NO
#define NV_OPS_XO_6 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5),
#define NV_OPS_XB2_6
#define NV_XB2_SETUP_6 NV_SETUP_XB2_NO
#define NV_OPS_XO_7 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5), [xo6] "r" (xo6),
#define NV_OPS_XB2_7
#define NV_XB2_SETUP_7 NV_SETUP_XB2_NO
#define NV_OPS_XO_8 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5), [xo6] "r" (xo6), [xo7] "r" (xo7),
#define NV_OPS_XB2_8
#define NV_XB2_SETUP_8 NV_SETUP_XB2_NO
#define NV_OPS_XO_9 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5), [xo6] "r" (xo6), [xo7] "r" (xo7), [xo8] "r" (xo8),
#define NV_OPS_XB2_9 [xb2] "=&r" (xb2),
#define NV_XB2_SETUP_9 NV_SETUP_XB2_YES
#define NV_OPS_XO_10 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5), [xo6] "r" (xo6), [xo7] "r" (xo7), [xo8] "r" (xo8),
#define NV_OPS_XB2_10 [xb2] "=&r" (xb2),
#define NV_XB2_SETUP_10 NV_SETUP_XB2_YES
#define NV_OPS_XO_11 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5), [xo6] "r" (xo6), [xo7] "r" (xo7), [xo8] "r" (xo8),
#define NV_OPS_XB2_11 [xb2] "=&r" (xb2),
#define NV_XB2_SETUP_11 NV_SETUP_XB2_YES
#define NV_OPS_XO_12 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5), [xo6] "r" (xo6), [xo7] "r" (xo7), [xo8] "r" (xo8),
#define NV_OPS_XB2_12 [xb2] "=&r" (xb2),
#define NV_XB2_SETUP_12 NV_SETUP_XB2_YES
#define NV_OPS_XO_13 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5), [xo6] "r" (xo6), [xo7] "r" (xo7), [xo8] "r" (xo8),
#define NV_OPS_XB2_13 [xb2] "=&r" (xb2),
#define NV_XB2_SETUP_13 NV_SETUP_XB2_YES
#define NV_OPS_XO_14 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5), [xo6] "r" (xo6), [xo7] "r" (xo7), [xo8] "r" (xo8),
#define NV_OPS_XB2_14 [xb2] "=&r" (xb2),
#define NV_XB2_SETUP_14 NV_SETUP_XB2_YES
#define NV_OPS_XO_15 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5), [xo6] "r" (xo6), [xo7] "r" (xo7), [xo8] "r" (xo8),
#define NV_OPS_XB2_15 [xb2] "=&r" (xb2),
#define NV_XB2_SETUP_15 NV_SETUP_XB2_YES
#define NV_OPS_XO_16 [xo1] "r" (xo1), [xo2] "r" (xo2), [xo3] "r" (xo3), [xo4] "r" (xo4), [xo5] "r" (xo5), [xo6] "r" (xo6), [xo7] "r" (xo7), [xo8] "r" (xo8),
#define NV_OPS_XB2_16 [xb2] "=&r" (xb2),
#define NV_XB2_SETUP_16 NV_SETUP_XB2_YES

#define NV_OPS_XO_I(cdim)     NV_OPS_XO_ ## cdim
#define NV_OPS_XO(cdim)       NV_OPS_XO_I(cdim)
#define NV_OPS_XB2_I(cdim)    NV_OPS_XB2_ ## cdim
#define NV_OPS_XB2(cdim)      NV_OPS_XB2_I(cdim)
#define NV_XB2_SETUP_I(cdim)  NV_XB2_SETUP_ ## cdim
#define NV_XB2_SETUP(cdim)    NV_XB2_SETUP_I(cdim)

/* ------------------------------------------------------------------ */
/* cdim == NVLEN * VL path: NVLEN vectors per column, 4 columns unrolled */
/* ------------------------------------------------------------------ */

#define NVLEN_COL_1_0 \
    NVLEN_LOAD_V(0, 0) \
    NVLEN_XADV
#define NVLEN_COL_1_1 \
    NVLEN_LOAD_V(1, 0) \
    NVLEN_XADV
#define NVLEN_COL_1_2 \
    NVLEN_LOAD_V(2, 0) \
    NVLEN_XADV
#define NVLEN_COL_1_3 \
    NVLEN_LOAD_V(3, 0) \
    NVLEN_XADV

#define NVLEN_STORE_1_0 \
    NVLEN_XFORM_V(0) \
    NVLEN_STORE_V(0, 0) \
    NVLEN_YADV
#define NVLEN_STORE_1_1 \
    NVLEN_XFORM_V(1) \
    NVLEN_STORE_V(1, 0) \
    NVLEN_YADV
#define NVLEN_STORE_1_2 \
    NVLEN_XFORM_V(2) \
    NVLEN_STORE_V(2, 0) \
    NVLEN_YADV
#define NVLEN_STORE_1_3 \
    NVLEN_XFORM_V(3) \
    NVLEN_STORE_V(3, 0) \
    NVLEN_YADV

#define NVLEN_BODY_1 \
    NVLEN_COL_1_0 \
    NVLEN_COL_1_1 \
    NVLEN_COL_1_2 \
    NVLEN_COL_1_3
#define NVLEN_STOREBLOCK_1 \
    NVLEN_STORE_1_0 \
    NVLEN_STORE_1_1 \
    NVLEN_STORE_1_2 \
    NVLEN_STORE_1_3
#define NVLEN_TAIL_1 \
    NVLEN_LOAD_V(0, 0) \
    NVLEN_XFORM_V(0) \
    NVLEN_STORE_V(0, 0)

#define NVLEN_COL_2_0 \
    NVLEN_LOAD_V(0, 0) \
    NVLEN_LOAD_V(1, 1) \
    NVLEN_XADV
#define NVLEN_COL_2_1 \
    NVLEN_LOAD_V(2, 0) \
    NVLEN_LOAD_V(3, 1) \
    NVLEN_XADV
#define NVLEN_COL_2_2 \
    NVLEN_LOAD_V(4, 0) \
    NVLEN_LOAD_V(5, 1) \
    NVLEN_XADV
#define NVLEN_COL_2_3 \
    NVLEN_LOAD_V(6, 0) \
    NVLEN_LOAD_V(7, 1) \
    NVLEN_XADV

#define NVLEN_STORE_2_0 \
    NVLEN_XFORM_V(0) \
    NVLEN_XFORM_V(1) \
    NVLEN_STORE_V(0, 0) \
    NVLEN_STORE_V(1, 1) \
    NVLEN_YADV
#define NVLEN_STORE_2_1 \
    NVLEN_XFORM_V(2) \
    NVLEN_XFORM_V(3) \
    NVLEN_STORE_V(2, 0) \
    NVLEN_STORE_V(3, 1) \
    NVLEN_YADV
#define NVLEN_STORE_2_2 \
    NVLEN_XFORM_V(4) \
    NVLEN_XFORM_V(5) \
    NVLEN_STORE_V(4, 0) \
    NVLEN_STORE_V(5, 1) \
    NVLEN_YADV
#define NVLEN_STORE_2_3 \
    NVLEN_XFORM_V(6) \
    NVLEN_XFORM_V(7) \
    NVLEN_STORE_V(6, 0) \
    NVLEN_STORE_V(7, 1) \
    NVLEN_YADV

#define NVLEN_BODY_2 \
    NVLEN_COL_2_0 \
    NVLEN_COL_2_1 \
    NVLEN_COL_2_2 \
    NVLEN_COL_2_3
#define NVLEN_STOREBLOCK_2 \
    NVLEN_STORE_2_0 \
    NVLEN_STORE_2_1 \
    NVLEN_STORE_2_2 \
    NVLEN_STORE_2_3
#define NVLEN_TAIL_2 \
    NVLEN_LOAD_V(0, 0) \
    NVLEN_LOAD_V(1, 1) \
    NVLEN_XFORM_V(0) \
    NVLEN_XFORM_V(1) \
    NVLEN_STORE_V(0, 0) \
    NVLEN_STORE_V(1, 1)

#define NVLEN_COL_3_0 \
    NVLEN_LOAD_V(0, 0) \
    NVLEN_LOAD_V(1, 1) \
    NVLEN_LOAD_V(2, 2) \
    NVLEN_XADV
#define NVLEN_COL_3_1 \
    NVLEN_LOAD_V(3, 0) \
    NVLEN_LOAD_V(4, 1) \
    NVLEN_LOAD_V(5, 2) \
    NVLEN_XADV
#define NVLEN_COL_3_2 \
    NVLEN_LOAD_V(6, 0) \
    NVLEN_LOAD_V(7, 1) \
    NVLEN_LOAD_V(8, 2) \
    NVLEN_XADV
#define NVLEN_COL_3_3 \
    NVLEN_LOAD_V(9, 0) \
    NVLEN_LOAD_V(10, 1) \
    NVLEN_LOAD_V(11, 2) \
    NVLEN_XADV

#define NVLEN_STORE_3_0 \
    NVLEN_XFORM_V(0) \
    NVLEN_XFORM_V(1) \
    NVLEN_XFORM_V(2) \
    NVLEN_STORE_V(0, 0) \
    NVLEN_STORE_V(1, 1) \
    NVLEN_STORE_V(2, 2) \
    NVLEN_YADV
#define NVLEN_STORE_3_1 \
    NVLEN_XFORM_V(3) \
    NVLEN_XFORM_V(4) \
    NVLEN_XFORM_V(5) \
    NVLEN_STORE_V(3, 0) \
    NVLEN_STORE_V(4, 1) \
    NVLEN_STORE_V(5, 2) \
    NVLEN_YADV
#define NVLEN_STORE_3_2 \
    NVLEN_XFORM_V(6) \
    NVLEN_XFORM_V(7) \
    NVLEN_XFORM_V(8) \
    NVLEN_STORE_V(6, 0) \
    NVLEN_STORE_V(7, 1) \
    NVLEN_STORE_V(8, 2) \
    NVLEN_YADV
#define NVLEN_STORE_3_3 \
    NVLEN_XFORM_V(9) \
    NVLEN_XFORM_V(10) \
    NVLEN_XFORM_V(11) \
    NVLEN_STORE_V(9, 0) \
    NVLEN_STORE_V(10, 1) \
    NVLEN_STORE_V(11, 2) \
    NVLEN_YADV

#define NVLEN_BODY_3 \
    NVLEN_COL_3_0 \
    NVLEN_COL_3_1 \
    NVLEN_COL_3_2 \
    NVLEN_COL_3_3
#define NVLEN_STOREBLOCK_3 \
    NVLEN_STORE_3_0 \
    NVLEN_STORE_3_1 \
    NVLEN_STORE_3_2 \
    NVLEN_STORE_3_3
#define NVLEN_TAIL_3 \
    NVLEN_LOAD_V(0, 0) \
    NVLEN_LOAD_V(1, 1) \
    NVLEN_LOAD_V(2, 2) \
    NVLEN_XFORM_V(0) \
    NVLEN_XFORM_V(1) \
    NVLEN_XFORM_V(2) \
    NVLEN_STORE_V(0, 0) \
    NVLEN_STORE_V(1, 1) \
    NVLEN_STORE_V(2, 2)

#define NVLEN_COL_4_0 \
    NVLEN_LOAD_V(0, 0) \
    NVLEN_LOAD_V(1, 1) \
    NVLEN_LOAD_V(2, 2) \
    NVLEN_LOAD_V(3, 3) \
    NVLEN_XADV
#define NVLEN_COL_4_1 \
    NVLEN_LOAD_V(4, 0) \
    NVLEN_LOAD_V(5, 1) \
    NVLEN_LOAD_V(6, 2) \
    NVLEN_LOAD_V(7, 3) \
    NVLEN_XADV
#define NVLEN_COL_4_2 \
    NVLEN_LOAD_V(8, 0) \
    NVLEN_LOAD_V(9, 1) \
    NVLEN_LOAD_V(10, 2) \
    NVLEN_LOAD_V(11, 3) \
    NVLEN_XADV
#define NVLEN_COL_4_3 \
    NVLEN_LOAD_V(12, 0) \
    NVLEN_LOAD_V(13, 1) \
    NVLEN_LOAD_V(14, 2) \
    NVLEN_LOAD_V(15, 3) \
    NVLEN_XADV

#define NVLEN_STORE_4_0 \
    NVLEN_XFORM_V(0) \
    NVLEN_XFORM_V(1) \
    NVLEN_XFORM_V(2) \
    NVLEN_XFORM_V(3) \
    NVLEN_STORE_V(0, 0) \
    NVLEN_STORE_V(1, 1) \
    NVLEN_STORE_V(2, 2) \
    NVLEN_STORE_V(3, 3) \
    NVLEN_YADV
#define NVLEN_STORE_4_1 \
    NVLEN_XFORM_V(4) \
    NVLEN_XFORM_V(5) \
    NVLEN_XFORM_V(6) \
    NVLEN_XFORM_V(7) \
    NVLEN_STORE_V(4, 0) \
    NVLEN_STORE_V(5, 1) \
    NVLEN_STORE_V(6, 2) \
    NVLEN_STORE_V(7, 3) \
    NVLEN_YADV
#define NVLEN_STORE_4_2 \
    NVLEN_XFORM_V(8) \
    NVLEN_XFORM_V(9) \
    NVLEN_XFORM_V(10) \
    NVLEN_XFORM_V(11) \
    NVLEN_STORE_V(8, 0) \
    NVLEN_STORE_V(9, 1) \
    NVLEN_STORE_V(10, 2) \
    NVLEN_STORE_V(11, 3) \
    NVLEN_YADV
#define NVLEN_STORE_4_3 \
    NVLEN_XFORM_V(12) \
    NVLEN_XFORM_V(13) \
    NVLEN_XFORM_V(14) \
    NVLEN_XFORM_V(15) \
    NVLEN_STORE_V(12, 0) \
    NVLEN_STORE_V(13, 1) \
    NVLEN_STORE_V(14, 2) \
    NVLEN_STORE_V(15, 3) \
    NVLEN_YADV

#define NVLEN_BODY_4 \
    NVLEN_COL_4_0 \
    NVLEN_COL_4_1 \
    NVLEN_COL_4_2 \
    NVLEN_COL_4_3
#define NVLEN_STOREBLOCK_4 \
    NVLEN_STORE_4_0 \
    NVLEN_STORE_4_1 \
    NVLEN_STORE_4_2 \
    NVLEN_STORE_4_3
#define NVLEN_TAIL_4 \
    NVLEN_LOAD_V(0, 0) \
    NVLEN_LOAD_V(1, 1) \
    NVLEN_LOAD_V(2, 2) \
    NVLEN_LOAD_V(3, 3) \
    NVLEN_XFORM_V(0) \
    NVLEN_XFORM_V(1) \
    NVLEN_XFORM_V(2) \
    NVLEN_XFORM_V(3) \
    NVLEN_STORE_V(0, 0) \
    NVLEN_STORE_V(1, 1) \
    NVLEN_STORE_V(2, 2) \
    NVLEN_STORE_V(3, 3)

/* ---------------- dispatchers --------------------------------------- */
#define NV_LOADS_I(cdim)        NV_LOADS_ ## cdim
#define NV_LOADS(cdim)          NV_LOADS_I(cdim)
#define NV_STORE_SCT_I(cdim)    NV_STORE_SCT_ ## cdim
#define NV_STORE_SCT(cdim)      NV_STORE_SCT_I(cdim)
#define NV_STORE_ILV_I(cdim)    NV_STORE_ILV_ ## cdim
#define NV_STORE_ILV(cdim)      NV_STORE_ILV_I(cdim)
#define NV_HAS_ILV_I(cdim)      NV_HAS_ILV_ ## cdim
#define NV_HAS_ILV(cdim)        NV_HAS_ILV_I(cdim)

#define NVLEN_BODY_I(n)         NVLEN_BODY_ ## n
#define NVLEN_BODY(n)           NVLEN_BODY_I(n)
#define NVLEN_STOREBLOCK_I(n)   NVLEN_STOREBLOCK_ ## n
#define NVLEN_STOREBLOCK(n)     NVLEN_STOREBLOCK_I(n)
#define NVLEN_TAIL_I(n)         NVLEN_TAIL_ ## n
#define NVLEN_TAIL(n)           NVLEN_TAIL_I(n)

#endif /* UKR1M_NV_BLOCKS_H */
