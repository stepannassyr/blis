/*
 * ukr1m_nv_selector.h -- expands the transpose-path kernel for one
 * (lda, kappa) combination, and picks the store strategy at run time.
 *
 * Counterpart of kernels/rviv/1m/ukr1m_nv_selector.h.
 *
 * Caller must have defined exactly one of LDA1 / LDAG, exactly one of
 * KAPPA1 / KAPPAG, and CDIM.
 */

/* ---- load variant -------------------------------------------------- */
#if defined(LDA1)
    #define NV_LOAD_ROW(z, b, o) NV_LOAD_ROW_C(z, b, o)
    #define NV_PREPARE_XIDX
    #define LP_LDA "lda1"
#elif defined(LDAG)
    #define NV_LOAD_ROW(z, b, o) NV_LOAD_ROW_G(z, b, o)
    #define NV_PREPARE_XIDX PREPARE_XINDEX("lda")
    #define LP_LDA "ldag"
#else
    #error "ukr1m_nv_selector.h: neither LDA1 nor LDAG defined"
#endif

/* ---- kappa variant ------------------------------------------------- */
#if defined(KAPPA1)
    #define NV_XFORM_ROW(z)
    #define NV_PREPARE_SCALAR
    #define LP_KAP "kappa1"
#elif defined(KAPPAG)
    #define NV_XFORM_ROW(z) VFMUL_KAPPA(z)
    #define NV_PREPARE_SCALAR PREPARE_SCALAR_BCAST("p7")
    #define LP_KAP "kappag"
#else
    #error "ukr1m_nv_selector.h: neither KAPPA1 nor KAPPAG defined"
#endif

/* ---- store variant -------------------------------------------------
 * ldp == cdim is the common case (full micropanel, cdim_bcast == 1): the
 * packed block for a VL-chunk of n is then cdim*VL contiguous elements, so
 * the transpose can be done in registers and written with ST{2,3,4}x
 * instead of cdim scatters.
 */
#if NV_HAS_ILV(CDIM)
if ( ldp == CDIM )
{
    #define NV_STOREBLOCK   NV_STORE_ILV(CDIM)
    #define NV_PREPARE_YIDX
    #define LABELPREFIX     "pk_c" STR(CDIM) "_" LP_LDA "_" LP_KAP "_ilv"

    #include "ukr1m_nv_template.h"

    #undef LABELPREFIX
    #undef NV_PREPARE_YIDX
    #undef NV_STOREBLOCK
}
else
#endif
{
    #define NV_STOREBLOCK   NV_STORE_SCT(CDIM)
    #define NV_PREPARE_YIDX PREPARE_YINDEX("ldp")
    #define LABELPREFIX     "pk_c" STR(CDIM) "_" LP_LDA "_" LP_KAP "_sct"

    #include "ukr1m_nv_template.h"

    #undef LABELPREFIX
    #undef NV_PREPARE_YIDX
    #undef NV_STOREBLOCK
}

#undef LP_KAP
#undef NV_PREPARE_SCALAR
#undef NV_XFORM_ROW
#undef LP_LDA
#undef NV_PREPARE_XIDX
#undef NV_LOAD_ROW
