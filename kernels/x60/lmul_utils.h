#if !defined(LMUL)
#error lmul_utils.h included before defining LMUL macro
#endif

#if LMUL != 1 && LMUL != 2 && LMUL != 4 && LMUL != 8
#error invalid LMUL value
#endif

#define LMUL_1_0 0
#define LMUL_1_1 1
#define LMUL_1_2 2
#define LMUL_1_3 3
#define LMUL_1_4 4
#define LMUL_1_5 5
#define LMUL_1_6 6
#define LMUL_1_7 7
#define LMUL_1_8 8
#define LMUL_1_9 9
#define LMUL_1_10 10
#define LMUL_1_11 11
#define LMUL_1_12 12
#define LMUL_1_13 13
#define LMUL_1_14 14
#define LMUL_1_15 15
#define LMUL_1_16 16
#define LMUL_1_17 17
#define LMUL_1_18 18
#define LMUL_1_19 19
#define LMUL_1_20 20
#define LMUL_1_21 21
#define LMUL_1_22 22
#define LMUL_1_23 23
#define LMUL_1_24 24
#define LMUL_1_25 25
#define LMUL_1_26 26
#define LMUL_1_27 27
#define LMUL_1_28 28
#define LMUL_1_29 29
#define LMUL_1_30 30
#define LMUL_1_31 31

#define LMUL_2_0 0
#define LMUL_2_1 2
#define LMUL_2_2 4
#define LMUL_2_3 6
#define LMUL_2_4 8
#define LMUL_2_5 10
#define LMUL_2_6 12
#define LMUL_2_7 14
#define LMUL_2_8 16
#define LMUL_2_9 18
#define LMUL_2_10 20
#define LMUL_2_11 22
#define LMUL_2_12 24
#define LMUL_2_13 26
#define LMUL_2_14 28
#define LMUL_2_15 30

#define LMUL_4_0 0
#define LMUL_4_1 4
#define LMUL_4_2 8
#define LMUL_4_3 12
#define LMUL_4_4 16
#define LMUL_4_5 20
#define LMUL_4_6 24
#define LMUL_4_7 28

#define LMUL_8_0 0
#define LMUL_8_1 8
#define LMUL_8_2 16
#define LMUL_8_3 24

#define LMUL_V_INDEX_PASTER(lmul, vidx) LMUL_ ## lmul ## _ ## vidx
#define LMUL_V_INDEX(lmul, vidx) LMUL_V_INDEX_PASTER(lmul, vidx)

#define LMID(vidx) LMUL_V_INDEX(LMUL, vidx)


#define LMUL_1_SHIFT(reg)
#define LMUL_2_SHIFT(reg)\
    "slli " reg ", " reg ", 1\n\t"
#define LMUL_4_SHIFT(reg)\
    "slli " reg ", " reg ", 2\n\t"
#define LMUL_8_SHIFT(reg)\
    "slli " reg ", " reg ", 3\n\t"

#define LMUL_SHIFT_INSTRUCTION_PASTER(lmul, reg) LMUL_ ## lmul ## _SHIFT(reg)
#define LMUL_SHIFT_INSTRUCTION(lmul, reg) LMUL_SHIFT_INSTRUCTION_PASTER(lmul, reg)
#define LMUL_SHIFT(reg) LMUL_SHIFT_INSTRUCTION(LMUL, reg)
