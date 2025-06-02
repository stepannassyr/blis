

#define VLOAD_STRIDED(vreg, addrreg, stride1reg)\
    "vlse64.v " vreg ", (" addrreg "), " stride1reg "\n\t"

#define VSTORE_STRIDED(vreg, addrreg, stride1reg)\
    "vsse64.v " vreg ", (" addrreg "), " stride1reg "\n\t"

#define VLOAD(vreg, addrreg)\
    "vle64.v " vreg ", (" addrreg ")\n\t"

#define VSTORE(vreg, addrreg)\
    "vse64.v " vreg ", (" addrreg ")\n\t"


#define VSTRIDE_FROM_1STRIDE_G(strideregvlen, stridereg1, sizeshift)\
    "mul " strideregvlen ", %[vlen], " stridereg1 "\n\t"

#define VSTRIDE_FROM_1STRIDE_C(strideregvlen, stridereg1, sizeshift)\
    "add " strideregvlen ", %[vlen], 0\n\t"\
    "slli " strideregvlen ", " strideregvlen ", " sizeshift "\n\t"


#define PREPARE_STRIDE_G(strideregvlen, stridereg1, sizeshift)\
    "slli " stridereg1 ", " stridereg1 ", " sizeshift "\n\t"\
    VSTRIDE_FROM_1STRIDE_G(strideregvlen, stridereg1, sizeshift)

#define PREPARE_STRIDE_C(strideregvlen, stridereg1, sizeshift)\
    VSTRIDE_FROM_1STRIDE_C(strideregvlen, stridereg1, sizeshift)


#define PREPARE_LDIM_NON1(strideregvlen, ldimreg, sizeshift)\
    "slli " strideregvlen ", " ldimreg ", " sizeshift "\n\t"

#define PREPARE_LDIM_NON1_G(strideregvlen, ldimreg, sizeshift)\
    "mul " strideregvlen ", " ldimreg ", " stridereg1 "\n\t"


#define VFMA_F0(vdst, vsrc)\
        "vfmacc.vf " vdst ", f0, " vsrc "\n\t"

#define VFMUL_F0(vdst, vsrc)\
        "vfmul.vf " vdst ", " vsrc ", f0\n\t"

#define VFIRST(v1, v2) v1
#define VSECOND(v1, v2) v2

#define PREPARE_SCALAR_LOADF0\
    "fld f0, (%[scalarptr])\n\t"

#define TAILPREPARE_WHOLEV
#define TAILPREPARE_VREST\
    "vsetvli %[vlen], %[counter], e" SIZEBITS ", m2, ta, ma\n\t"\
    VSTRIDE_FROM_1STRIDE_X("%[xvstride]", "%[xstride1]", SIZESHIFT)\
    VSTRIDE_FROM_1STRIDE_Y("%[yvstride]", "%[ystride1]", SIZESHIFT)\
    PREPARE_LDIMX("%[xvstride]", "%[ldimx]", SIZESHIFT)\
    PREPARE_LDIMY("%[yvstride]", "%[ldimy]", SIZESHIFT)

#define TAILDECREMENT_WHOLEV "addi %[counter], %[counter], -1\n\t"
#define TAILDECREMENT_VREST "sub %[counter], %[counter], %[vlen]\n\t"

#define MAKEUNROLL_GID(reg, imm)\
    "mv %[unroll], " reg "\n\t"

#define MAKEUNROLL_IID(reg, imm)\
    "li %[unroll], " imm "\n\t"

#define MAKEUNROLL_GMUL(reg, imm)\
    "li %[unroll], " imm "\n\t"\
    "mul %[unroll], " reg ", " imm "\n\t"

#define MAKEUNROLL_GSHIFT(reg, imm)\
    "slli %[unroll], " reg ", " imm "\n\t"

#define MAKEUNROLL_IMUL(reg, imm)\
    "li %[unroll], " imm "\n\t"

#define MAKEUNROLL_ISHIFT(reg, imm)\
    "li %[unroll], 1\n\t"\
    "slli %[unroll], %[unroll], " imm "\n\t"
    
#define MAKEUNROLL_I(reg, imm, shift_or_mul) MAKEUNROLL_I ## shift_or_mul(reg, imm)
#define MAKEUNROLL_FROMG(reg, imm, shift_or_mul) MAKEUNROLL_G ## shift_or_mul(reg, imm)
