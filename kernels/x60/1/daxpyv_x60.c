#include "blis.h"

#include "ukr1v_inputs.h"

#include "ukr1_macros.h"


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
            "vsetvli %[vlen], %[vlen], e8, m1, ta, ma\n\t"
            : [vlen] "+r" (vlen)
            :
            :
       );

    vlen = vlen/sizeof(double);

    uint64_t unroll = 4*2*vlen;
    uint64_t niter = n / unroll;
    uint64_t nleft = n % unroll;

    volatile ukrinputs_t ukrinputs;
    ukrinputs.scalar = alpha;
    ukrinputs.incx = incx;
    ukrinputs.incy = incy;
    ukrinputs.x = x;
    ukrinputs.y = y;
    ukrinputs.n = n;
    ukrinputs.niter = niter;
    ukrinputs.nleft = nleft;

    #ifdef UKR1_4VM2
    ukrinputs.vlen = 2*vlen;
    #else
    ukrinputs.vlen = vlen;
    #endif

    #define SIZESHIFT "3"
    #define SIZEBITS  "64"
    #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0
    #define VTRANSFORM VFMA_F0
    #define VXTOY VSECOND
    #define LDIMFIXUP(fixup) fixup

    if ((incx == 1) && (incy == 1))
    {
        #define LABELPREFIX "axpy1x1y"
        #define VLOADX VLOAD
        #define VLOADY VLOAD
        #define VSTOREY VSTORE
        #define PREPARE_STRIDEX PREPARE_STRIDE_C
        #define PREPARE_STRIDEY PREPARE_STRIDE_C
        #define PREPARE_LDIMX(ldimreg, offset, sizeshift) VSTRIDE_FROM_1STRIDE_C(ldimreg, STRIDE1_X, sizeshift)
        #define PREPARE_LDIMY(ldimreg, offset, sizeshift) VSTRIDE_FROM_1STRIDE_C(ldimreg, STRIDE1_Y, sizeshift)

        #include UKRINCLUDE
    }
    else if (incx == 1)
    {
        #define LABELPREFIX "axpy1xny"
        #define VLOADX VLOAD
        #define VLOADY(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, STRIDE1_Y)
        #define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, STRIDE1_Y)
        #define PREPARE_STRIDEX PREPARE_STRIDE_C
        #define PREPARE_STRIDEY PREPARE_STRIDE_G
        #define PREPARE_LDIMX(ldimreg, offset, sizeshift) VSTRIDE_FROM_1STRIDE_C(ldimreg, STRIDE1_X, sizeshift)
        #define PREPARE_LDIMY(ldimreg, offset, sizeshift) VSTRIDE_FROM_1STRIDE_G(ldimreg, STRIDE1_Y, sizeshift)

        #include UKRINCLUDE
    }
    else if (incy == 1)
    {
        #define LABELPREFIX "axpynx1y"
        #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, STRIDE1_X)
        #define VLOADY VLOAD
        #define VSTOREY VSTORE
        #define PREPARE_STRIDEX PREPARE_STRIDE_G
        #define PREPARE_STRIDEY PREPARE_STRIDE_C
        #define PREPARE_LDIMX(ldimreg, offset, sizeshift) VSTRIDE_FROM_1STRIDE_G(ldimreg, STRIDE1_X, sizeshift)
        #define PREPARE_LDIMY(ldimreg, offset, sizeshift) VSTRIDE_FROM_1STRIDE_C(ldimreg, STRIDE1_Y, sizeshift)

        #include UKRINCLUDE
    }
    else
    {
        #define LABELPREFIX "axpynxny"
        #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, STRIDE1_X)
        #define VLOADY(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, STRIDE1_Y)
        #define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, STRIDE1_Y)
        #define PREPARE_STRIDEX PREPARE_STRIDE_G
        #define PREPARE_STRIDEY PREPARE_STRIDE_G
        #define PREPARE_LDIMX(ldimreg, offset, sizeshift) VSTRIDE_FROM_1STRIDE_G(ldimreg, STRIDE1_X, sizeshift)
        #define PREPARE_LDIMY(ldimreg, offset, sizeshift) VSTRIDE_FROM_1STRIDE_G(ldimreg, STRIDE1_Y, sizeshift)

        #include UKRINCLUDE
    }
}
