/*
 * ukr1m_nv_case_template.h -- fans one CDIM out over (lda, kappa).
 * Counterpart of kernels/rviv/1m/ukr1m_nv_case_template.h.
 */

if ( lda == 1 )
{
#define LDA1
    if ( PACKM_KAPPA_IS_1 )
    {
        #define KAPPA1
        #include "ukr1m_nv_selector.h"
        #undef KAPPA1
    }
    else
    {
        #define KAPPAG
        #include "ukr1m_nv_selector.h"
        #undef KAPPAG
    }
#undef LDA1
}
else
{
#define LDAG
    if ( PACKM_KAPPA_IS_1 )
    {
        #define KAPPA1
        #include "ukr1m_nv_selector.h"
        #undef KAPPA1
    }
    else
    {
        #define KAPPAG
        #include "ukr1m_nv_selector.h"
        #undef KAPPAG
    }
#undef LDAG
}
