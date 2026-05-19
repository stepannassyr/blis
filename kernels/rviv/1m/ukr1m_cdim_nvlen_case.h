
        const void* scalarptr = kappa;
        uint64_t xstride1 = inca;
        uint64_t ystride1 = 1;

        uint64_t ldimx = lda;
        uint64_t ldimy = ldp;

        #define VSTRIDE_FROM_1STRIDE_Y VSTRIDE_FROM_1STRIDE_C


        if ( bli_xeq1( *(( float* )kappa) ) )
        {
            #define PREPARE_SCALAR
            #define VTRANSFORM(vdst, vsrc) 
            if ( inca == 1 )  // continous memory.
            {

                #define LABELPREFIX "pck_cdim_" STR(NVLEN) "vlen_ia1_kappa1"
                #define VLOADX VLOAD
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_C
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_C

                #include UKRINCLUDE

                #undef LABELPREFIX
                #undef VLOADX
                #undef VSTOREY
                #undef PREPARE_STRIDEX
                #undef PREPARE_STRIDEY
                #undef VSTRIDE_FROM_1STRIDE_X

            }
            else  // gather load/ cont. store.
            {
                #define LABELPREFIX "pck_cdim_" STR(NVLEN) "vlen_iag_kappa1"
                #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[xstride1]")
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_G
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_G

                #include UKRINCLUDE

                #undef LABELPREFIX
                #undef VLOADX
                #undef VSTOREY
                #undef PREPARE_STRIDEX
                #undef PREPARE_STRIDEY
                #undef VSTRIDE_FROM_1STRIDE_X

            }
            #undef PREPARE_SCALAR
            #undef VTRANSFORM
        }
        else  // *kappa != 1.0
        {
            #define PREPARE_SCALAR PREPARE_SCALAR_LOADF0(DT_SUFFIX)
            #define VTRANSFORM VFMUL_F0

            if ( inca == 1 )  // continous memory.
            {

                #define LABELPREFIX "pck_cdim_" STR(NVLEN) "vlen_ia1_kappag"
                #define VLOADX VLOAD
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_C
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_C

                #include UKRINCLUDE

                #undef LABELPREFIX
                #undef VLOADX
                #undef VSTOREY
                #undef PREPARE_STRIDEX
                #undef PREPARE_STRIDEY
                #undef VSTRIDE_FROM_1STRIDE_X
            }
            else  // gather load/ cont. store.
            {
                #define LABELPREFIX "pck_cdim_" STR(NVLEN) "vlen_iag_kappag"
                #define VLOADX(vreg, addrreg) VLOAD_STRIDED(vreg, addrreg, "%[xstride1]")
                #define VSTOREY VSTORE
                #define PREPARE_STRIDEX PREPARE_STRIDE_G
                #define PREPARE_STRIDEY PREPARE_STRIDE_C
                #define VSTRIDE_FROM_1STRIDE_X VSTRIDE_FROM_1STRIDE_G

                #include UKRINCLUDE

                #undef LABELPREFIX
                #undef VLOADX
                #undef VSTOREY
                #undef PREPARE_STRIDEX
                #undef PREPARE_STRIDEY
                #undef VSTRIDE_FROM_1STRIDE_X
            }
            #undef PREPARE_SCALAR
            #undef VTRANSFORM
        } // end of if ( *kappa == 1.0 )

        #undef VSTRIDE_FROM_1STRIDE_Y
