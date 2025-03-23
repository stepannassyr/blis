#include "blis.h"


void bli_dpackm_x60_2vx14
     (
             conj_t  conja,
             pack_t  schema,
             dim_t   cdim_,
             dim_t   cdim_max,
             dim_t   cdim_bcast,
             dim_t   n_,
             dim_t   n_max_,
       const void*   kappa,
       const void*   a, inc_t inca_, inc_t lda_,
             void*   p,              inc_t ldp_,
       const void*   params,
       const cntx_t* cntx
     )
{
    uint64_t vlen;
    __asm__(
        "csrr %[vlen],vlenb"
        : [vlen] "=r" (vlen)
        :
        :
    );

    const int64_t cdim  = cdim_;
    const int64_t mr    = 2*vlen/sizeof(double);
    const int64_t nr    = 14;

    const int64_t n     = n_;
    const int64_t inca  = inca_;
    const int64_t lda   = lda_;
    const int64_t ldp   = ldp_;

    const uint64_t unroll = 8;
    const uint64_t niter = n/unroll;
    const uint64_t nleft = n % unroll;

    // x60 scalar/fp-reg packa special (faster than rvv)
    if ( ( cdim == 8 && cdim_bcast == 1 ) && ( bli_deq1( *(( double* )kappa) ) ) && ( inca == 1 ) )
    {
        const uint64_t unroll = 2;
        const uint64_t nleft = n % unroll;
        const uint64_t niter = n / unroll;
        __asm__(
            "ld t0, %[a]\n\t"    // t0 = a
            "ld t4, %[niter]\n\t"    // t4 = loop counter
            "ld t2, %[p]\n\t"    // t2 = p
            "add t3, t2, 0\n\t"
            "ld t5, %[lda]\n\t"
            "ld t6, %[ldp]\n\t"
            "slli t5, t5, 3\n\t"
            "slli t6, t6, 3\n\t"
            "add s4, t1, t5\n\t"
            "beq t4, zero, .packa_8d_end%=\n\t"
            "fld f0,  0(t0)\n\t"
            "fld f1,  8(t0)\n\t"
            "prefetch.w 0(t2)\n\t"
            "add t1, t0, t5\n\t" // t1 = a+lda
            "fld f2, 16(t0)\n\t"
            "fld f3, 24(t0)\n\t"
            "add t4, t4, -1\n\t"
            "fld f4, 32(t0)\n\t"
            "prefetch.r 0(s4)\n\t"
            "sub t3, t3, t6\n\t"
            "fld f5, 40(t0)\n\t"
            "fld f6, 48(t0)\n\t"
            "slli t5, t5, 1\n\t"
            "slli t6, t6, 1\n\t"
            "fld f7, 56(t0)\n\t"
            "fld f8,  0(t1)\n\t"
            "fld f9,  8(t1)\n\t"
            "add s3, t2, t6\n\t"
            "fld f10, 16(t1)\n\t"
            "fld f11, 24(t1)\n\t"
            "add s4, s4, t5\n\t"
            "fld f12, 32(t1)\n\t"
            "fld f13, 40(t1)\n\t"
            "add t0, t0, t5\n\t"
            "fld f14, 48(t1)\n\t"
            "fld f15, 56(t1)\n\t"
            "beq t4, zero, .packa_8d_last%=\n\t"
            ".packa_8d_loop%=:\n\t"
                "prefetch.w 0(s3)\n\t"
                "fsd f0,   0(t2)\n\t"
                "fsd f1,   8(t2)\n\t"
                "fsd f2,  16(t2)\n\t"
                "fsd f3,  24(t2)\n\t"
                "prefetch.r 0(s4)\n\t"
                "add t1, t1, t5\n\t"
                "add t3, t3, t6\n\t"
                "fld f0,   0(t0)\n\t"
                "fsd f4,  32(t2)\n\t"
                "add s3, s3, t6\n\t"
                "fld f1,   8(t0)\n\t"
                "fsd f5,  40(t2)\n\t"
                "add t4, t4, -1\n\t"
                "add s4, s4, t5\n\t"
                "fld f2,  16(t0)\n\t"
                "fsd f6,  48(t2)\n\t"
                "fld f3,  24(t0)\n\t"
                "fsd f7,  56(t2)\n\t"
                "fld f4,  32(t0)\n\t"
                "prefetch.w 0(s3)\n\t"
                "fsd f8,   0(t3)\n\t"
                "fld f5,  40(t0)\n\t"
                "fsd f9,   8(t3)\n\t"
                "fld f6,  48(t0)\n\t"
                "prefetch.r 0(s4)\n\t"
                "fsd f10, 16(t3)\n\t"
                "fld f7,  56(t0)\n\t"
                "fsd f11, 24(t3)\n\t"
                "add t2, t2, t6\n\t"
                "fld f8,   0(t1)\n\t"
                "fsd f12, 32(t3)\n\t"
                "fld f9,   8(t1)\n\t"
                "fsd f13, 40(t3)\n\t"
                "fld f10, 16(t1)\n\t"
                "fsd f14, 48(t3)\n\t"
                "add s3, s3, t6\n\t"
                "add t0, t0, t5\n\t"
                "fld f11, 24(t1)\n\t"
                "fsd f15, 56(t3)\n\t"
                "add s4, s4, t5\n\t"
                "fld f12, 32(t1)\n\t"
                "fld f13, 40(t1)\n\t"
                "fld f14, 48(t1)\n\t"
                "fld f15, 56(t1)\n\t"
                "bnez t4, .packa_8d_loop%=\n\t"
            ".packa_8d_last%=:\n\t"
                "fsd f0,   0(t2)\n\t"
                "fsd f1,   8(t2)\n\t"
                "fsd f2,  16(t2)\n\t"
                "fsd f3,  24(t2)\n\t"
                "add t3, t3, t6\n\t"
                "fsd f4,  32(t2)\n\t"
                "fsd f5,  40(t2)\n\t"
                "fsd f6,  48(t2)\n\t"
                "fsd f7,  56(t2)\n\t"
                "fsd f8,   0(t3)\n\t"
                "fsd f9,   8(t3)\n\t"
                "fsd f10, 16(t3)\n\t"
                "fsd f11, 24(t3)\n\t"
                "add t2, t2, t6\n\t"
                "fsd f12, 32(t3)\n\t"
                "fsd f13, 40(t3)\n\t"
                "fsd f14, 48(t3)\n\t"
                "fsd f15, 56(t3)\n\t"
            ".packa_8d_end%=:\n\t"
            "ld t4, %[nleft]\n\t"
            "beq t4, zero, .packa_8d_1end%=\n\t"
            "srli t5, t5, 1\n\t"
            "srli t6, t6, 1\n\t"
            "add t1, t0, 32\n\t"
            "add t4, t4, -1\n\t"
            "add t3, t2, 32\n\t"
            "sub t3, t3, t6\n\t" // t3 = p+4*sizeof(double) (since we +ldp t3 in the loop the first time)

            "fld f0,  0(t0)\n\t"
            "fld f1,  8(t0)\n\t"
            "fld f2, 16(t0)\n\t"
            "fld f3, 24(t0)\n\t"
            "fld f4,  0(t1)\n\t"
            "fld f5,  8(t1)\n\t"
            "fld f6, 16(t1)\n\t"
            "fld f7, 24(t1)\n\t"
            "add t0, t0, t5\n\t"

            "beq t4, zero, .packa_8d_1last%=\n\t"
            "add s3, t2, t6\n\t"
            ".packa_8d_1loop%=:\n\t"
                "prefetch.w 0(s3)\n\t"
                "fsd f0,   0(t2)\n\t"
                "fsd f1,   8(t2)\n\t"
                "add t1, t1, t5\n\t"
                "add t3, t3, t6\n\t"
                "fsd f2,  16(t2)\n\t"
                "fsd f3,  24(t2)\n\t"
                "fld f0,   0(t0)\n\t"
                "fsd f4,   0(t3)\n\t"
                "fld f1,   8(t0)\n\t"
                "fsd f5,   8(t3)\n\t"
                "add t4, t4, -1\n\t"
                "fld f2,  16(t0)\n\t"
                "fsd f6,  16(t3)\n\t"
                "fld f3,  24(t0)\n\t"
                "fsd f7,  24(t3)\n\t"
                "fld f4,   0(t1)\n\t"
                "fld f5,   8(t1)\n\t"
                "add t0, t0, t5\n\t"
                "add t2, t2, t6\n\t"
                "add s3, s3, t6\n\t"
                "fld f6,  16(t1)\n\t"
                "fld f7,  24(t1)\n\t"
                "bnez t4, .packa_8d_1loop%=\n\t"
            ".packa_8d_1last%=:\n\t"
                "fsd f0,   0(t2)\n\t"
                "fsd f1,   8(t2)\n\t"
                "add t3, t3, t6\n\t"
                "fsd f2,  16(t2)\n\t"
                "fsd f3,  24(t2)\n\t"
                "fsd f4,   0(t3)\n\t"
                "fsd f5,   8(t3)\n\t"
                "fsd f6,  16(t3)\n\t"
                "fsd f7,  24(t3)\n\t"
            ".packa_8d_1end%=:\n\t"
            : [dummy_p] "+m"(*(double(*)[])p)
            : [n] "m" (n), [a] "m" (a), [p] "m"(p),
              [lda] "m" (lda), [ldp] "m" (ldp),
              [niter] "m" (niter), [nleft] "m" (nleft)
            : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3", "s4",
              "f0", "f1", "f2", "f3",
              "f4", "f5", "f6", "f7",
              "f8", "f9", "f10", "f11",
              "f12", "f13","f14", "f15"
        );
    }
    else if ( cdim == mr && cdim_bcast == 1 )
    {
        if ( bli_deq1( *(( double* )kappa) ) )
        {
            if ( inca == 1 )  // continous memory. packA style
            {
                __asm__(
                    "ld t0, %[a]\n\t"    // t0 = a
                    "ld t4, %[niter]\n\t"    // t4 = loop counter
                    "vsetvli s3, zero, e64, m1, ta, ma\n\t"
                    "slli s3, s3, 3\n\t" // s3 = vlen
                    "add t1, t0, s3\n\t" // t1 = a+vlen
                    "ld t2, %[p]\n\t"    // t2 = p
                    "add t3, t2, s3\n\t" // t3 = p+vlen
                    "ld t5, %[lda]\n\t"
                    "ld t6, %[ldp]\n\t"
                    "slli t5, t5, 3\n\t"
                    "slli t6, t6, 3\n\t"
                    "beq t4, zero, .packaloop8end%=\n\t"
                    ".packaloop8%=:\n\t"
                    "vle64.v v0, (t0)\n\t"
                    "vle64.v v1, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"

                    "vle64.v v2, (t0)\n\t"
                    "vle64.v v3, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"

                    "vle64.v v4, (t0)\n\t"
                    "vle64.v v5, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"

                    "vle64.v v6, (t0)\n\t"
                    "vle64.v v7, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v0, (t2)\n\t"
                    "vse64.v v1, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vle64.v v8, (t0)\n\t"
                    "vle64.v v9, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v2, (t2)\n\t"
                    "vse64.v v3, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vle64.v v10, (t0)\n\t"
                    "vle64.v v11, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v4, (t2)\n\t"
                    "vse64.v v5, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vle64.v v12, (t0)\n\t"
                    "vle64.v v13, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v6, (t2)\n\t"
                    "vse64.v v7, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vle64.v v14, (t0)\n\t"
                    "vle64.v v15, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v8, (t2)\n\t"
                    "vse64.v v9, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v10, (t2)\n\t"
                    "vse64.v v11, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v12, (t2)\n\t"
                    "vse64.v v13, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v14, (t2)\n\t"
                    "vse64.v v15, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "add t4, t4, -1\n\t"
                    "bnez t4, .packaloop8%=\n\t"

                    ".packaloop8end%=:\n\t"
                    "ld t4, %[nleft]\n\t"
                    "beq t4, zero, .packaloopend%=\n\t"
                    
                    ".packaloop%=:\n\t"
                    "vle64.v v0, (t0)\n\t"
                    "vle64.v v1, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v0, (t2)\n\t"
                    "vse64.v v1, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "add t4, t4, -1\n\t"
                    "bnez t4, .packaloop%=\n\t"
                    ".packaloopend%=:\n\t"
                    : [dummy_p] "+m"(*(double(*)[])p)
                    : [niter] "m" (niter), [a] "m" (a), [p] "m"(p),
                      [nleft] "m" (nleft), [lda] "m" (lda), [ldp] "m" (ldp)
                    : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3",
                      "v0", "v1", "v2", "v3",
                      "v4", "v5", "v6", "v7",
                      "v8", "v9", "v10", "v11",
                      "v12", "v13", "v14", "v15"
                );
            }
            else  // gather load/ cont. store. packB style
            {
                __asm__(
                    "ld t0, %[a]\n\t"    // t0 = a
                    "ld t4, %[niter]\n\t"    // t4 = loop counter
                    "ld s4, %[inca]\n\t"
                    "vsetvli s3, zero, e64, m1, ta, ma\n\t"
                    "slli s3, s3, 3\n\t" // s3 = vlen
                    "mul t2, s3, s4\n\t" // inca*vlen*sizeof(double)
                    "add t1, t0, t2\n\t" // t1 = a+inca*vlen
                    "ld t2, %[p]\n\t"    // t2 = p
                    "add t3, t2, s3\n\t" // t3 = p+vlen
                    "ld t5, %[lda]\n\t"
                    "ld t6, %[ldp]\n\t"
                    "slli t5, t5, 3\n\t"
                    "slli t6, t6, 3\n\t"
                    "slli s4, s4, 3\n\t"
                    "beq t4, zero, .packaloop8end%=\n\t"
                    ".packaloop8%=:\n\t"
                    "vlse64.v v0, (t0), s4\n\t"
                    "vlse64.v v1, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"

                    "vlse64.v v2, (t0), s4\n\t"
                    "vlse64.v v3, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"

                    "vlse64.v v4, (t0), s4\n\t"
                    "vlse64.v v5, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"

                    "vlse64.v v6, (t0), s4\n\t"
                    "vlse64.v v7, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v0, (t2)\n\t"
                    "vse64.v v1, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v8, (t0), s4\n\t"
                    "vlse64.v v9, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v2, (t2)\n\t"
                    "vse64.v v3, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v10, (t0), s4\n\t"
                    "vlse64.v v11, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v4, (t2)\n\t"
                    "vse64.v v5, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v12, (t0), s4\n\t"
                    "vlse64.v v13, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v6, (t2)\n\t"
                    "vse64.v v7, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v14, (t0), s4\n\t"
                    "vlse64.v v15, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v8, (t2)\n\t"
                    "vse64.v v9, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v10, (t2)\n\t"
                    "vse64.v v11, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v12, (t2)\n\t"
                    "vse64.v v13, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v14, (t2)\n\t"
                    "vse64.v v15, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "add t4, t4, -1\n\t"
                    "bnez t4, .packaloop8%=\n\t"

                    ".packaloop8end%=:\n\t"
                    "ld t4, %[nleft]\n\t"
                    "beq t4, zero, .packaloopend%=\n\t"
                    
                    ".packaloop%=:\n\t"
                    "vlse64.v v0, (t0), s4\n\t"
                    "vlse64.v v1, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vse64.v v0, (t2)\n\t"
                    "vse64.v v1, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "add t4, t4, -1\n\t"
                    "bnez t4, .packaloop%=\n\t"
                    ".packaloopend%=:\n\t"
                    : [dummy_p] "+m"(*(double(*)[])p)
                    : [niter] "m" (niter), [a] "m" (a), [p] "m"(p),
                      [nleft] "m" (nleft), [lda] "m" (lda), [ldp] "m" (ldp),
                      [inca] "m" (inca)
                    : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3", "s4",
                      "v0", "v1", "v2", "v3",
                      "v4", "v5", "v6", "v7",
                      "v8", "v9", "v10", "v11",
                      "v12", "v13", "v14", "v15"
                );
            }
        }
        else  // *kappa != 1.0
        {

            if ( inca == 1 )  // continous memory. packA style
            {
                __asm__(
                    "ld t0, %[kappa]\n\t"
                    "fld f0, (t0)\n\t"
                    "ld t0, %[a]\n\t"    // t0 = a
                    "ld t4, %[niter]\n\t"    // t4 = loop counter
                    "vsetvli s3, zero, e64, m1, ta, ma\n\t"
                    "slli s3, s3, 3\n\t" // s3 = vlen
                    "add t1, t0, s3\n\t" // t1 = a+vlen
                    "ld t2, %[p]\n\t"    // t2 = p
                    "add t3, t2, s3\n\t" // t3 = p+vlen
                    "ld t5, %[lda]\n\t"
                    "ld t6, %[ldp]\n\t"
                    "slli t5, t5, 3\n\t"
                    "slli t6, t6, 3\n\t"
                    "beq t4, zero, .packaloop8end%=\n\t"
                    ".packaloop8%=:\n\t"
                    "vle64.v v0, (t0)\n\t"
                    "vle64.v v1, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v0, v0, f0\n\t"
                    "vfmul.vf v1, v1, f0\n\t"

                    "vle64.v v2, (t0)\n\t"
                    "vle64.v v3, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v2, v2, f0\n\t"
                    "vfmul.vf v3, v3, f0\n\t"

                    "vle64.v v4, (t0)\n\t"
                    "vle64.v v5, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v4, v4, f0\n\t"
                    "vfmul.vf v5, v5, f0\n\t"

                    "vle64.v v6, (t0)\n\t"
                    "vle64.v v7, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v6, v6, f0\n\t"
                    "vfmul.vf v7, v7, f0\n\t"
                    "vse64.v v0, (t2)\n\t"
                    "vse64.v v1, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vle64.v v8, (t0)\n\t"
                    "vle64.v v9, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v8, v8, f0\n\t"
                    "vfmul.vf v9, v9, f0\n\t"
                    "vse64.v v2, (t2)\n\t"
                    "vse64.v v3, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vle64.v v10, (t0)\n\t"
                    "vle64.v v11, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v10, v10, f0\n\t"
                    "vfmul.vf v11, v11, f0\n\t"
                    "vse64.v v4, (t2)\n\t"
                    "vse64.v v5, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vle64.v v12, (t0)\n\t"
                    "vle64.v v13, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v12, v12, f0\n\t"
                    "vfmul.vf v13, v13, f0\n\t"
                    "vse64.v v6, (t2)\n\t"
                    "vse64.v v7, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vle64.v v14, (t0)\n\t"
                    "vle64.v v15, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v14, v14, f0\n\t"
                    "vfmul.vf v15, v15, f0\n\t"
                    "vse64.v v8, (t2)\n\t"
                    "vse64.v v9, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v10, (t2)\n\t"
                    "vse64.v v11, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v12, (t2)\n\t"
                    "vse64.v v13, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v14, (t2)\n\t"
                    "vse64.v v15, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "add t4, t4, -1\n\t"
                    "bnez t4, .packaloop8%=\n\t"

                    ".packaloop8end%=:\n\t"
                    "ld t4, %[nleft]\n\t"
                    "beq t4, zero, .packaloopend%=\n\t"
                    
                    ".packaloop%=:\n\t"
                    "vle64.v v0, (t0)\n\t"
                    "vle64.v v1, (t1)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v0, v0, f0\n\t"
                    "vfmul.vf v1, v1, f0\n\t"
                    "vse64.v v0, (t2)\n\t"
                    "vse64.v v1, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "add t4, t4, -1\n\t"
                    "bnez t4, .packaloop%=\n\t"
                    ".packaloopend%=:\n\t"
                    : [dummy_p] "+m"(*(double(*)[])p)
                    : [niter] "m" (niter), [a] "m" (a), [p] "m"(p),
                      [nleft] "m" (nleft), [lda] "m" (lda), [ldp] "m" (ldp),
                      [kappa] "m" (kappa)
                    : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3",
                      "v0", "v1", "v2", "v3",
                      "v4", "v5", "v6", "v7",
                      "v8", "v9", "v10", "v11",
                      "v12", "v13", "v14", "v15"
                );
            }
            else  // gather load/ cont. store. packB style
            {
                __asm__(
                    "ld t0, %[kappa]\n\t"
                    "fld f0, (t0)\n\t"
                    "ld t0, %[a]\n\t"    // t0 = a
                    "ld t4, %[niter]\n\t"    // t4 = loop counter
                    "ld s4, %[inca]\n\t"
                    "vsetvli s3, zero, e64, m1, ta, ma\n\t"
                    "slli s3, s3, 3\n\t" // s3 = vlen
                    "mul t2, s3, s4\n\t" // inca*vlen*sizeof(double)
                    "add t1, t0, t2\n\t" // t1 = a+inca*vlen
                    "ld t2, %[p]\n\t"    // t2 = p
                    "add t3, t2, s3\n\t" // t3 = p+vlen
                    "ld t5, %[lda]\n\t"
                    "ld t6, %[ldp]\n\t"
                    "slli t5, t5, 3\n\t"
                    "slli t6, t6, 3\n\t"
                    "slli s4, s4, 3\n\t"
                    "beq t4, zero, .packaloop8end%=\n\t"
                    ".packaloop8%=:\n\t"
                    "vlse64.v v0, (t0), s4\n\t"
                    "vlse64.v v1, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v0, v0, f0\n\t"
                    "vfmul.vf v1, v1, f0\n\t"

                    "vlse64.v v2, (t0), s4\n\t"
                    "vlse64.v v3, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v2, v2, f0\n\t"
                    "vfmul.vf v3, v3, f0\n\t"

                    "vlse64.v v4, (t0), s4\n\t"
                    "vlse64.v v5, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v4, v4, f0\n\t"
                    "vfmul.vf v5, v5, f0\n\t"

                    "vlse64.v v6, (t0), s4\n\t"
                    "vlse64.v v7, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v6, v6, f0\n\t"
                    "vfmul.vf v7, v7, f0\n\t"
                    "vse64.v v0, (t2)\n\t"
                    "vse64.v v1, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v8, (t0), s4\n\t"
                    "vlse64.v v9, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v8, v8, f0\n\t"
                    "vfmul.vf v9, v9, f0\n\t"
                    "vse64.v v2, (t2)\n\t"
                    "vse64.v v3, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v10, (t0), s4\n\t"
                    "vlse64.v v11, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v10, v10, f0\n\t"
                    "vfmul.vf v11, v11, f0\n\t"
                    "vse64.v v4, (t2)\n\t"
                    "vse64.v v5, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v12, (t0), s4\n\t"
                    "vlse64.v v13, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v12, v12, f0\n\t"
                    "vfmul.vf v13, v13, f0\n\t"
                    "vse64.v v6, (t2)\n\t"
                    "vse64.v v7, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v14, (t0), s4\n\t"
                    "vlse64.v v15, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v14, v14, f0\n\t"
                    "vfmul.vf v15, v15, f0\n\t"
                    "vse64.v v8, (t2)\n\t"
                    "vse64.v v9, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v10, (t2)\n\t"
                    "vse64.v v11, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v12, (t2)\n\t"
                    "vse64.v v13, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vse64.v v14, (t2)\n\t"
                    "vse64.v v15, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "add t4, t4, -1\n\t"
                    "bnez t4, .packaloop8%=\n\t"

                    ".packaloop8end%=:\n\t"
                    "ld t4, %[nleft]\n\t"
                    "beq t4, zero, .packaloopend%=\n\t"
                    
                    ".packaloop%=:\n\t"
                    "vlse64.v v0, (t0), s4\n\t"
                    "vlse64.v v1, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v0, v0, f0\n\t"
                    "vfmul.vf v1, v1, f0\n\t"
                    "vse64.v v0, (t2)\n\t"
                    "vse64.v v1, (t3)\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "add t4, t4, -1\n\t"
                    "bnez t4, .packaloop%=\n\t"
                    ".packaloopend%=:\n\t"
                    : [dummy_p] "+m"(*(double(*)[])p)
                    : [niter] "m" (niter), [a] "m" (a), [p] "m"(p),
                      [nleft] "m" (nleft), [lda] "m" (lda), [ldp] "m" (ldp),
                      [kappa] "m" (kappa), [inca] "m" (inca)
                    : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3", "s4",
                      "v0", "v1", "v2", "v3",
                      "v4", "v5", "v6", "v7",
                      "v8", "v9", "v10", "v11",
                      "v12", "v13", "v14", "v15"
                );
            }
        } // end of if ( *kappa == 1.0 )
    }
    else if ( cdim == nr && cdim_bcast == 1 && inca == 1 )
    {
        double kappa_d =*(( double* )kappa);

        if ( bli_deq1(kappa_d))
        {
            __asm__(
                "ld t0, %[a]\n\t"    // t0 = a
                "add t1, t0, 56\n\t" // t1 = a+7*sizeof(double)
                "ld t4, %[n]\n\t"    // t4 = loop counter
                "ld t2, %[p]\n\t"    // t2 = p
                "ld t5, %[lda]\n\t"
                "ld t6, %[ldp]\n\t"
                "slli t5, t5, 3\n\t"
                "slli t6, t6, 3\n\t"
                "add t3, t2, 56\n\t"
                "sub t3, t3, t6\n\t" // t3 = p+7*sizeof(double) (since we +ldp t3 in the loop the first time)
                "beq t4, zero, .packa_14d_end%=\n\t"
                "fld f0,  0(t0)\n\t"
                "fld f1,  8(t0)\n\t"
                "fld f2, 16(t0)\n\t"
                "fld f3, 24(t0)\n\t"
                "fld f4, 32(t0)\n\t"
                "fld f5, 40(t0)\n\t"
                "fld f6, 48(t0)\n\t"
                "fld f7, 0(t1)\n\t"
                "fld f8, 8(t1)\n\t"
                "fld f9, 16(t1)\n\t"
                "add t0, t0, t5\n\t"
                "add t4, t4, -1\n\t"
                "fld f10, 24(t1)\n\t"
                "fld f11, 32(t1)\n\t"
                "fld f12, 40(t1)\n\t"
                "fld f13, 48(t1)\n\t"
                "beq t4, zero, .packa_14d_last%=\n\t"
                ".packa_14d_loop%=:\n\t"
                    "fsd f0,   0(t2)\n\t"
                    "fsd f1,   8(t2)\n\t"
                    "fsd f2,  16(t2)\n\t"
                    "fsd f3,  24(t2)\n\t"
                    "fld f0,   0(t0)\n\t"
                    "add t1, t1, t5\n\t"
                    "add t3, t3, t6\n\t"
                    "fsd f4,  32(t2)\n\t"
                    "fld f1,   8(t0)\n\t"
                    "fsd f5,  40(t2)\n\t"
                    "fld f2,  16(t0)\n\t"
                    "fsd f6,  48(t2)\n\t"
                    "fld f3,  24(t0)\n\t"
                    "fsd f7,   0(t3)\n\t"
                    "fld f4,  32(t0)\n\t"
                    "fsd f8,   8(t3)\n\t"
                    "add t4, t4, -1\n\t"
                    "fld f5,  40(t0)\n\t"
                    "fsd f9,  16(t3)\n\t"
                    "fld f6,  48(t0)\n\t"
                    "fsd f10, 24(t3)\n\t"
                    "fld f7,   0(t1)\n\t"
                    "fsd f11, 32(t3)\n\t"
                    "fld f8,   8(t1)\n\t"
                    "fsd f12, 40(t3)\n\t"
                    "fld f9,  16(t1)\n\t"
                    "fsd f13, 48(t3)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t2, t2, t6\n\t"
                    "fld f10,  24(t1)\n\t"
                    "fld f11,  32(t1)\n\t"
                    "fld f12,  40(t1)\n\t"
                    "fld f13,  48(t1)\n\t"
                    "bnez t4, .packa_14d_loop%=\n\t"
                ".packa_14d_last%=:\n\t"
                    "fsd f0,   0(t2)\n\t"
                    "fsd f1,   8(t2)\n\t"
                    "fsd f2,  16(t2)\n\t"
                    "fsd f3,  24(t2)\n\t"
                    "add t3, t3, t6\n\t"
                    "fsd f4,  32(t2)\n\t"
                    "fsd f5,  40(t2)\n\t"
                    "fsd f6,  48(t2)\n\t"
                    "fsd f7,   0(t3)\n\t"
                    "fsd f8,   8(t3)\n\t"
                    "fsd f9,  16(t3)\n\t"
                    "fsd f10, 24(t3)\n\t"
                    "fsd f11, 32(t3)\n\t"
                    "fsd f12, 40(t3)\n\t"
                    "fsd f13, 48(t3)\n\t"
                ".packa_14d_end%=:\n\t"
                : [dummy_p] "+m"(*(double(*)[])p)
                : [n] "m" (n), [a] "m" (a), [p] "m"(p),
                  [lda] "m" (lda), [ldp] "m" (ldp)
                : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3", "s4",
                  "f0", "f1", "f2", "f3",
                  "f4", "f5", "f6", "f7",
                  "f8", "f9", "f10", "f11",
                  "f12", "f13"
            );
        }
        else
        {
            __asm__(
                "ld t0, %[a]\n\t"    // t0 = a
                "add t1, t0, 56\n\t" // t1 = a+7*sizeof(double)
                "ld t4, %[n]\n\t"    // t4 = loop counter
                "ld t2, %[p]\n\t"    // t2 = p
                "ld t5, %[lda]\n\t"
                "ld t6, %[ldp]\n\t"
                "slli t5, t5, 3\n\t"
                "slli t6, t6, 3\n\t"
                "add t3, t2, 56\n\t"
                "sub t3, t3, t6\n\t" // t3 = p+7*sizeof(double) (since we +ldp t3 in the loop the first time)
                "fld f0,  0(t0)\n\t"
                "fld f1,  8(t0)\n\t"
                "fld f2, 16(t0)\n\t"
                "fld f3, 24(t0)\n\t"
                "fmul.d f0, f0, %[kappa]\n\t"
                "fld f4, 32(t0)\n\t"
                "fmul.d f1, f1, %[kappa]\n\t"
                "fld f5, 40(t0)\n\t"
                "fmul.d f2, f2, %[kappa]\n\t"
                "fld f6, 48(t0)\n\t"
                "fmul.d f3, f3, %[kappa]\n\t"
                "fld f7, 0(t1)\n\t"
                "fmul.d f4, f4, %[kappa]\n\t"
                "fld f8, 8(t1)\n\t"
                "fmul.d f5, f5, %[kappa]\n\t"
                "fld f9, 16(t1)\n\t"
                "fmul.d f6, f6, %[kappa]\n\t"
                "add t0, t0, t5\n\t"
                "fmul.d f7, f7, %[kappa]\n\t"
                "add t4, t4, -1\n\t"
                "fmul.d f8, f8, %[kappa]\n\t"
                "fld f10, 24(t1)\n\t"
                "fmul.d f9, f9, %[kappa]\n\t"
                "fld f11, 32(t1)\n\t"
                "fmul.d f10, f10, %[kappa]\n\t"
                "fld f12, 40(t1)\n\t"
                "fmul.d f11, f11, %[kappa]\n\t"
                "fld f13, 48(t1)\n\t"
                "fmul.d f12, f12, %[kappa]\n\t"
                "fmul.d f13, f13, %[kappa]\n\t"
                "beq t4, zero, .packa_14d_last%=\n\t"
                ".packa_14d_loop%=:\n\t"
                    "fsd f0,   0(t2)\n\t"
                    "fsd f1,   8(t2)\n\t"
                    "fsd f2,  16(t2)\n\t"
                    "fsd f3,  24(t2)\n\t"
                    "fld f0,   0(t0)\n\t"
                    "add t1, t1, t5\n\t"
                    "add t3, t3, t6\n\t"
                    "fsd f4,  32(t2)\n\t"
                    "fld f1,   8(t0)\n\t"
                    "fsd f5,  40(t2)\n\t"
                    "fld f2,  16(t0)\n\t"
                    "fsd f6,  48(t2)\n\t"
                    "fld f3,  24(t0)\n\t"
                    "fmul.d f0, f0, %[kappa]\n\t"
                    "fsd f7,   0(t3)\n\t"
                    "fld f4,  32(t0)\n\t"
                    "fmul.d f1, f1, %[kappa]\n\t"
                    "fsd f8,   8(t3)\n\t"
                    "add t4, t4, -1\n\t"
                    "fmul.d f2, f2, %[kappa]\n\t"
                    "fld f5,  40(t0)\n\t"
                    "fsd f9,  16(t3)\n\t"
                    "fmul.d f3, f3, %[kappa]\n\t"
                    "fld f6,  48(t0)\n\t"
                    "fsd f10, 24(t3)\n\t"
                    "fmul.d f4, f4, %[kappa]\n\t"
                    "fld f7,   0(t1)\n\t"
                    "fsd f11, 32(t3)\n\t"
                    "fmul.d f5, f5, %[kappa]\n\t"
                    "fld f8,   8(t1)\n\t"
                    "fsd f12, 40(t3)\n\t"
                    "fmul.d f6, f6, %[kappa]\n\t"
                    "fld f9,  16(t1)\n\t"
                    "fsd f13, 48(t3)\n\t"
                    "add t0, t0, t5\n\t"
                    "add t2, t2, t6\n\t"
                    "fmul.d f7, f7, %[kappa]\n\t"
                    "fld f10,  24(t1)\n\t"
                    "fld f11,  32(t1)\n\t"
                    "fmul.d f8, f8, %[kappa]\n\t"
                    "fld f12,  40(t1)\n\t"
                    "fld f13,  48(t1)\n\t"
                    "fmul.d f9, f9, %[kappa]\n\t"
                    "fmul.d f10, f10, %[kappa]\n\t"
                    "fmul.d f11, f11, %[kappa]\n\t"
                    "fmul.d f12, f12, %[kappa]\n\t"
                    "fmul.d f13, f13, %[kappa]\n\t"
                    "bnez t4, .packa_14d_loop%=\n\t"
                ".packa_14d_last%=:\n\t"
                    "fsd f0,   0(t2)\n\t"
                    "fsd f1,   8(t2)\n\t"
                    "fsd f2,  16(t2)\n\t"
                    "fsd f3,  24(t2)\n\t"
                    "add t3, t3, t6\n\t"
                    "fsd f4,  32(t2)\n\t"
                    "fsd f5,  40(t2)\n\t"
                    "fsd f6,  48(t2)\n\t"
                    "fsd f7,   0(t2)\n\t"
                    "fsd f8,   8(t3)\n\t"
                    "fsd f9,  16(t3)\n\t"
                    "fsd f10, 24(t3)\n\t"
                    "fsd f11, 32(t3)\n\t"
                    "fsd f12, 40(t3)\n\t"
                    "fsd f13, 48(t3)\n\t"
                : [dummy_p] "+m"(*(double(*)[])p)
                : [n] "m" (n), [a] "m" (a), [p] "m"(p),
                  [lda] "m" (lda), [ldp] "m" (ldp),
                  [kappa] "f" (kappa_d)
                : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3", "s4",
                  "f0", "f1", "f2", "f3",
                  "f4", "f5", "f6", "f7",
                  "f8", "f9", "f10", "f11",
                  "f12", "f13"
            );
        }
    }
	else
	{
        //printf("packing with cdim=%ld, cdim_bcast=%ld, n=%ld, inca=%ld\n", cdim, cdim_bcast, n, inca);
        //printf("mr=%ld, nr=%ld\n", mr, nr);
		bli_dscal2bbs_mxn
		(
		  conja,
		  cdim_,
		  n_,
		  kappa,
		  a,       inca, lda,
		  p, cdim_bcast, ldp
		);
	}

	bli_dset0s_edge
	(
	  cdim_*cdim_bcast, cdim_max*cdim_bcast,
	  n_, n_max_,
	  p, ldp
	);
}

