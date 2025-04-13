#include "blis.h"

#include "ukr1m_inputs.h"

#include "../1/ukr1_macros.h"


void bli_dpackm_x60
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
    uint64_t vlen = bli_cntx_get_blksz_def_dt( BLIS_DOUBLE, BLIS_MR, cntx )/2;

    vlen *= sizeof(double);

    // override vlen
    __asm__(
            //"csrr %[vlen],vlenb\n\t"
            "vsetvli %[vlen], %[vlen], e8, m1\n\t"
            : [vlen] "+r" (vlen)
            :
            :
       );

    vlen = vlen/sizeof(double);

    const int64_t cdim  = cdim_;
    const int64_t mr    = 2*vlen;
    const int64_t nr    = 14;

    const int64_t n     = n_;
    const int64_t inca  = inca_;
    const int64_t lda   = lda_;
    const int64_t ldp   = ldp_;


    // output is y in the generic kernel
    void* y = p;

    #define SIZESHIFT "3"
    #define SIZEBITS  "64"
    //#define PREPARE_SCALAR PREPARE_SCALAR_LOADF0
    //#define VTRANSFORM VFMA_F0
    #define VLOADY(vreg, addrreg)
    #define VXTOY VFIRST
    #define LDIMFIXUP(fixup)

    if ( cdim == mr && cdim_bcast == 1 )
    {
        uint64_t unroll = 4;
        uint64_t niter = n/unroll;
        uint64_t nleft = n % unroll;

        volatile ukrinputs_t ukrinputs;
        ukrinputs.n = n;
        ukrinputs.niter = niter;
        ukrinputs.nleft = nleft;
        ukrinputs.inca = inca;
        ukrinputs.incp = 1;
        ukrinputs.lda = lda;
        ukrinputs.ldp = ldp;
        ukrinputs.kappa = kappa;
        ukrinputs.a = a;
        ukrinputs.p = p;

        #ifdef UKR1_4VM2
        ukrinputs.vlen = 2*vlen;
        #else
        ukrinputs.vlen = vlen;
        #endif

        if ( bli_deq1( *(( double* )kappa) ) )
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
                #define PREPARE_LDIMX(ldimreg, offset, sizeshift) PREPARE_LDIM_NON1(ldimreg, offset, sizeshift)
                #define PREPARE_LDIMY(ldimreg, offset, sizeshift) PREPARE_LDIM_NON1(ldimreg, offset, sizeshift)

                #include UKRINCLUDE

            }
            else  // gather load/ cont. store.
            {
                #define LABELPREFIX "pck_mr_iag_kappa1"
                #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, STRIDE1_X)
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_G
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define PREPARE_LDIMX(ldimreg, offset, sizeshift) PREPARE_LDIM_NON1(ldimreg, offset, sizeshift)
                #define PREPARE_LDIMY(ldimreg, offset, sizeshift) PREPARE_LDIM_NON1(ldimreg, offset, sizeshift)

                #include UKRINCLUDE

            }
        }
        else  // *kappa != 1.0
        {
            #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0
            #define VTRANSFORM VFMUL_F0

            if ( inca == 1 )  // continous memory.
            {

                #define LABELPREFIX "pck_mr_ia1_kappag"
                #define VLOADX VLOAD
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_C
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define PREPARE_LDIMX(ldimreg, offset, sizeshift) PREPARE_LDIM_NON1(ldimreg, offset, sizeshift)
                #define PREPARE_LDIMY(ldimreg, offset, sizeshift) PREPARE_LDIM_NON1(ldimreg, offset, sizeshift)

                #include UKRINCLUDE
            }
            else  // gather load/ cont. store.
            {
                #define LABELPREFIX "pck_mr_iag_kappag"
                #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, STRIDE1_X)
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_G
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define PREPARE_LDIMX(ldimreg, offset, sizeshift) PREPARE_LDIM_NON1(ldimreg, offset, sizeshift)
                #define PREPARE_LDIMY(ldimreg, offset, sizeshift) PREPARE_LDIM_NON1(ldimreg, offset, sizeshift)

                #include UKRINCLUDE
            }
        } // end of if ( *kappa == 1.0 )
    }
	else
	{
        //printf("unaccelerated packing with cdim=%ld, cdim_bcast=%ld, n=%ld, inca=%ld, lda=%ld, ldp=%ld\n",
        //        cdim, cdim_bcast, n, inca, lda, ldp);
        //printf("mr=%ld, nr=%ld\n", mr, nr);
		bli_dscal2bbs_mxn
		(
		  conja,
		  cdim_,
		  n_,
		  kappa,
		  a,       inca, lda,
		  p, cdim_bcast, ldp
		);
	}

	bli_dset0s_edge
	(
	  cdim_*cdim_bcast, cdim_max*cdim_bcast,
	  n_, n_max_,
	  p, ldp
	);
}

