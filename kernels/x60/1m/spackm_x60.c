#include "blis.h"

#include "ukr1m_inputs.h"

#include "../1/ukr1_macros.h"

void bli_sspackm_x60_ref
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

void bli_spackm_x60
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
{

    uint64_t vlen = bli_rvv_get_vlen();

    // override vlen
    __asm__(
            //"csrr %[vlen],vlenb\n\t"
            "vsetvli %[vlen], %[vlen], e8, m1, ta, ma\n\t"
            : [vlen] "+r" (vlen)
            :
            :
       );

    vlen = vlen/sizeof(float);

    const int64_t cdim  = cdim_;

    int64_t n     = n_max_;
    const int64_t inca  = inca_;
    const int64_t lda   = lda_;
    const int64_t ldp   = ldp_;


    // output is y in the generic kernel
    const void* x = a;
    void* y = p;

    const void* scalarptr = kappa;

    //printf("packing with cdim=%ld, cdim_bcast=%ld, n=%ld, inca=%ld, lda=%ld, ldp=%ld\n",
    //        cdim, cdim_bcast, n, inca, lda, ldp);

    #define MAKEUNROLL MAKEUNROLL_I

    #define bli_xeq1 bli_seq1
    #define DT_SUFFIX S
    #define SIZESHIFT "2"
    #define SIZEBITS  "32"
    #define VXTOY VFIRST
    #define VLOADY(vreg, addrreg)
    #define LDIMFIXUP(fixup)

    #define PREPARE_LDIMX(strideregvlen, ldimreg, sizeshift) PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)
    #define PREPARE_LDIMY(strideregvlen, ldimreg, sizeshift) PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)

    if ( cdim == vlen && cdim_bcast == 1 )
    {
#define NVLEN 1
        #define LMUL 1
        #define UKRINCLUDE "../1/ukr1_4u1vmx.h"

        #include "ukr1m_cdim_nvlen_case.h"

        #undef UKRINCLUDE
        #undef LMUL
#undef NVLEN
    }
    else if ( cdim == 2*vlen && cdim_bcast == 1 )
    {
#define NVLEN 2
#if 0
        #define LMUL 1
        #define UKRINCLUDE "../1/ukr1_4u2vmx.h"
#else
        #define LMUL 2
        #define UKRINCLUDE "../1/ukr1_4u1vmx.h"
#endif
        #include "ukr1m_cdim_nvlen_case.h"
        #undef UKRINCLUDE
        #undef LMUL
#undef NVLEN
    }
#define LMUL 1
    else if ( cdim == 3*vlen && cdim_bcast == 1 )
    {
#define NVLEN 3
        #define LMUL 1
        #define UKRINCLUDE "../1/ukr1_4u3vmx.h"
        #include "ukr1m_cdim_nvlen_case.h"
        #undef UKRINCLUDE
        #undef LMUL
#undef NVLEN
    }
    else if ( cdim == 4*vlen && cdim_bcast == 1 )
    {
#define NVLEN 4
#if 1
        #define LMUL 1
        #define UKRINCLUDE "../1/ukr1_4u4vmx.h"
#elif 0
        #define LMUL 2
        #define UKRINCLUDE "../1/ukr1_4u2vmx.h"
#else
        #define LMUL 4
        #define UKRINCLUDE "../1/ukr1_4u1vmx.h"
#endif
        #include "ukr1m_cdim_nvlen_case.h"
        #undef UKRINCLUDE
        #undef LMUL
#undef NVLEN
    }
#define LMUL 1
#define PRELOAD_DIST 1
#undef MAKEUNROLL
#undef LDIMFIXUP
    else if (cdim_bcast == 1 && cdim <= 16)
    {
        //printf("transpose-vectorized packing with cdim=%ld, cdim_bcast=%ld, n=%ld, inca=%ld, lda=%ld, ldp=%ld\n",
        //        cdim, cdim_bcast, n, inca, lda, ldp);
        uint64_t xstride1 = lda; // lda
        uint64_t ystride1 = ldp; // == nr

        uint64_t ldimx = inca;
        uint64_t ldimy = 1; // incp always 1
        if (cdim == 1)
        {
            #define CDIM 1
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
#undef PRELOAD_DIST
#define PRELOAD_DIST 2
        else if (cdim == 2)
        {
            #define CDIM 2
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
#undef PRELOAD_DIST
#define PRELOAD_DIST 3
        else if (cdim == 3)
        {
            #define CDIM 3
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
#undef PRELOAD_DIST
#define PRELOAD_DIST 4
        else if (cdim == 4)
        {
            #define CDIM 4
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
#undef PRELOAD_DIST
#define PRELOAD_DIST 5
        else if (cdim == 5)
        {
            #define CDIM 5
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
#undef PRELOAD_DIST
#define PRELOAD_DIST 6
        else if (cdim == 6)
        {
            #define CDIM 6
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
#undef PRELOAD_DIST
#define PRELOAD_DIST 7
        else if (cdim == 7)
        {
            #define CDIM 7
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
#undef PRELOAD_DIST
#define PRELOAD_DIST 8
        else if (cdim == 8)
        {
            #define CDIM 8
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if (cdim == 9)
        {
            #define CDIM 9
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if (cdim == 10)
        {
            #define CDIM 10
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if (cdim == 11)
        {
            #define CDIM 11
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if (cdim == 12)
        {
            #define CDIM 12
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if (cdim == 13)
        {
            #define CDIM 13
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if (cdim == 14)
        {
            #define CDIM 14
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if (cdim == 15)
        {
            #define CDIM 15
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
        else if (cdim == 16)
        {
            #define CDIM 16
            #include "ukr1m_nv_case_template.h"
            #undef CDIM
        }
    }
    #undef PRELOAD_DIST

	else
	{
        //printf("unaccelerated packing with cdim=%ld, cdim_bcast=%ld, n=%ld, inca=%ld, lda=%ld, ldp=%ld\n",
        //        cdim, cdim_bcast, n, inca, lda, ldp);
        //printf("mr=%ld, nr=%ld\n", mr, nr);
		//bli_sscal2bbs_mxn
		//(
		//  conja,
		//  cdim_,
		//  n_,
		//  kappa,
		//  a,       inca, lda,
		//  p, cdim_bcast, ldp
		//);
        bli_sspackm_x60_ref
        (
            conja, schema, cdim_, cdim_max, cdim_bcast,
            n_, n_max_, kappa, a, inca_, lda_,
            p, ldp_, params, cntx
        );
	}

	bli_sset0s_edge
	(
	  cdim_*cdim_bcast, cdim_max*cdim_bcast,
	  n_, n_max_,
	  p, ldp
	);
}

