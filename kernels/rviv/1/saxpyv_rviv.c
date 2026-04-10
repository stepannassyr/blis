#include "blis.h"

#include "ukr1v_inputs.h"

#include "ukr1_macros.h"

#define LMUL 2
#define UKRINCLUDE "../1/ukr1_4u1vmx.h"

void bli_saxpyv_rviv(
             conj_t  conjx,
             dim_t   n,
       const void*  alpha,
       const void*  x, inc_t incx_,
             void*  y, inc_t incy_,
       const cntx_t* cntx)
{
    if (bli_seq0(* ((float*)alpha)))
    {
        return;
    }

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

    vlen = vlen/sizeof(float);

    const void* scalarptr = alpha;
    uint64_t xstride1 = incx;
    uint64_t ystride1 = incy;
    uint64_t ldimx = 1;
    uint64_t ldimy = 1;

    #define MAKEUNROLL MAKEUNROLL_FROMG

    #define SIZESHIFT "2"
    #define SIZEBITS  "32"
    #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0_S
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

        #undef LABELPREFIX
        #undef VLOADX
        #undef VLOADY
        #undef VSTOREY
        #undef PREPARE_STRIDEX
        #undef PREPARE_STRIDEY
        #undef VSTRIDE_FROM_1STRIDE_X
        #undef VSTRIDE_FROM_1STRIDE_Y
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

        #undef LABELPREFIX
        #undef VLOADX
        #undef VLOADY
        #undef VSTOREY
        #undef PREPARE_STRIDEX
        #undef PREPARE_STRIDEY
        #undef VSTRIDE_FROM_1STRIDE_X
        #undef VSTRIDE_FROM_1STRIDE_Y
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

        #undef LABELPREFIX
        #undef VLOADX
        #undef VLOADY
        #undef VSTOREY
        #undef PREPARE_STRIDEX
        #undef PREPARE_STRIDEY
        #undef VSTRIDE_FROM_1STRIDE_X
        #undef VSTRIDE_FROM_1STRIDE_Y
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

        #undef LABELPREFIX
        #undef VLOADX
        #undef VLOADY
        #undef VSTOREY
        #undef PREPARE_STRIDEX
        #undef PREPARE_STRIDEY
        #undef VSTRIDE_FROM_1STRIDE_X
        #undef VSTRIDE_FROM_1STRIDE_Y
    }
}
