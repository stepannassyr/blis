/*
 * ukr1m_cdim_nvlen_case.h -- fans one NVLEN out over (inca, kappa).
 * Counterpart of kernels/rviv/1m/ukr1m_cdim_nvlen_case.h.
 *
 * The store side is always contiguous here, so only the load side and kappa
 * vary.  Unlike the RVV version there is no LMUL/UKRINCLUDE choice: SVE has
 * a single fixed vector length, so NVLEN alone selects the body.
 */

#define NVLEN_STORE_V(z, k) NVLEN_STORE_V_C(z, k)
#define NVLEN_XADV          NVLEN_XADV_I
#define NVLEN_YADV          NVLEN_YADV_I

if ( inca == 1 )
{
    #define NVLEN_LOAD_V(z, k) NVLEN_LOAD_V_C(z, k)
    #define NVLEN_PREPARE_XIDX
    #define LP_INCA "ia1"

    if ( PACKM_KAPPA_IS_1 )
    {
        #define NVLEN_XFORM_V(z)
        #define NVLEN_PREPARE_SCALAR
        #define LABELPREFIX "pk_n" STR(NVLEN) "vl_" LP_INCA "_kappa1"
        #include "ukr1m_nvlen_template.h"
        #undef LABELPREFIX
        #undef NVLEN_PREPARE_SCALAR
        #undef NVLEN_XFORM_V
    }
    else
    {
        #define NVLEN_XFORM_V(z) VFMUL_KAPPA(z)
        #define NVLEN_PREPARE_SCALAR PREPARE_SCALAR_BCAST("p0")
        #define LABELPREFIX "pk_n" STR(NVLEN) "vl_" LP_INCA "_kappag"
        #include "ukr1m_nvlen_template.h"
        #undef LABELPREFIX
        #undef NVLEN_PREPARE_SCALAR
        #undef NVLEN_XFORM_V
    }

    #undef LP_INCA
    #undef NVLEN_PREPARE_XIDX
    #undef NVLEN_LOAD_V
}
else
{
    #define NVLEN_LOAD_V(z, k) NVLEN_LOAD_V_G(z, k)
    #define NVLEN_PREPARE_XIDX PREPARE_XINDEX("inca")
    #define LP_INCA "iag"

    if ( PACKM_KAPPA_IS_1 )
    {
        #define NVLEN_XFORM_V(z)
        #define NVLEN_PREPARE_SCALAR
        #define LABELPREFIX "pk_n" STR(NVLEN) "vl_" LP_INCA "_kappa1"
        #include "ukr1m_nvlen_template.h"
        #undef LABELPREFIX
        #undef NVLEN_PREPARE_SCALAR
        #undef NVLEN_XFORM_V
    }
    else
    {
        #define NVLEN_XFORM_V(z) VFMUL_KAPPA(z)
        #define NVLEN_PREPARE_SCALAR PREPARE_SCALAR_BCAST("p0")
        #define LABELPREFIX "pk_n" STR(NVLEN) "vl_" LP_INCA "_kappag"
        #include "ukr1m_nvlen_template.h"
        #undef LABELPREFIX
        #undef NVLEN_PREPARE_SCALAR
        #undef NVLEN_XFORM_V
    }

    #undef LP_INCA
    #undef NVLEN_PREPARE_XIDX
    #undef NVLEN_LOAD_V
}

#undef NVLEN_YADV
#undef NVLEN_XADV
#undef NVLEN_STORE_V
