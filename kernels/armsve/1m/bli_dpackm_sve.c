/*
 * bli_dpackm_sve.c -- double-precision packm micro-kernel for AArch64 SVE.
 * Counterpart of kernels/rviv/1m/dpackm_rviv.c.
 */

#ifdef PACKM_SVE_STANDALONE
#include "packm_sve_standalone.h"
#else
#include "blis.h"
#endif

#define CTYPE   double
#define ESZ     ".d"
#define MSUF    "d"
#define SHIFT   "3"
#define ESIZE   "8"
#define INCE    "incd"
#define CNTE    "cntd"
#define RPFX    ""
#define GXTW    "lsl"

#define PACKM_KAPPA_IS_1  bli_deq1( *(( double* ) kappa) )

#define MAKE_REF_NAME_PASTER(base, cname) base ## cname ## _ref
#define MAKE_REF_NAME_EVAL(base, cname) MAKE_REF_NAME_PASTER(base, cname)
#define MAKE_REF_NAME(base) MAKE_REF_NAME_EVAL(base, BLIS_CNAME)

#ifndef PACKM_SVE_STANDALONE
void MAKE_REF_NAME(bli_ddpackm_)
     (
             conj_t  conja,
             pack_t  schema,
             dim_t   cdim_,
             dim_t   cdim_max,
             dim_t   cdim_bcast,
             dim_t   n_,
             dim_t   n_max_,
       const void*   kappa,
       const void*   a, inc_t inca_, inc_t lda_,
             void*   p,              inc_t ldp_,
       const void*   params,
       const cntx_t* cntx
     );
#endif

#define PACKM_REF_FALLBACK()                                    \
    MAKE_REF_NAME(bli_ddpackm_)                                 \
    (                                                           \
        conja, schema, cdim_, cdim_max, cdim_bcast,             \
        n_, n_max_, kappa, a, inca_, lda_,                      \
        p, ldp_, params, cntx                                   \
    )

#define PACKM_SET0_EDGE()                                       \
    bli_dset0s_edge                                             \
    (                                                           \
        cdim_ * cdim_bcast, cdim_max * cdim_bcast,              \
        n_, n_max_,                                             \
        p, ldp_                                                 \
    )

void bli_dpackm_sve
     (
             conj_t  conja,
             pack_t  schema,
             dim_t   cdim_,
             dim_t   cdim_max,
             dim_t   cdim_bcast,
             dim_t   n_,
             dim_t   n_max_,
       const void*   kappa,
       const void*   a, inc_t inca_, inc_t lda_,
             void*   p,              inc_t ldp_,
       const void*   params,
       const cntx_t* cntx
     )
#include "ukr1m_packm_body.h"
