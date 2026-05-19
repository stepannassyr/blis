#include "blis.h"

#include "ukr1v_inputs.h"

#include "ukr1_macros.h"

#define LMUL 2
#define UKRINCLUDE "../1/ukr1_4u1vmx.h"

void bli_dcopyv_rviv(
             conj_t  conjx,
             dim_t   n,
       const void*  x, inc_t incx_,
             void*  y, inc_t incy_,
       const cntx_t* cntx)
{
    int64_t incx = incx_;
    int64_t incy = incy_;

    uint64_t vlen = bli_rvv_get_vlen();

    // override vlen
    __asm__(
            "vsetvli %[vlen], %[vlen], e8, m1, ta, ma\n\t"
            : [vlen] "+r" (vlen)
            :
            :
       );

    vlen = vlen/sizeof(double);

    const void* scalarptr = NULL;
    uint64_t xstride1 = incx;
    uint64_t ystride1 = incy;
    uint64_t ldimx = 1;
    uint64_t ldimy = 1;

    #define MAKEUNROLL MAKEUNROLL_FROMG

    #define SIZESHIFT "3"
    #define SIZEBITS  "64"
    #define PREPARE_SCALAR
    #define VTRANSFORM(vdst, vsrc) 
    #define VXTOY VFIRST
    #define VLOADY(vreg, addrreg)
    #define LDIMFIXUP(fixup) fixup

    #define PREPARE_LDIMX(strideregvlen, ldimreg, sizeshift)
    #define PREPARE_LDIMY(strideregvlen, ldimreg, sizeshift) 

   
    if ((incx == 1) && (incy == 1))
    {
        #define LABELPREFIX "copy1x1y"
        #define VLOADX VLOAD
        #define VSTOREY VSTORE
        #define PREPARE_STRIDEX PREPARE_STRIDE_C
        #define PREPARE_STRIDEY PREPARE_STRIDE_C
        #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_C
        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_C

        #include UKRINCLUDE

        #undef LABELPREFIX
        #undef VLOADX
        #undef VSTOREY
        #undef PREPARE_STRIDEX
        #undef PREPARE_STRIDEY
        #undef VSTRIDE_FROM_1STRIDE_X
        #undef VSTRIDE_FROM_1STRIDE_Y
    }
    else if (incx == 1)
    {
        #define LABELPREFIX "copy1xny"
        #define VLOADX VLOAD
        #define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, "%[ystride1]")
        #define PREPARE_STRIDEX PREPARE_STRIDE_C
        #define PREPARE_STRIDEY PREPARE_STRIDE_G
        #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_C
        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_G

        #include UKRINCLUDE

        #undef LABELPREFIX
        #undef VLOADX
        #undef VSTOREY
        #undef PREPARE_STRIDEX
        #undef PREPARE_STRIDEY
        #undef VSTRIDE_FROM_1STRIDE_X
        #undef VSTRIDE_FROM_1STRIDE_Y
    }
    else if (incy == 1)
    {
        #define LABELPREFIX "copynx1y"
        #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[xstride1]")
        #define VSTOREY VSTORE
        #define PREPARE_STRIDEX PREPARE_STRIDE_G
        #define PREPARE_STRIDEY PREPARE_STRIDE_C
        #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_G
        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_C

        #include UKRINCLUDE

        #undef LABELPREFIX
        #undef VLOADX
        #undef VSTOREY
        #undef PREPARE_STRIDEX
        #undef PREPARE_STRIDEY
        #undef VSTRIDE_FROM_1STRIDE_X
        #undef VSTRIDE_FROM_1STRIDE_Y
    }
    else
    {
        #define LABELPREFIX "copynxny"
        #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[xstride1]")
        #define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, "%[ystride1]")
        #define PREPARE_STRIDEX PREPARE_STRIDE_G
        #define PREPARE_STRIDEY PREPARE_STRIDE_G
        #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_G
        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_G

        #include UKRINCLUDE

        #undef LABELPREFIX
        #undef VLOADX
        #undef VSTOREY
        #undef PREPARE_STRIDEX
        #undef PREPARE_STRIDEY
        #undef VSTRIDE_FROM_1STRIDE_X
        #undef VSTRIDE_FROM_1STRIDE_Y
    }
}
