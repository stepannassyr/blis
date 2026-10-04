// Undoes ukr1m_pf_on.h: the copy loops' prefetch hooks are empty again.
#undef PF_DECLARE
#undef PF_PREPARE
#undef PFX
#undef PF_OPERANDS
#define PF_DECLARE
#define PF_PREPARE(colstridereg)
#define PFX(addrreg)
#define PF_OPERANDS
