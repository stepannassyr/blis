#undef N_MULTIPLE_1
#undef N_MULTIPLE_2
#undef N_MULTIPLE_3
#undef N_MULTIPLE_4
#undef NPTRS
#undef DECLARE_EXTRA_PTRS
#undef INIT_EXTRA_PTRS
#undef REWIND_PTRS
#undef CLOBBER_PTRS
#undef ADJUST_STRIDE


#if CDIM == 1
#define N_MULTIPLE_1
#elif CDIM == 2
#define N_MULTIPLE_2
#elif CDIM == 3
#define N_MULTIPLE_3
#elif CDIM == 4
#define N_MULTIPLE_2
#elif CDIM == 5
#define N_MULTIPLE_1
#elif CDIM == 6
#define N_MULTIPLE_3
#elif CDIM == 7
#define N_MULTIPLE_1
#elif CDIM == 8
#define N_MULTIPLE_2
#elif CDIM == 9
#define N_MULTIPLE_3
#elif CDIM == 10
#define N_MULTIPLE_2
#elif CDIM == 11
#define N_MULTIPLE_1
#elif CDIM == 12
#define N_MULTIPLE_3
#elif CDIM == 13
#define N_MULTIPLE_1
#elif CDIM == 14
#define N_MULTIPLE_2
#elif CDIM == 15
#define N_MULTIPLE_3
#elif CDIM == 16
#define N_MULTIPLE_2
#else
#error INVALID cdim CDIM
#endif


#if defined(N_MULTIPLE_1)
#define NPTRS 1

#define DECLARE_EXTRA_PTRS
#define INIT_EXTRA_PTRS
#define REWIND_PTRS\
        "sub %[xptr], %[xptr], %[xfinoff]\n\t"\
        "add %[yptr], %[yptr], %[yfinoff]\n\t"
#define CLOBBER_PTRS\
      [xptr] "+r" (x), [yptr] "+r" (y),
#define ADJUST_STRIDE(stridereg)

#elif defined(N_MULTIPLE_2)
#define NPTRS 2

#define DECLARE_EXTRA_PTRS\
    uint64_t xptr2;\
    uint64_t yptr2;
#define INIT_EXTRA_PTRS\
    "add %[xptr2], %[xptr], %[xvstride]\n\t"\
    "add %[yptr2], %[yptr], %[yvstride]\n\t"
#define REWIND_PTRS\
    "sub %[xptr], %[xptr], %[xfinoff]\n\t"\
    "sub %[xptr2], %[xptr2], %[xfinoff]\n\t"\
    "add %[yptr], %[yptr], %[yfinoff]\n\t"\
    "add %[yptr2], %[yptr2], %[yfinoff]\n\t"
#define CLOBBER_PTRS\
      [xptr] "+r" (x), \
      [xptr2] "=r" (xptr2), \
      [yptr] "+r" (y), \
      [yptr2] "=r" (yptr2),
#define ADJUST_STRIDE(stridereg)\
    "slli " stridereg ", " stridereg ", 1\n\t"

#elif defined(N_MULTIPLE_3)
#define NPTRS 3

#define DECLARE_EXTRA_PTRS\
    uint64_t xptr2;\
    uint64_t xptr3;\
    uint64_t yptr2;\
    uint64_t yptr3;
#define INIT_EXTRA_PTRS\
    "add %[xptr2], %[xptr], %[xvstride]\n\t"\
    "add %[xptr3], %[xptr2], %[xvstride]\n\t"\
    "add %[yptr2], %[yptr], %[yvstride]\n\t"\
    "add %[yptr3], %[yptr2], %[yvstride]\n\t"
#define REWIND_PTRS\
    "sub %[xptr], %[xptr], %[xfinoff]\n\t"\
    "sub %[xptr2], %[xptr2], %[xfinoff]\n\t"\
    "sub %[xptr3], %[xptr3], %[xfinoff]\n\t"\
    "add %[yptr], %[yptr], %[yfinoff]\n\t"\
    "add %[yptr2], %[yptr2], %[yfinoff]\n\t"\
    "add %[yptr3], %[yptr3], %[yfinoff]\n\t"

#define CLOBBER_PTRS\
      [xptr] "+r" (x), \
      [xptr2] "=r" (xptr2), \
      [xptr3] "=r" (xptr3), \
      [yptr] "+r" (y), \
      [yptr2] "=r" (yptr2),\
      [yptr3] "=r" (yptr3),

#define ADJUST_STRIDE(stridereg)\
    "li %[yptrprefetch], 3\n\t"\
    "mul " stridereg ", " stridereg ", %[yptrprefetch]\n\t"

#else
#error incorrect N_MULTIPLE_X
#endif
