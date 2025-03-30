/* base inputs struct
typedef struct
{
    uint64_t n;        // 0
    uint64_t niter;    // 8
    uint64_t nleft;    // 16
    uint64_t vlen;     // 24
    int64_t incx;      // 32
    int64_t incy;      // 40
    const void* scalar;// 48 (alpha...)
    const void* x;     // 56
          void* y;     // 64
} ukrinputs_t; */

#define I_N      "0"
#define I_NITER  "8"
#define I_NLEFT  "16"
#define I_VLEN   "24"
#define I_INCX   "32"
#define I_INCY   "40"
#define I_SCALAR "48"
#define I_X      "56"
#define I_Y      "64"

#define STRIDE1_X "s5"
#define STRIDE1_Y "s4"

#define VLOAD_STRIDED(vreg, addrreg, stride1reg)\
    "vlse64.v " vreg ", (" addrreg "), " stride1reg "\n\t"

#define VSTORE_STRIDED(vreg, addrreg, stride1reg)\
    "vsse64.v " vreg ", (" addrreg "), " stride1reg "\n\t"

#define VLOAD(vreg, addrreg)\
    "vle64.v " vreg ", (" addrreg ")\n\t"

#define VSTORE(vreg, addrreg)\
    "vse64.v " vreg ", (" addrreg ")\n\t"

#define VSTRIDE_FROM_1STRIDE_G(strideregvlen, stridereg1, sizeshift)\
    "mul " strideregvlen ", s3, " stridereg1 "\n\t"

#define VSTRIDE_FROM_1STRIDE_C(strideregvlen, stridereg1, sizeshift)\
    "add " strideregvlen ", s3, 0\n\t"\
    "slli " strideregvlen ", " strideregvlen ", " sizeshift "\n\t"

#define PREPARE_STRIDE_G(strideregvlen, stridereg1, offset, sizeshift)\
    "ld " stridereg1 ", " offset "(s2)\n\t"\
    "slli " stridereg1 ", " stridereg1 ", " sizeshift "\n\t"\
    VSTRIDE_FROM_1STRIDE_G(strideregvlen, stridereg1, sizeshift)

#define PREPARE_STRIDE_C(strideregvlen, stridereg1, offset, sizeshift)\
    VSTRIDE_FROM_1STRIDE_C(strideregvlen, stridereg1, sizeshift)

#define VFMA_F0(vdst, vsrc)\
        "vfmacc.vf " vdst ", f0, " vsrc "\n\t"

#define VFIRST(v1, v2) v1
#define VSECOND(v1, v2) v2

#define PREPARE_SCALAR_LOADF0\
    "ld t0, " I_SCALAR "(s2)\n\t" \
    "fld f0, (t0)\n\t"
