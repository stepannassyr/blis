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
    const int64_t mr    = 2*vlen;

    const int64_t n     = n_;
    const int64_t inca  = inca_;
    const int64_t lda   = lda_;
    const int64_t ldp   = ldp_;

    const uint64_t unroll = 8;
    const uint64_t niter = n/unroll;
    const uint64_t nleft = n % unroll;


    if ( cdim == mr && cdim_bcast == 1 )
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
                    "add t2, t2, s3\n\t" // t3 = p+vlen
                    "ld t5, %[lda]\n\t"
                    "ld t6, %[ldp]\n\t"
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
            else  // gather/scatter load/store. packB style
            {
                __asm__(
                    "ld t0, %[a]\n\t"    // t0 = a
                    "ld t4, %[niter]\n\t"    // t4 = loop counter
                    "vsetvli s3, zero, e64, m1, ta, ma\n\t"
                    "slli s3, s3, 3\n\t" // s3 = vlen
                    "add t1, t0, s3\n\t" // t1 = a+vlen
                    "ld t2, %[p]\n\t"    // t2 = p
                    "add t2, t2, s3\n\t" // t3 = p+vlen
                    "ld t5, %[lda]\n\t"
                    "ld t6, %[ldp]\n\t"
                    "ld s4, %[inca]\n\t"
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
                    "vsse64.v v0, (t2), s4\n\t"
                    "vsse64.v v1, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v8, (t0), s4\n\t"
                    "vlse64.v v9, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vsse64.v v2, (t2), s4\n\t"
                    "vsse64.v v3, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v10, (t0), s4\n\t"
                    "vlse64.v v11, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vsse64.v v4, (t2), s4\n\t"
                    "vsse64.v v5, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v12, (t0), s4\n\t"
                    "vlse64.v v13, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vsse64.v v6, (t2), s4\n\t"
                    "vsse64.v v7, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v14, (t0), s4\n\t"
                    "vlse64.v v15, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vsse64.v v8, (t2), s4\n\t"
                    "vsse64.v v9, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vsse64.v v10, (t2), s4\n\t"
                    "vsse64.v v11, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vsse64.v v12, (t2), s4\n\t"
                    "vsse64.v v13, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vsse64.v v14, (t2), s4\n\t"
                    "vsse64.v v15, (t3), s4\n\t"
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
                    "vsse64.v v0, (t2), s4\n\t"
                    "vsse64.v v1, (t3), s4\n\t"
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
                    "add t2, t2, s3\n\t" // t3 = p+vlen
                    "ld t5, %[lda]\n\t"
                    "ld t6, %[ldp]\n\t"
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
            else  // gather/scatter load/store. packB style
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
                    "add t2, t2, s3\n\t" // t3 = p+vlen
                    "ld t5, %[lda]\n\t"
                    "ld t6, %[ldp]\n\t"
                    "ld s4, %[inca]\n\t"
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
                    "vsse64.v v0, (t2), s4\n\t"
                    "vsse64.v v1, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v8, (t0), s4\n\t"
                    "vlse64.v v9, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v8, v8, f0\n\t"
                    "vfmul.vf v9, v9, f0\n\t"
                    "vsse64.v v2, (t2), s4\n\t"
                    "vsse64.v v3, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v10, (t0), s4\n\t"
                    "vlse64.v v11, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v10, v10, f0\n\t"
                    "vfmul.vf v11, v11, f0\n\t"
                    "vsse64.v v4, (t2), s4\n\t"
                    "vsse64.v v5, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v12, (t0), s4\n\t"
                    "vlse64.v v13, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v12, v12, f0\n\t"
                    "vfmul.vf v13, v13, f0\n\t"
                    "vsse64.v v6, (t2), s4\n\t"
                    "vsse64.v v7, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vlse64.v v14, (t0), s4\n\t"
                    "vlse64.v v15, (t1), s4\n\t"
                    "add t0, t0, t5\n\t"
                    "add t1, t1, t5\n\t"
                    "vfmul.vf v14, v14, f0\n\t"
                    "vfmul.vf v15, v15, f0\n\t"
                    "vsse64.v v8, (t2), s4\n\t"
                    "vsse64.v v9, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vsse64.v v10, (t2), s4\n\t"
                    "vsse64.v v11, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vsse64.v v12, (t2), s4\n\t"
                    "vsse64.v v13, (t3), s4\n\t"
                    "add t2, t2, t6\n\t"
                    "add t3, t3, t6\n\t"

                    "vsse64.v v14, (t2), s4\n\t"
                    "vsse64.v v15, (t3), s4\n\t"
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
                    "vsse64.v v0, (t2), s4\n\t"
                    "vsse64.v v1, (t3), s4\n\t"
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
	else
	{
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

