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

    const void* scalarptr = alpha;
    uint64_t xstride1 = incx;
    uint64_t ystride1 = incy;
    uint64_t ldimx = 1;
    uint64_t ldimy = 1;

    #define MAKEUNROLL MAKEUNROLL_FROMG

    #define SIZESHIFT "3"
    #define SIZEBITS  "64"
    #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0
    #define VTRANSFORM VFMA_F0
    #define VXTOY VSECOND
    #define LDIMFIXUP(fixup) fixup

    #define PREPARE_LDIMX(strideregvlen, ldimreg, sizeshift)
    #define PREPARE_LDIMY(strideregvlen, ldimreg, sizeshift)

    if ((incx == 1) && (incy == 1))
    {
        #define LABELPREFIX "axpy1x1y"
        #define VLOADX VLOAD
        #define VLOADY VLOAD
        #define VSTOREY VSTORE
        #define PREPARE_STRIDEX PREPARE_STRIDE_C
        #define PREPARE_STRIDEY PREPARE_STRIDE_C
        #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_C
        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_C

        #include UKRINCLUDE
    }
    else if (incx == 1)
    {
        #define LABELPREFIX "axpy1xny"
        #define VLOADX VLOAD
        #define VLOADY(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[ystride1]")
        #define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, "%[ystride1]")
        #define PREPARE_STRIDEX PREPARE_STRIDE_C
        #define PREPARE_STRIDEY PREPARE_STRIDE_G
        #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_C
        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_G

        #include UKRINCLUDE
    }
    else if (incy == 1)
    {
        #define LABELPREFIX "axpynx1y"
        #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[xstride1]")
        #define VLOADY VLOAD
        #define VSTOREY VSTORE
        #define PREPARE_STRIDEX PREPARE_STRIDE_G
        #define PREPARE_STRIDEY PREPARE_STRIDE_C
        #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_G
        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_C

        #include UKRINCLUDE
    }
    else
    {
        #define LABELPREFIX "axpynxny"
        #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[xstride1]")
        #define VLOADY(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[ystride1]")
        #define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, "%[ystride1]")
        #define PREPARE_STRIDEX PREPARE_STRIDE_G
        #define PREPARE_STRIDEY PREPARE_STRIDE_G
        #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_G
        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_G

        #include UKRINCLUDE
    }
}
