/*
 * bli_spackm_sve.c -- single-precision packm micro-kernel for AArch64 SVE.
 * Counterpart of kernels/rviv/1m/spackm_rviv.c.
 */

#ifdef PACKM_SVE_STANDALONE
#include "packm_sve_standalone.h"
#else
#include "blis.h"
#endif

#define CTYPE   float
#define ESZ     ".s"
#define MSUF    "w"
#define SHIFT   "2"
#define ESIZE   "4"
#define INCE    "incw"
#define CNTE    "cntw"
#define RPFX    "w"
#define GXTW    "uxtw"

#define PACKM_KAPPA_IS_1  bli_seq1( *(( float* ) kappa) )

#define MAKE_REF_NAME_PASTER(base, cname) base ## cname ## _ref
#define MAKE_REF_NAME_EVAL(base, cname) MAKE_REF_NAME_PASTER(base, cname)
#define MAKE_REF_NAME(base) MAKE_REF_NAME_EVAL(base, BLIS_CNAME)

#ifndef PACKM_SVE_STANDALONE
void MAKE_REF_NAME(bli_sspackm_)
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
    MAKE_REF_NAME(bli_sspackm_)                                 \
    (                                                           \
        conja, schema, cdim_, cdim_max, cdim_bcast,             \
        n_, n_max_, kappa, a, inca_, lda_,                      \
        p, ldp_, params, cntx                                   \
    )

#define PACKM_SET0_EDGE()                                       \
    bli_sset0s_edge                                             \
    (                                                           \
        cdim_ * cdim_bcast, cdim_max * cdim_bcast,              \
        n_, n_max_,                                             \
        p, ldp_                                                 \
    )

void bli_spackm_sve
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
