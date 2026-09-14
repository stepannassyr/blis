/*
 * ukr1m_packm_body.h -- shared dispatch tree for the SVE packm kernels.
 *
 * The RVV port carries two near-identical 298-line files (dpackm_rviv.c and
 * spackm_rviv.c).  Everything that differs between them is a handful of
 * tokens, so here the tree lives once and the .c files only set the
 * datatype macros.
 *
 * Required from the includer:
 *   CTYPE                 float | double
 *   ESZ, MSUF, SHIFT, ESIZE, INCE, CNTE     (see 1/ukr1_macros.h)
 *   PACKM_KAPPA_IS_1      test on *kappa
 *   PACKM_REF_FALLBACK()  reference packm for cases we do not vectorise
 *   PACKM_SET0_EDGE()     zero the cdim/n padding of the packed panel
 */

#include <stdint.h>

#include "../1/ukr1_macros.h"
#include "ukr1m_nv_blocks.h"

#ifndef NV_CDIM_MAX
#define NV_CDIM_MAX 16
#endif

{
    uint64_t vlen;
    __asm__ ( CNTE " %0" : "=r" (vlen) );      /* elements per vector */

    const uint64_t cdim = (uint64_t) cdim_;
    const uint64_t n    = (uint64_t) n_;
    const uint64_t inca = (uint64_t) inca_;
    const uint64_t lda  = (uint64_t) lda_;
    const uint64_t ldp  = (uint64_t) ldp_;

    const void*  x = a;                        /* input  (x in the ukrs)  */
    void*        y = p;                        /* output (y in the ukrs)  */
    const void*  scalarptr = kappa;

    /* ---------------------------------------------------------------- *
     * cdim == NVLEN * VL : vector runs along cdim, stores contiguous    *
     * ---------------------------------------------------------------- */
    if ( cdim_bcast == 1 && cdim == 1 * vlen )
    {
        #define NVLEN 1
        #include "ukr1m_cdim_nvlen_case.h"
        #undef NVLEN
    }
    else if ( cdim_bcast == 1 && cdim == 2 * vlen )
    {
        #define NVLEN 2
        #include "ukr1m_cdim_nvlen_case.h"
        #undef NVLEN
    }
    else if ( cdim_bcast == 1 && cdim == 3 * vlen )
    {
        #define NVLEN 3
        #include "ukr1m_cdim_nvlen_case.h"
        #undef NVLEN
    }
    else if ( cdim_bcast == 1 && cdim == 4 * vlen )
    {
        #define NVLEN 4
        #include "ukr1m_cdim_nvlen_case.h"
        #undef NVLEN
    }

    /* ---------------------------------------------------------------- *
     * cdim <= 16 : vector runs along n, output transposed in-register   *
     * ---------------------------------------------------------------- */
    else if ( cdim_bcast == 1 && cdim >= 1 && cdim <= NV_CDIM_MAX )
    {
        if ( cdim == 1 )
        {
            #define CDIM 1
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 2 )
        {
            #define CDIM 2
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 3 )
        {
            #define CDIM 3
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 4 )
        {
            #define CDIM 4
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 5 )
        {
            #define CDIM 5
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 6 )
        {
            #define CDIM 6
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 7 )
        {
            #define CDIM 7
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 8 )
        {
            #define CDIM 8
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 9 )
        {
            #define CDIM 9
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 10 )
        {
            #define CDIM 10
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 11 )
        {
            #define CDIM 11
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 12 )
        {
            #define CDIM 12
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 13 )
        {
            #define CDIM 13
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 14 )
        {
            #define CDIM 14
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if ( cdim == 15 )
        {
            #define CDIM 15
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else /* cdim == 16 */
        {
            #define CDIM 16
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
    }
    else
    {
        PACKM_REF_FALLBACK();
    }

    (void) x; (void) y; (void) scalarptr; (void) vlen;

    PACKM_SET0_EDGE();
}
