#include "blis.h"

#include "ukr1_macros.h"

typedef struct
{
    uint64_t n;        // 0
    uint64_t niter;    // 8
    uint64_t nleft;    // 16
    uint64_t vlen;     // 24
    int64_t incx;      // 32
    int64_t incy;      // 40
    const void* alpha; // 48
    const void* x;     // 56
          void* y;     // 64
    
} ukrinputs_t;

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

    int64_t incx = incx_;
    int64_t incy = incy_;

    // vlen should be half of MR
    uint64_t vlen = bli_cntx_get_blksz_def_dt( BLIS_DOUBLE, BLIS_MR, cntx )/2;

    vlen *= sizeof(double);

    // override vlen
    __asm__(
            //"csrr %[vlen],vlenb\n\t"
            "vsetvli %[vlen], %[vlen], e8, m1\n\t"
            : [vlen] "+r" (vlen)
            :
            :
       );

    vlen = vlen/sizeof(double);

    uint64_t unroll = 8*vlen;
    uint64_t niter = n / unroll;
    uint64_t nleft = n % unroll;

    volatile ukrinputs_t ukrinputs;
    ukrinputs.alpha = alpha;
    ukrinputs.incx = incx;
    ukrinputs.incy = incy;
    ukrinputs.x = x;
    ukrinputs.y = y;
    ukrinputs.n = n;
    ukrinputs.niter = niter;
    ukrinputs.nleft = nleft;
    ukrinputs.vlen = vlen;

    #define SIZESHIFT "3"
    #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0
    #define VTRANSFORM VFMA_F0
    #define VXTOY VSECOND

    if ((incx == 1) && (incy == 1))
    {
        #define LABELPREFIX "axpy1x1y"
        #define VLOADX VLOAD
        #define VLOADY VLOAD
        #define VSTOREY VSTORE
        #define PREPARE_STRIDEX PREPARE_STRIDE_C
        #define PREPARE_STRIDEY PREPARE_STRIDE_C
        #define VSTRIDE_FROM_1STRIDEX VSTRIDE_FROM_1STRIDE_C
        #define VSTRIDE_FROM_1STRIDEY VSTRIDE_FROM_1STRIDE_C

        #include "ukr1_8v.h"
    }
    else if (incx == 1)
    {
        #define LABELPREFIX "axpy1xny"
        #define VLOADX VLOAD
        #define VLOADY(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, STRIDE1_Y)
        #define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, STRIDE1_Y)
        #define PREPARE_STRIDEX PREPARE_STRIDE_C
        #define PREPARE_STRIDEY PREPARE_STRIDE_G
        #define VSTRIDE_FROM_1STRIDEX VSTRIDE_FROM_1STRIDE_C
        #define VSTRIDE_FROM_1STRIDEY VSTRIDE_FROM_1STRIDE_G

        #include "ukr1_8v.h"
    }
    else if (incy == 1)
    {
        #define LABELPREFIX "axpynx1y"
        #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, STRIDE1_X)
        #define VLOADY VLOAD
        #define VSTOREY VSTORE
        #define PREPARE_STRIDEX PREPARE_STRIDE_G
        #define PREPARE_STRIDEY PREPARE_STRIDE_C
        #define VSTRIDE_FROM_1STRIDEX VSTRIDE_FROM_1STRIDE_G
        #define VSTRIDE_FROM_1STRIDEY VSTRIDE_FROM_1STRIDE_C

        #include "ukr1_8v.h"
    }
    else
    {
        #define LABELPREFIX "axpynxny"
        #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, STRIDE1_X)
        #define VLOADY(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, STRIDE1_Y)
        #define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, STRIDE1_Y)
        #define PREPARE_STRIDEX PREPARE_STRIDE_G
        #define PREPARE_STRIDEY PREPARE_STRIDE_G
        #define VSTRIDE_FROM_1STRIDEX VSTRIDE_FROM_1STRIDE_G
        #define VSTRIDE_FROM_1STRIDEY VSTRIDE_FROM_1STRIDE_G

        #include "ukr1_8v.h"
    }
}
