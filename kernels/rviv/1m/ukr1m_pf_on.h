// Turns on the copy loops' source prefetch (see ukr1_macros.h): for packm
// cases whose source vectors are contiguous columns. ukr1m_pf_off.h undoes it.
#if RVIV_PACKM_PF
#undef PF_DECLARE
#undef PF_PREPARE
#undef PFX
#undef PF_OPERANDS
#define PF_DECLARE uint64_t xpfoff; uint64_t xpfptr;
#define PF_PREPARE(colstridereg) \
    "slli %[xpfoff], " colstridereg ", " STR(RVIV_PACKM_PF_COLS_SHIFT) "\n\t"
#define PFX(addrreg) \
    "add %[xpfptr], " addrreg ", %[xpfoff]\n\t" PF_GROUP_LINES(LMUL, "%[xpfptr]")
#define PF_OPERANDS [xpfoff] "=&r" (xpfoff), [xpfptr] "=&r" (xpfptr),
#endif
