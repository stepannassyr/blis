// Turns on the copy loops' source prefetch (see ukr1_macros.h): for packm
// cases whose source vectors are contiguous columns. ukr1m_pf_off.h undoes it.
#if RVIV_PACKM_PF
#undef PF_DECLARE
#undef PF_PREPARE
#undef PFX
#undef PFX_V1
#undef PFX_V2
#undef PFX_V3
#undef PF_OPERANDS
#undef PF_GROUP
#define PF_DECLARE uint64_t xpfoff; uint64_t xpfptr;
#define PF_PREPARE(colstridereg) \
    "slli %[xpfoff], " colstridereg ", " STR(RVIV_PACKM_PF_COLS_SHIFT) "\n\t"
// a register group of VLENB * LMUL bytes: one prefetch per line it spans
#if RVIV_PACKM_PF_VLENB * LMUL <= 64
#define PF_GROUP(r) PREFETCH_R(r, 0)
#elif RVIV_PACKM_PF_VLENB * LMUL <= 128
#define PF_GROUP(r) PREFETCH_R(r, 0) PREFETCH_R(r, 64)
#elif RVIV_PACKM_PF_VLENB * LMUL <= 256
#define PF_GROUP(r) PREFETCH_R(r, 0) PREFETCH_R(r, 64) PREFETCH_R(r, 128) PREFETCH_R(r, 192)
#else
#define PF_GROUP(r) PREFETCH_R(r, 0) PREFETCH_R(r, 64) PREFETCH_R(r, 128) PREFETCH_R(r, 192)\
                    PREFETCH_R(r, 256) PREFETCH_R(r, 320) PREFETCH_R(r, 384) PREFETCH_R(r, 448)
#endif
#define PFX(addrreg) \
    "add %[xpfptr], " addrreg ", %[xpfoff]\n\t" PF_GROUP("%[xpfptr]")
// the k-th group of a column starts k * VLENB * LMUL bytes in: it prefetches
// only if that is a new line (at VLENB 32 and LMUL 1: groups 0 and 2)
#if (1 * RVIV_PACKM_PF_VLENB * LMUL) % 64 == 0
#define PFX_V1(addrreg) PFX(addrreg)
#else
#define PFX_V1(addrreg)
#endif
#if (2 * RVIV_PACKM_PF_VLENB * LMUL) % 64 == 0
#define PFX_V2(addrreg) PFX(addrreg)
#else
#define PFX_V2(addrreg)
#endif
#if (3 * RVIV_PACKM_PF_VLENB * LMUL) % 64 == 0
#define PFX_V3(addrreg) PFX(addrreg)
#else
#define PFX_V3(addrreg)
#endif
#define PF_OPERANDS [xpfoff] "=&r" (xpfoff), [xpfptr] "=&r" (xpfptr),
#endif
