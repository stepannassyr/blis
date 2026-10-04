
#define STR_I(x) #x
#define STR(x) STR_I(x)

#define VLOAD_STRIDED(vreg, addrreg, stride1reg)\
    "vlse" SIZEBITS ".v " vreg ", (" addrreg "), " stride1reg "\n\t"

#define VSTORE_STRIDED(vreg, addrreg, stride1reg)\
    "vsse" SIZEBITS ".v " vreg ", (" addrreg "), " stride1reg "\n\t"

#define VLOAD(vreg, addrreg)\
    "vle" SIZEBITS ".v " vreg ", (" addrreg ")\n\t"

#define VSTORE(vreg, addrreg)\
    "vse" SIZEBITS ".v " vreg ", (" addrreg ")\n\t"


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

#define PREPARE_SCALAR_LOADF0_D\
    "fld f0, (%[scalarptr])\n\t"

#define PREPARE_SCALAR_LOADF0_S\
    "flw f0, (%[scalarptr])\n\t"

#define PREPARE_SCALAR_LOADF0_PASTER(dt_suffix) PREPARE_SCALAR_LOADF0_ ## dt_suffix
#define PREPARE_SCALAR_LOADF0(dt_suffix) PREPARE_SCALAR_LOADF0_PASTER(dt_suffix)

#define TAILPREPARE_WHOLEV
#define TAILPREPARE_VREST\
    "vsetvli %[vlen], %[counter], e" SIZEBITS ", m" STR(LMUL) ", ta, ma\n\t"\
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

// --- Software prefetch of packm's source --------------------------------
//
// On an in-order core (SpacemiT X60) every source line packm reads from
// memory stalls the pipeline for its full latency (~340 cycles on the K1);
// prefetching the lines a few columns ahead lets several be in flight.
// prefetch.r off(rs1) is encoded as ori x0, rs1, off+1 (Zicbop; off a multiple
// of 32): no -march support needed, and a no-op hint on cores without Zicbop.
#define PREFETCH_R(addrreg, off) "ori x0, " addrreg ", " STR(off) "+1\n\t"

// All of packm's source prefetching, on or off (the fixes stay either way).
#ifndef RVIV_PACKM_PF
#define RVIV_PACKM_PF 1
#endif
// Copy loops (ukr1_4u*vmx.h, cdim a multiple of vlen): each panel column is
// one contiguous source vector, a column stride apart; prefetch the column
// 2^RVIV_PACKM_PF_COLS_SHIFT ahead -- 8 by default, where prefetch_test on
// the K1 reaches its floor from memory.
#ifndef RVIV_PACKM_PF_COLS_SHIFT
#define RVIV_PACKM_PF_COLS_SHIFT 3
#endif
// Transposing loops (ukr1m_nv_template.h, cdim <= 16): each panel column's
// source is contiguous along k; prefetch this many bytes ahead along it.
// A multiple of 32 up to 2016; 512 is 8 lines.
#ifndef RVIV_PACKM_PF_BYTES
#define RVIV_PACKM_PF_BYTES 512
#endif
// The transposing loop's old prefetch.w of the packed destination, one line in
// seven: off by default -- the destination streams, and a prefetch.w forces
// the read a no-write-allocate store stream would skip.
#ifndef RVIV_PACKM_PF_DEST
#define RVIV_PACKM_PF_DEST 0
#endif

// The lines of one LMUL register group at VLEN 256 (32 bytes a register): the
// first and the last byte's line, and those between.
#define PF_GROUP_LINES_1(r) PREFETCH_R(r, 0)
#define PF_GROUP_LINES_2(r) PREFETCH_R(r, 0) PREFETCH_R(r, 32)
#define PF_GROUP_LINES_4(r) PREFETCH_R(r, 0) PREFETCH_R(r, 64) PREFETCH_R(r, 96)
#define PF_GROUP_LINES_8(r) PREFETCH_R(r, 0) PREFETCH_R(r, 64) PREFETCH_R(r, 128)\
                            PREFETCH_R(r, 192) PREFETCH_R(r, 224)
#define PF_GROUP_LINES_I(lmul, r) PF_GROUP_LINES_##lmul(r)
#define PF_GROUP_LINES(lmul, r) PF_GROUP_LINES_I(lmul, r)

// Hooks in the copy loops, empty unless an includer turns them on
// (ukr1m_pf_on.h / ukr1m_pf_off.h): level-1 kernels share the loops.
//   PF_DECLARE             the registers it needs
//   PF_PREPARE(colstride)  where the column stride (bytes) is in a register
//   PFX(addr)              after each load of a source vector at addr
//   PF_OPERANDS            first in the asm's output operands
#define PF_DECLARE
#define PF_PREPARE(colstridereg)
#define PFX(addrreg)
#define PF_OPERANDS
