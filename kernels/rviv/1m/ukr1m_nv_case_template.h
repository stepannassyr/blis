

if (lda == 1)
{
#define LDA1
    if(bli_xeq1(*(( double* ) kappa)))
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
    if(bli_xeq1(*(( double* ) kappa)))
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
