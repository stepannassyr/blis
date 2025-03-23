#include "blis.h"

void bli_daxpyv_x60(
             conj_t  conjx,
             dim_t   n,
       const void*  alpha,
       const void*  x, inc_t incx_,
             void*  y, inc_t incy_,
       const cntx_t* cntx)
{
    if (bli_deq0(* ((double*)alpha)))
    {
        return;
    }

    uint64_t incx = incx_;
    uint64_t incy = incy_;

    uint64_t vlen;
    __asm__(
        "csrr %[vlen],vlenb\n\t"
        : [vlen] "=r" (vlen)
        :
        :
    );

    vlen = vlen/sizeof(double);

    uint64_t unroll = 8*vlen;
    uint64_t niter = n / unroll;
    uint64_t nleft = n % unroll;


    if ((incx == 1) && (incy == 1))
    {
        __asm__ (
            "fld f0, (%[alpha])\n\t"
            "vsetvli s3, zero, e64, m1, ta, ma\n\t"
            "slli s3, s3, 3\n\t"
            "slli t6, s3, 1\n\t"
            "ld t0, %[x]\n\t"
            "add t1, t0, s3\n\t"
            "ld t2, %[y]\n\t"
            "add t3, t2, s3\n\t"

            "ld t4, %[niter]\n\t"
            "beq t4, zero, .axpy8end%=\n\t"
            ".axpy8loop%=:\n\t"

                "vle64.v v0, (t0)\n\t"
                "vle64.v v1, (t1)\n\t"
                "vle64.v v2, (t2)\n\t"
                "vle64.v v3, (t3)\n\t"
                "vfmacc.vf v2, f0, v0\n\t"
                "vfmacc.vf v3, f0, v1\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v2, (t2)\n\t"
                "vse64.v v3, (t3)\n\t"

                "vle64.v v4, (t0)\n\t"
                "vle64.v v5, (t1)\n\t"
                "add t2, t2, t6\n\t"
                "add t3, t3, t6\n\t"
                "vle64.v v6, (t2)\n\t"
                "vle64.v v7, (t3)\n\t"
                "vfmacc.vf v6, f0, v4\n\t"
                "vfmacc.vf v7, f0, v5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v6, (t2)\n\t"
                "vse64.v v7, (t3)\n\t"

                "vle64.v v8, (t0)\n\t"
                "vle64.v v9, (t1)\n\t"
                "add t2, t2, t6\n\t"
                "add t3, t3, t6\n\t"
                "vle64.v v10, (t2)\n\t"
                "vle64.v v11, (t3)\n\t"
                "vfmacc.vf v10, f0, v8\n\t"
                "vfmacc.vf v11, f0, v9\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v10, (t2)\n\t"
                "vse64.v v11, (t3)\n\t"

                "vle64.v v12, (t0)\n\t"
                "vle64.v v13, (t1)\n\t"
                "add t2, t2, t6\n\t"
                "add t3, t3, t6\n\t"
                "vle64.v v14, (t2)\n\t"
                "vle64.v v15, (t3)\n\t"
                "vfmacc.vf v14, f0, v12\n\t"
                "vfmacc.vf v15, f0, v13\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v14, (t2)\n\t"
                "vse64.v v15, (t3)\n\t"

                "add t2, t2, t6\n\t"
                "add t3, t3, t6\n\t"

                "add t4, t4, -1\n\t"
                "bnez t4, .axpy8loop%=\n\t"

            ".axpy8end%=:\n\t"
            "ld t4, %[nleft]\n\t"
            "beq t4, zero, .axpyend%=\n\t"
            ".axpyloop%=:\n\t"
                "vsetvli s3, t4, e64, m1, ta, ma\n\t"
                "slli t1, s3, 3\n\t"
                "vle64.v v0, (t0)\n\t"
                "vle64.v v2, (t2)\n\t"
                "add t0, t0, t1\n\t"
                "vfmacc.vf v2, f0, v0\n\t"
                "vse64.v v2, (t2)\n\t"
                "add t2, t2, t1\n\t"
                "sub t4, t4, s3\n\t"
                "bnez t4, .axpyloop%=\n\t"
            ".axpyend%=:\n\t"
            : [dummy_y] "+m"(*(double(*)[])y)
            : [alpha] "r" (alpha), [niter] "m" (niter), [nleft] "m" (nleft),
              [x] "m" (x), [y] "m" (y)
            : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15"
        );
    }
    else if (incx == 1)
    {
        __asm__ (
            "fld f0, (%[alpha])\n\t"
            "vsetvli s3, zero, e64, m1, ta, ma\n\t"
            "slli %[incy], %[incy], 3\n\t"
            "mul t5, s3, %[incy]\n\t"
            "slli s3, s3, 3\n\t"
            "slli t6, s3, 1\n\t"
            "ld t0, %[x]\n\t"
            "add t1, t0, s3\n\t"
            "ld t2, %[y]\n\t"
            "add t3, t2, t5\n\t"

            "slli t5, t5, 1\n\t"

            "ld t4, %[niter]\n\t"
            "beq t4, zero, .axpy8end%=\n\t"
            ".axpy8loop%=:\n\t"

                "vle64.v v0, (t0)\n\t"
                "vle64.v v1, (t1)\n\t"
                "vlse64.v v2, (t2), %[incy]\n\t"
                "vlse64.v v3, (t3), %[incy]\n\t"
                "vfmacc.vf v2, f0, v0\n\t"
                "vfmacc.vf v3, f0, v1\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v2, (t2), %[incy]\n\t"
                "vsse64.v v3, (t3), %[incy]\n\t"

                "vle64.v v4, (t0)\n\t"
                "vle64.v v5, (t1)\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "vlse64.v v6, (t2), %[incy]\n\t"
                "vlse64.v v7, (t3), %[incy]\n\t"
                "vfmacc.vf v6, f0, v4\n\t"
                "vfmacc.vf v7, f0, v5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v6, (t2), %[incy]\n\t"
                "vsse64.v v7, (t3), %[incy]\n\t"

                "vle64.v v8, (t0)\n\t"
                "vle64.v v9, (t1)\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "vlse64.v v10, (t2), %[incy]\n\t"
                "vlse64.v v11, (t3), %[incy]\n\t"
                "vfmacc.vf v10, f0, v8\n\t"
                "vfmacc.vf v11, f0, v9\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v10, (t2), %[incy]\n\t"
                "vsse64.v v11, (t3), %[incy]\n\t"

                "vle64.v v12, (t0)\n\t"
                "vle64.v v13, (t1)\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "vlse64.v v14, (t2), %[incy]\n\t"
                "vlse64.v v15, (t3), %[incy]\n\t"
                "vfmacc.vf v14, f0, v12\n\t"
                "vfmacc.vf v15, f0, v13\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v14, (t2), %[incy]\n\t"
                "vsse64.v v15, (t3), %[incy]\n\t"

                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"

                "add t4, t4, -1\n\t"
                "bnez t4, .axpy8loop%=\n\t"

            ".axpy8end%=:\n\t"
            "ld t4, %[nleft]\n\t"
            "beq t4, zero, .axpyend%=\n\t"
            ".axpyloop%=:\n\t"
                "vsetvli s3, t4, e64, m1, ta, ma\n\t"
                "slli t1, s3, 3\n\t"
                "mul t5, s3, %[incy]\n\t"
                "vle64.v v0, (t0)\n\t"
                "vlse64.v v2, (t2), %[incy]\n\t"
                "add t0, t0, t1\n\t"
                "vfmacc.vf v2, f0, v0\n\t"
                "vsse64.v v2, (t2), %[incy]\n\t"
                "add t2, t2, t5\n\t"
                "sub t4, t4, s3\n\t"
                "bnez t4, .axpyloop%=\n\t"
            ".axpyend%=:\n\t"
            : [dummy_y] "+m"(*(double(*)[])y)
            : [alpha] "r" (alpha), [niter] "m" (niter), [nleft] "m" (nleft),
              [x] "m" (x), [y] "m" (y),
              [incy] "r" (incy)
            : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15"
        );
    }
    else if (incy == 1)
    {
        __asm__ (
            "fld f0, (%[alpha])\n\t"
            "vsetvli s3, zero, e64, m1, ta, ma\n\t"
            "slli %[incx], %[incx], 3\n\t"
            "mul t6, s3, %[incx]\n\t"
            "slli s3, s3, 3\n\t"
            "slli t5, s3, 1\n\t"
            "ld t0, %[x]\n\t"
            "add t1, t0, t6\n\t"
            "ld t2, %[y]\n\t"
            "add t3, t2, s3\n\t"

            "slli t6, t6, 1\n\t"

            "ld t4, %[niter]\n\t"
            "beq t4, zero, .axpy8end%=\n\t"
            ".axpy8loop%=:\n\t"

                "vlse64.v v0, (t0), %[incx]\n\t"
                "vlse64.v v1, (t1), %[incx]\n\t"
                "vle64.v v2, (t2)\n\t"
                "vle64.v v3, (t3)\n\t"
                "vfmacc.vf v2, f0, v0\n\t"
                "vfmacc.vf v3, f0, v1\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v2, (t2)\n\t"
                "vse64.v v3, (t3)\n\t"

                "vlse64.v v4, (t0), %[incx]\n\t"
                "vlse64.v v5, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "vle64.v v6, (t2)\n\t"
                "vle64.v v7, (t3)\n\t"
                "vfmacc.vf v6, f0, v4\n\t"
                "vfmacc.vf v7, f0, v5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v6, (t2)\n\t"
                "vse64.v v7, (t3)\n\t"

                "vlse64.v v8, (t0), %[incx]\n\t"
                "vlse64.v v9, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "vle64.v v10, (t2)\n\t"
                "vle64.v v11, (t3)\n\t"
                "vfmacc.vf v10, f0, v8\n\t"
                "vfmacc.vf v11, f0, v9\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v10, (t2)\n\t"
                "vse64.v v11, (t3)\n\t"

                "vlse64.v v12, (t0), %[incx]\n\t"
                "vlse64.v v13, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "vle64.v v14, (t2)\n\t"
                "vle64.v v15, (t3)\n\t"
                "vfmacc.vf v14, f0, v12\n\t"
                "vfmacc.vf v15, f0, v13\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v14, (t2)\n\t"
                "vse64.v v15, (t3)\n\t"

                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"

                "add t4, t4, -1\n\t"
                "bnez t4, .axpy8loop%=\n\t"

            ".axpy8end%=:\n\t"
            "ld t4, %[nleft]\n\t"
            "beq t4, zero, .axpyend%=\n\t"
            ".axpyloop%=:\n\t"
                "vsetvli s3, t4, e64, m1, ta, ma\n\t"
                "mul t6, s3, %[incx]\n\t"
                "slli t1, s3, 3\n\t"
                "vlse64.v v0, (t0), %[incx]\n\t"
                "vle64.v v2, (t2)\n\t"
                "add t0, t0, t6\n\t"
                "vfmacc.vf v2, f0, v0\n\t"
                "vse64.v v2, (t2)\n\t"
                "add t2, t2, t1\n\t"
                "sub t4, t4, s3\n\t"
                "bnez t4, .axpyloop%=\n\t"
            ".axpyend%=:\n\t"
            : [dummy_y] "+m"(*(double(*)[])y)
            : [alpha] "r" (alpha), [niter] "m" (niter), [nleft] "m" (nleft),
              [x] "m" (x), [y] "m" (y),
              [incx] "r" (incx)
            : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15"
        );
    }
    else
    {
        __asm__ (
            "fld f0, (%[alpha])\n\t"
            "vsetvli s3, zero, e64, m1, ta, ma\n\t"
            "slli %[incx], %[incx], 3\n\t"
            "slli %[incy], %[incy], 3\n\t"
            "mul t6, s3, %[incx]\n\t"
            "mul t5, s3, %[incy]\n\t"
            "ld t0, %[x]\n\t"
            "add t1, t0, t6\n\t"
            "ld t2, %[y]\n\t"
            "add t3, t2, t5\n\t"

            "slli t5, t5, 1\n\t"
            "slli t6, t6, 1\n\t"

            "ld t4, %[niter]\n\t"
            "beq t4, zero, .axpy8end%=\n\t"
            ".axpy8loop%=:\n\t"

                "vlse64.v v0, (t0), %[incx]\n\t"
                "vlse64.v v1, (t1), %[incx]\n\t"
                "vlse64.v v2, (t2), %[incy]\n\t"
                "vlse64.v v3, (t3), %[incy]\n\t"
                "vfmacc.vf v2, f0, v0\n\t"
                "vfmacc.vf v3, f0, v1\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v2, (t2), %[incy]\n\t"
                "vsse64.v v3, (t3), %[incy]\n\t"

                "vlse64.v v4, (t0), %[incx]\n\t"
                "vlse64.v v5, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "vlse64.v v6, (t2), %[incy]\n\t"
                "vlse64.v v7, (t3), %[incy]\n\t"
                "vfmacc.vf v6, f0, v4\n\t"
                "vfmacc.vf v7, f0, v5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v6, (t2), %[incy]\n\t"
                "vsse64.v v7, (t3), %[incy]\n\t"

                "vlse64.v v8, (t0), %[incx]\n\t"
                "vlse64.v v9, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "vlse64.v v10, (t2), %[incy]\n\t"
                "vlse64.v v11, (t3), %[incy]\n\t"
                "vfmacc.vf v10, f0, v8\n\t"
                "vfmacc.vf v11, f0, v9\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v10, (t2), %[incy]\n\t"
                "vsse64.v v11, (t3), %[incy]\n\t"

                "vlse64.v v12, (t0), %[incx]\n\t"
                "vlse64.v v13, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "vlse64.v v14, (t2), %[incy]\n\t"
                "vlse64.v v15, (t3), %[incy]\n\t"
                "vfmacc.vf v14, f0, v12\n\t"
                "vfmacc.vf v15, f0, v13\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v14, (t2), %[incy]\n\t"
                "vsse64.v v15, (t3), %[incy]\n\t"

                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"

                "add t4, t4, -1\n\t"
                "bnez t4, .axpy8loop%=\n\t"

            ".axpy8end%=:\n\t"
            "ld t4, %[nleft]\n\t"
            "beq t4, zero, .axpyend%=\n\t"
            ".axpyloop%=:\n\t"
                "vsetvli s3, t4, e64, m1, ta, ma\n\t"
                "mul t6, s3, %[incx]\n\t"
                "mul t5, s3, %[incy]\n\t"
                "vlse64.v v0, (t0), %[incx]\n\t"
                "vlse64.v v2, (t2), %[incy]\n\t"
                "add t0, t0, t6\n\t"
                "vfmacc.vf v2, f0, v0\n\t"
                "vsse64.v v2, (t2), %[incy]\n\t"
                "add t2, t2, t5\n\t"
                "sub t4, t4, s3\n\t"
                "bnez t4, .axpyloop%=\n\t"
            ".axpyend%=:\n\t"
            : [dummy_y] "+m"(*(double(*)[])y)
            : [alpha] "r" (alpha), [niter] "m" (niter), [nleft] "m" (nleft),
              [x] "m" (x), [y] "m" (y),
              [incx] "r" (incx), [incy] "r" (incy)
            : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s3",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15"
        );
    }
}
