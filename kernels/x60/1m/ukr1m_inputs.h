#include <stdint.h>

// 1m inputs struct
typedef struct
{
    uint64_t n;        // 0
    uint64_t niter;    // 8
    uint64_t nleft;    // 16
    uint64_t vlen;     // 24
    int64_t inca;      // 32
    int64_t incp;      // 40
    int64_t lda;       // 48
    int64_t ldp;       // 56
    const void* kappa; // 64
    const void* a;     // 72
          void* p;     // 80
} ukrinputs_t;

#define I_N      "0"
#define I_NITER  "8"
#define I_NLEFT  "16"
#define I_VLEN   "24"
#define I_INCX   "32"
#define I_INCY   "40"
#define I_LDIMX  "48"
#define I_LDIMY  "56"
#define I_SCALAR "64"
#define I_X      "72"
#define I_Y      "80"

#define STRIDE1_X "s5"
#define STRIDE1_Y "s4"


#define UKR1_4VM2

#if defined(UKR1_4VM2)
#define UKRINCLUDE "../1/ukr1_4vm2.h"
#else
#define UKRINCLUDE "../1/ukr1_8v.h"
#endif

#define TAILPREPARE TAILPREPARE_WHOLEV
#define TAILDECREMENT TAILDECREMENT_WHOLEV
