#include "blis.h"

void bli_dcopyv_x60(
             conj_t  conjx,
             dim_t   n,
       const void*  x, inc_t incx_,
             void*  y, inc_t incy_,
       const cntx_t* cntx)
{
    
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
            "vsetvli s3, zero, e64, m1, ta, ma\n\t"
            "slli s3, s3, 3\n\t"
            "slli t6, s3, 1\n\t"
            "ld t0, %[x]\n\t"
            "add t1, t0, s3\n\t"
            "ld t2, %[y]\n\t"
            "add t3, t2, s3\n\t"

            "ld t4, %[niter]\n\t"
            "beq t4, zero, .copy8end%=\n\t"
            ".copy8loop%=:\n\t"

                "vle64.v v0, (t0)\n\t"
                "vle64.v v1, (t1)\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v0, (t2)\n\t"
                "vse64.v v1, (t3)\n\t"

                "vle64.v v4, (t0)\n\t"
                "vle64.v v5, (t1)\n\t"
                "add t2, t2, t6\n\t"
                "add t3, t3, t6\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v4, (t2)\n\t"
                "vse64.v v5, (t3)\n\t"

                "vle64.v v8, (t0)\n\t"
                "vle64.v v9, (t1)\n\t"
                "add t2, t2, t6\n\t"
                "add t3, t3, t6\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v8, (t2)\n\t"
                "vse64.v v9, (t3)\n\t"

                "vle64.v v12, (t0)\n\t"
                "vle64.v v13, (t1)\n\t"
                "add t2, t2, t6\n\t"
                "add t3, t3, t6\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v12, (t2)\n\t"
                "vse64.v v13, (t3)\n\t"

                "add t2, t2, t6\n\t"
                "add t3, t3, t6\n\t"

                "add t4, t4, -1\n\t"
                "bnez t4, .copy8loop%=\n\t"

            ".copy8end%=:\n\t"
            "ld t4, %[nleft]\n\t"
            "beq t4, zero, .copyend%=\n\t"
            ".copyloop%=:\n\t"
                "vsetvli s3, t4, e64, m1, ta, ma\n\t"
                "slli t1, s3, 3\n\t"
                "vle64.v v0, (t0)\n\t"
                "add t0, t0, t1\n\t"
                "vse64.v v0, (t2)\n\t"
                "add t2, t2, t1\n\t"
                "sub t4, t4, s3\n\t"
                "bnez t4, .copyloop%=\n\t"
            ".copyend%=:\n\t"
            : [dummy_y] "+m"(*(double(*)[])y)
            : [niter] "m" (niter), [nleft] "m" (nleft),
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
            "beq t4, zero, .copy8end%=\n\t"
            ".copy8loop%=:\n\t"

                "vle64.v v0, (t0)\n\t"
                "vle64.v v1, (t1)\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v0, (t2), %[incy]\n\t"
                "vsse64.v v1, (t3), %[incy]\n\t"

                "vle64.v v4, (t0)\n\t"
                "vle64.v v5, (t1)\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v4, (t2), %[incy]\n\t"
                "vsse64.v v5, (t3), %[incy]\n\t"

                "vle64.v v8, (t0)\n\t"
                "vle64.v v9, (t1)\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v8, (t2), %[incy]\n\t"
                "vsse64.v v9, (t3), %[incy]\n\t"

                "vle64.v v12, (t0)\n\t"
                "vle64.v v13, (t1)\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v12, (t2), %[incy]\n\t"
                "vsse64.v v13, (t3), %[incy]\n\t"

                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"

                "add t4, t4, -1\n\t"
                "bnez t4, .copy8loop%=\n\t"

            ".copy8end%=:\n\t"
            "ld t4, %[nleft]\n\t"
            "beq t4, zero, .copyend%=\n\t"
            ".copyloop%=:\n\t"
                "vsetvli s3, t4, e64, m1, ta, ma\n\t"
                "slli t1, s3, 3\n\t"
                "mul t5, s3, %[incy]\n\t"
                "vle64.v v0, (t0)\n\t"
                "add t0, t0, t1\n\t"
                "vsse64.v v0, (t2), %[incy]\n\t"
                "add t2, t2, t5\n\t"
                "sub t4, t4, s3\n\t"
                "bnez t4, .copyloop%=\n\t"
            ".copyend%=:\n\t"
            : [dummy_y] "+m"(*(double(*)[])y)
            : [niter] "m" (niter), [nleft] "m" (nleft),
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
            "beq t4, zero, .copy8end%=\n\t"
            ".copy8loop%=:\n\t"

                "vlse64.v v0, (t0), %[incx]\n\t"
                "vlse64.v v1, (t1), %[incx]\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v0, (t2)\n\t"
                "vse64.v v1, (t3)\n\t"

                "vlse64.v v4, (t0), %[incx]\n\t"
                "vlse64.v v5, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v4, (t2)\n\t"
                "vse64.v v5, (t3)\n\t"

                "vlse64.v v8, (t0), %[incx]\n\t"
                "vlse64.v v9, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v8, (t2)\n\t"
                "vse64.v v9, (t3)\n\t"

                "vlse64.v v12, (t0), %[incx]\n\t"
                "vlse64.v v13, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vse64.v v12, (t2)\n\t"
                "vse64.v v13, (t3)\n\t"

                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"

                "add t4, t4, -1\n\t"
                "bnez t4, .copy8loop%=\n\t"

            ".copy8end%=:\n\t"
            "ld t4, %[nleft]\n\t"
            "beq t4, zero, .copyend%=\n\t"
            ".copyloop%=:\n\t"
                "vsetvli s3, t4, e64, m1, ta, ma\n\t"
                "slli t1, s3, 3\n\t"
                "mul t6, s3, %[incx]\n\t"
                "vlse64.v v0, (t0), %[incx]\n\t"
                "add t0, t0, t6\n\t"
                "vse64.v v0, (t2)\n\t"
                "add t2, t2, t1\n\t"
                "sub t4, t4, s3\n\t"
                "bnez t4, .copyloop%=\n\t"
            ".copyend%=:\n\t"
            : [dummy_y] "+m"(*(double(*)[])y)
            : [niter] "m" (niter), [nleft] "m" (nleft),
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
            "beq t4, zero, .copy8end%=\n\t"
            ".copy8loop%=:\n\t"

                "vlse64.v v0, (t0), %[incx]\n\t"
                "vlse64.v v1, (t1), %[incx]\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v0, (t2), %[incy]\n\t"
                "vsse64.v v1, (t3), %[incy]\n\t"

                "vlse64.v v4, (t0), %[incx]\n\t"
                "vlse64.v v5, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v4, (t2), %[incy]\n\t"
                "vsse64.v v5, (t3), %[incy]\n\t"

                "vlse64.v v8, (t0), %[incx]\n\t"
                "vlse64.v v9, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v8, (t2), %[incy]\n\t"
                "vsse64.v v9, (t3), %[incy]\n\t"

                "vlse64.v v12, (t0), %[incx]\n\t"
                "vlse64.v v13, (t1), %[incx]\n\t"
                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"
                "add t0, t0, t6\n\t"
                "add t1, t1, t6\n\t"
                "vsse64.v v12, (t2), %[incy]\n\t"
                "vsse64.v v13, (t3), %[incy]\n\t"

                "add t2, t2, t5\n\t"
                "add t3, t3, t5\n\t"

                "add t4, t4, -1\n\t"
                "bnez t4, .copy8loop%=\n\t"

            ".copy8end%=:\n\t"
            "ld t4, %[nleft]\n\t"
            "beq t4, zero, .copyend%=\n\t"
            ".copyloop%=:\n\t"
                "vsetvli s3, t4, e64, m1, ta, ma\n\t"
                "mul t6, s3, %[incx]\n\t"
                "mul t5, s3, %[incy]\n\t"
                "vlse64.v v0, (t0), %[incx]\n\t"
                "add t0, t0, t6\n\t"
                "vsse64.v v0, (t2), %[incy]\n\t"
                "add t2, t2, t5\n\t"
                "sub t4, t4, s3\n\t"
                "bnez t4, .copyloop%=\n\t"
            ".copyend%=:\n\t"
            : [dummy_y] "+m"(*(double(*)[])y)
            : [niter] "m" (niter), [nleft] "m" (nleft),
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
