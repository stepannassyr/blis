#include <stdint.h>

// 1v inputs struct
typedef struct
{
    uint64_t n;         // 0
    uint64_t niter;     // 8
    uint64_t nleft;     // 16
    uint64_t vlen;      // 24
    int64_t incx;       // 32
    int64_t incy;       // 40
    const void* scalar; // 48
    const void* x;      // 56
          void* y;      // 64
    
} ukrinputs_t;

#define I_N      "0"
#define I_NITER  "8"
#define I_NLEFT  "16"
#define I_VLEN   "24"
#define I_INCX   "32"
#define I_INCY   "40"
#define I_SCALAR "48"
#define I_X      "56"
#define I_Y      "64"
#define I_LDIMX  "INVALID"
#define I_LDIMY  "INVALID"

#define STRIDE1_X "s5"
#define STRIDE1_Y "s4"


//#define UKR1_4VM2

#if defined(UKR1_4VM2)
#define UKRINCLUDE "ukr1_4vm2.h"
#else
#define UKRINCLUDE "ukr1_8v.h"
#endif

#define TAILPREPARE TAILPREPARE_VREST
#define TAILDECREMENT TAILDECREMENT_VREST
