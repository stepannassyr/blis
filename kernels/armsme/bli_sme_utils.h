#include <stdint.h>

static inline uint64_t bli_sme_svl_bytes(void)
{
    uint64_t svl;
    __asm__ ("rdsvl %[svl], #1"
            : [svl] "=r" (svl)
            :
            : );
    return svl;
}


