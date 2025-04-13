

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

#define PREPARE_LDIM_NON1(ldimreg, offset, sizeshift)\
    "ld " ldimreg ", " offset "(s2)\n\t"\
    "slli " ldimreg ", " ldimreg ", " sizeshift "\n\t"

#define PREPARE_LDIM_NON1_G(ldimreg, offset, stridereg1, sizeshift)\
    "ld " ldimreg ", " offset "(s2)\n\t"\
    "mul " ldimreg ", " ldimreg ", " stridereg1 "\n\t"

#define VFMA_F0(vdst, vsrc)\
        "vfmacc.vf " vdst ", f0, " vsrc "\n\t"

#define VFMUL_F0(vdst, vsrc)\
        "vfmul.vf " vdst ", " vsrc ", f0\n\t"

#define VFIRST(v1, v2) v1
#define VSECOND(v1, v2) v2

#define PREPARE_SCALAR_LOADF0\
    "ld t0, " I_SCALAR "(s2)\n\t" \
    "fld f0, (t0)\n\t"

#define TAILPREPARE_WHOLEV
#define TAILPREPARE_VREST\
    "vsetvli s3, t4, e" SIZEBITS ", m2, ta, ma\n\t"\
    PREPARE_LDIMX("t6", I_LDIMX, SIZESHIFT)\
    PREPARE_LDIMY("t5", I_LDIMY, SIZESHIFT)

#define TAILDECREMENT_WHOLEV "addi t4, t4, -1\n\t"
#define TAILDECREMENT_VREST "sub t4, t4, s3\n\t"
