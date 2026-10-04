// Undoes ukr1m_pf_on.h: the copy loops' prefetch hooks are empty again.
#undef PF_DECLARE
#undef PF_PREPARE
#undef PFX
#undef PFX_V1
#undef PFX_V2
#undef PFX_V3
#undef PF_OPERANDS
#undef PF_GROUP
#define PF_DECLARE
#define PF_PREPARE(colstridereg)
#define PFX(addrreg)
#define PFX_V1(addrreg)
#define PFX_V2(addrreg)
#define PFX_V3(addrreg)
#define PF_OPERANDS
