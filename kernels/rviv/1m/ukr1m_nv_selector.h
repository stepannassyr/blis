
#define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, "%[ystride1]")
#define PREPARE_STRIDEY PREPARE_STRIDE_G
#define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_G
#define MAKEUNROLL MAKEUNROLL_FROMG
#define PREPARE_LDIMX(strideregvlen, ldimreg, sizeshift) PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)
#define PREPARE_LDIMY(strideregvlen, ldimreg, sizeshift) PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)
#define LDIMFIXUP(fixup) fixup

#if defined(LDA1)

#define VLOADX VLOAD
#define PREPARE_STRIDEX PREPARE_STRIDE_C
#define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_C
#define CALC_VOFFSET "slli %[yfinoff], %[vlen], " SIZESHIFT "\n\t"

#if defined(KAPPA1)
    #define LABELPREFIX "pck_cdim" STR(CDIM) "_lda1_kappa1"
    #define PREPARE_SCALAR
    #define VTRANSFORM(vdst, vsrc)

    #include "ukr1m_nv_template.h"
    #undef LABELPREFIX
    #undef PREPARE_SCALAR
    #undef VTRANSFORM
#elif defined (KAPPAG)
    #define LABELPREFIX "pck_cdim" STR(CDIM) "_lda1_kappag"
    #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0(DT_SUFFIX)
    #define VTRANSFORM VFMUL_F0

    #include "ukr1m_nv_template.h"
    #undef LABELPREFIX
    #undef PREPARE_SCALAR
    #undef VTRANSFORM
#else
#error incorrect macro usage
#endif

#undef VLOADX
#undef PREPARE_STRIDEX
#undef VSTRIDE_FROM_1STRIDE_X
#undef CALC_VOFFSET

#elif defined(LDAG)

#define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[xstride1]")
#define PREPARE_STRIDEX PREPARE_STRIDE_G
#define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_G
#define CALC_VOFFSET "mul %[yfinoff], %[vlen], %[xstride1]\n\t"

#if defined(KAPPA1)
    #define LABELPREFIX "pck_cdim" STR(CDIM) "_ldag_kappa1"
    #define PREPARE_SCALAR
    #define VTRANSFORM(vdst, vsrc)

    #include "ukr1m_nv_template.h"
    #undef LABELPREFIX
    #undef PREPARE_SCALAR
    #undef VTRANSFORM
#elif defined (KAPPAG)
    #define LABELPREFIX "pck_cdim" STR(CDIM) "_ldag_kappag"
    #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0(DT_SUFFIX)
    #define VTRANSFORM VFMUL_F0

    #include "ukr1m_nv_template.h"
    #undef LABELPREFIX
    #undef PREPARE_SCALAR
    #undef VTRANSFORM
#else
#error incorrect macro usage
#endif

#undef VLOADX
#undef PREPARE_STRIDEX
#undef VSTRIDE_FROM_1STRIDE_X
#undef CALC_VOFFSET

#else
#error incorrect macro usage
#endif

#undef VSTOREY
#undef PREPARE_STRIDEY
#undef VSTRIDE_FROM_1STRIDE_Y
#undef MAKEUNROLL
#undef PREPARE_LDIMX
#undef PREPARE_LDIMY
#undef LDIMFIXUP
