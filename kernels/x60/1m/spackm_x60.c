#include "blis.h"

#include "ukr1m_inputs.h"

#include "../1/ukr1_macros.h"


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

    // vlen should be half of MR
    uint64_t vlen = bli_cntx_get_blksz_def_dt( BLIS_SINGLE, BLIS_MR, cntx )/2;

    vlen *= sizeof(float);

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
    const int64_t mr    = 2*vlen;
    const int64_t nr    = 14;

    int64_t n     = n_;
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

    #define SIZESHIFT "2"
    #define SIZEBITS  "32"
    #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0_S
    #define VTRANSFORM VFMA_F0
    #define VXTOY VFIRST
    #define VLOADY(vreg, addrreg)
    #define LDIMFIXUP(fixup)

    #define PREPARE_LDIMX(strideregvlen, ldimreg, sizeshift) PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)
    #define PREPARE_LDIMY(strideregvlen, ldimreg, sizeshift) PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)

    if ( cdim == mr && cdim_bcast == 1 )
    {

        const void* scalarptr = kappa;
        uint64_t xstride1 = inca;
        uint64_t ystride1 = 1;

        uint64_t ldimx = lda;
        uint64_t ldimy = ldp;

        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_C


        if ( bli_deq1( *(( float* )kappa) ) )
        {
            #define PREPARE_SCALAR
            #define VTRANSFORM(vdst, vsrc) 
            if ( inca == 1 )  // continous memory.
            {

                #define LABELPREFIX "pck_mr_ia1_kappa1"
                #define VLOADX VLOAD
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_C
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_C

                #include UKRINCLUDE

            }
            else  // gather load/ cont. store.
            {
                #define LABELPREFIX "pck_mr_iag_kappa1"
                #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[xstride1]")
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_G
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_G

                #include UKRINCLUDE

            }
        }
        else  // *kappa != 1.0
        {
            #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0_S
            #define VTRANSFORM VFMUL_F0

            if ( inca == 1 )  // continous memory.
            {

                #define LABELPREFIX "pck_mr_ia1_kappag"
                #define VLOADX VLOAD
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_C
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_C

                #include UKRINCLUDE
            }
            else  // gather load/ cont. store.
            {
                #define LABELPREFIX "pck_mr_iag_kappag"
                #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[xstride1]")
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_G
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_G

                #include UKRINCLUDE
            }
        } // end of if ( *kappa == 1.0 )
    }
    if ( cdim == nr && cdim_bcast == 1 && lda == 1 && ldp == nr)
    {
        uint64_t xstride1 = 1; // lda
        uint64_t ystride1 = ldp; // == nr

        uint64_t ldimx = inca;
        uint64_t ldimy = 1; // incp always 1

        #define VLOADX VLOAD
        #define VSTOREY(vreg, addrreg) VSTORE_STRIDED(vreg, addrreg, "%[ystride1]")
        #define PREPARE_STRIDEX PREPARE_STRIDE_C
        #define PREPARE_STRIDEY PREPARE_STRIDE_G
        #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_C
        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_G
        #define MAKEUNROLL MAKEUNROLL_FROMG
        #define PREPARE_LDIMX(strideregvlen, ldimreg, sizeshift) PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)
        #define PREPARE_LDIMY(strideregvlen, ldimreg, sizeshift) PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)
        #define LDIMFIXUP(fixup) fixup

        if (bli_deq1(*(( float* )kappa)))
        {
            #define LABELPREFIX "pck_nr_iag_kappa1"
            #define PREPARE_SCALAR
            #define VTRANSFORM(vdst, vsrc) 

            #include "ukr1m_14v.h"

        }
        else
        {
            #define LABELPREFIX "pck_nr_iag_kappag"
            #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0_S
            #define VTRANSFORM VFMUL_F0

            #include "ukr1m_14v.h"
        }


    }
	else
	{
        //printf("unaccelerated packing with cdim=%ld, cdim_bcast=%ld, n=%ld, inca=%ld, lda=%ld, ldp=%ld\n",
        //        cdim, cdim_bcast, n, inca, lda, ldp);
        //printf("mr=%ld, nr=%ld\n", mr, nr);
		bli_sscal2bbs_mxn
		(
		  conja,
		  cdim_,
		  n_,
		  kappa,
		  a,       inca, lda,
		  p, cdim_bcast, ldp
		);
	}

	bli_sset0s_edge
	(
	  cdim_*cdim_bcast, cdim_max*cdim_bcast,
	  n_, n_max_,
	  p, ldp
	);
}

