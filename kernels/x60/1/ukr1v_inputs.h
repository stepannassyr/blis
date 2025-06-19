#include <stdint.h>

//#define UKR1_4VM2

#if defined(UKR1_4VM2)
#define UKRINCLUDE "ukr1_4vm2.h"
#define LMUL "2"
#else
#define UKRINCLUDE "ukr1_8v.h"
#define LMUL "1"
#endif

#define TAILPREPARE TAILPREPARE_VREST
#define TAILDECREMENT TAILDECREMENT_VREST
