#include "bli_vlen_utils.h"

uint64_t bli_rvv_get_vlen( void )
{
    static bool env_checked = false;
    static uint64_t env_vlen = 0;

    if ( !env_checked )
    {
        env_vlen = bli_env_get_var("BLIS_RVV_OVERRIDE_VLEN", 0);
        if ( env_vlen % 8 != 0 )
        {
            bli_print_msg( "BLIS_RVV_OVERRIDE_VLEN reduced to next smallest multiple of 8.", __FILE__, __LINE__ );
            env_vlen = env_vlen & ~0x7;
        }
        env_checked = true;
    }

    uint64_t vlen = env_vlen;

    if (0 == vlen)
    {
        __asm__(
                "vsetvli %[vlen], zero, e8, m1, ta, ma\n\t"
                : [vlen] "+r" (vlen)
           );
    }
    else
    {
        __asm__(
                "vsetvli %[vlen], %[vlen], e8, m1, ta, ma\n\t"
                : [vlen] "+r" (vlen)
           );
    }

    return vlen;
}
