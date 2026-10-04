
#define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, "%[ystride1]")
#define PREPARE_STRIDEY PREPARE_STRIDE_G
#define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_G
#define MAKEUNROLL MAKEUNROLL_FROMG
#define PREPARE_LDIMX(strideregvlen, ldimreg, sizeshift) PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)
#define PREPARE_LDIMY(strideregvlen, ldimreg, sizeshift) PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)
#define LDIMFIXUP(fixup) fixup

#if defined(LDA1)

#define VLOADX VLOAD
// each source column is contiguous along k: prefetch it RVIV_PACKM_PF_BYTES
// ahead (ukr1_macros.h), once per line. A block reads VLENB bytes of each
// column, so below 64 the offset alternates every block between that and -64,
// the column's previous line -- loaded already, in L1, a hint that costs
// nothing -- rather than prefetch one line twice, which costs the X60 the
// whole benefit. The alternation hits each line once whatever the alignment.
// With smaller registers (VLEN 128) a line spans four blocks and alternating
// would still repeat lines: no prefetch there.
#if RVIV_PACKM_PF && RVIV_PACKM_PF_VLENB >= 32
#define PFV(addrreg) "add %[pft], " addrreg ", %[unroll]\n\t" PREFETCH_R("%[pft]", 0)
#define PFV_INIT "li %[unroll], " STR(RVIV_PACKM_PF_BYTES) "\n\t"
#if RVIV_PACKM_PF_VLENB < 64
#define PFV_TOGGLE "xori %[unroll], %[unroll], " STR(((RVIV_PACKM_PF_BYTES) ^ -64)) "\n\t"
#else
#define PFV_TOGGLE
#endif
#else
#define PFV(addrreg)
#define PFV_INIT
#define PFV_TOGGLE
#endif
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
#undef PFV
#undef PFV_INIT
#undef PFV_TOGGLE
#undef PREPARE_STRIDEX
#undef VSTRIDE_FROM_1STRIDE_X
#undef CALC_VOFFSET

#elif defined(LDAG)

#define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[xstride1]")
// strided along k: a line ahead would cover one element's line, so none
#define PFV(addrreg)
#define PFV_INIT
#define PFV_TOGGLE
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
#undef PFV
#undef PFV_INIT
#undef PFV_TOGGLE
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
