/*
 * - copied from _l version and adapted (indices/mask/vrgather)
 */

#include "blis.h"

struct ukrinputs_t
{
    uint64_t k;     // 0
    uint64_t kc;    // 8
    uint64_t kleft; // 16
    uint64_t rs_c;  // 24
    uint64_t cs_c;  // 32
    const void* alpha;    // 40
    const void* a12;      // 48
    const void* a11;      // 56
    const void* b21;      // 64
    void* b11;      // 72
    void* c11;      // 80
    const void* a_next;   // 88
    const void* b_next;   // 96
};

void bli_dgemmtrsm_u_x60_2vx14(
       dim_t               m,
       dim_t               n,
       dim_t               k,
       const void*     restrict alpha,
       const void*     restrict a12,
       const void*     restrict a11,
       const void*     restrict b21,
       void*     restrict b11,
       void*     restrict c11, inc_t rsc, inc_t csc,
       const auxinfo_t* restrict data,
       const cntx_t*    restrict cntx
     )
{
    // unused for now, but keep for later memory/cache tuning
    const void* a_next = bli_auxinfo_next_a( data );
    const void* b_next = bli_auxinfo_next_b( data );

    const uint64_t unroll = 2;

    uint64_t kc = k / unroll;
    uint64_t kleft = k % unroll;
    uint64_t rs_c   = rsc;
    uint64_t cs_c   = csc;

    //printf("rs_c: %llu\n",rs_c);
    //printf("cs_c: %llu\n",cs_c);

    //printf("gemmtrsm_u called with m= %llu, n=%llu, k=%llu\n", m, n, k);
    uint64_t vlen;
    __asm__("csrr %[vlen], vlenb"
            : [vlen] "=r" (vlen)
            :
            :
            );

    vlen = vlen/sizeof(double);

    // TODO: vestigial kernels
    GEMMTRSM_UKR_SETUP_CT(d, vlen*2, 14, false);

    volatile struct ukrinputs_t ukrinputs;
    ukrinputs.k     = k;
    ukrinputs.kc    = kc;
    ukrinputs.kleft = kleft;
    ukrinputs.rs_c  = rs_c;
    ukrinputs.cs_c  = cs_c;
    ukrinputs.alpha = alpha;
    ukrinputs.a12 = a12;
    ukrinputs.a11 = a11;
    ukrinputs.b21 = b21;
    ukrinputs.b11 = b11;
    ukrinputs.c11 = c11;
    ukrinputs.a_next = a_next;
    ukrinputs.b_next = b_next;

    __asm__ (
        // For some reason llvm (at least the BSC version) uses t0-t4 despite the
        // clobber to store addresses of "m" operands. The order of 'ld' operations
        // has been manually adjusted to work despite that
        // This also means that we need to occupy more gprs since we can't
        // reliably "ld reg, %[var]" later.
        // TODO: pure ASM instead of inline ASM and handle the calling convention/boilerplate
    
        "add s2, %[inputs], 0\n\t"
        "ld s11, 56(s2)\n\t" // a11
        "ld s10, 80(s2)\n\t" // c11

        "ld s4, 24(s2)\n\t" // rs_c
        "add s4, s4, -1\n\t"
        "beq s4, zero, .rscokay%=\n\t"
        "unimp\n\t" // Fail if rs_c != 1
        ".rscokay%=:"
        "ld s7, 88(s2)\n\t" // a_next
        "ld s8, 96(s2)\n\t" // b_next
        "vsetvli s3, zero, e64, m1, ta, ma\n\t"
        "slli s3,s3,3\n\t"
        "slli t6,s3,1\n\t"

        // scalars
        "ld s4, 40(s2)\n\t" //alpha
        "fld f1, 0(s4)\n\t"
        
        // B11-tile
        "ld t4, 72(s2)\n\t" // b11
        "add s9, t4, 0\n\t"
        "ld s4, 32(s2)\n\t" // cs_c
        "slli s4, s4, 3\n\t"

        // pull up zeroing for potential skip at k=0
        "vmv.v.i v1,0\n\t"
        "vmv.v.i v2,0\n\t"
        "vmv.v.i v3,0\n\t"
        "vmv.v.i v4,0\n\t"
        "vmv.v.i v5,0\n\t"
        "vmv.v.i v6,0\n\t"
        "vmv.v.i v7,0\n\t"
        "vmv.v.i v8,0\n\t"
        "vmv.v.i v9,0\n\t"
        "vmv.v.i v10,0\n\t"
        "vmv.v.i v11,0\n\t"
        "vmv.v.i v12,0\n\t"
        "vmv.v.i v13,0\n\t"
        "vmv.v.i v14,0\n\t"
        "vmv.v.i v15,0\n\t"
        "vmv.v.i v16,0\n\t"
        "vmv.v.i v17,0\n\t"
        "vmv.v.i v18,0\n\t"
        "vmv.v.i v19,0\n\t"
        "vmv.v.i v20,0\n\t"
        "vmv.v.i v21,0\n\t"
        "vmv.v.i v22,0\n\t"
        "vmv.v.i v23,0\n\t"
        "vmv.v.i v24,0\n\t"
        "vmv.v.i v25,0\n\t"
        "vmv.v.i v26,0\n\t"
        "vmv.v.i v27,0\n\t"
        "vmv.v.i v28,0\n\t"
        // counters
        "ld t5, 0(s2)\n\t" // k
        "beq t5, zero, .k1done%=\n\t" // skip the gemm
        "ld t5, 8(s2)\n\t" // kc
        "ld s5, 16(s2)\n\t" //kleft
        // pointers
        "ld t2, 64(s2)\n\t" // b21
        "add t3, t2, 0\n\t"
        "ld t0, 48(s2)\n\t" // a12
        "add t1, t0, s3\n\t"

        "li s6, 7\n\t" // prefetch distance (TODO: optimize)
        // preload
        "vle64.v v29, (t0)\n\t"
        "fld f2, 0(t2)\n\t"
        "fld f3, 8(t2)\n\t"
        "fld f4, 16(t2)\n\t"
        "fld f5, 24(t2)\n\t"
        "vle64.v v30, (t1)\n\t"
        "fld f6, 32(t2)\n\t"
        "fld f7, 40(t2)\n\t"
        "add t0, t0, t6\n\t"
        "fld f8, 48(t2)\n\t"
        "fld f9, 56(t2)\n\t"
        "fld f10, 64(t2)\n\t"
        "fld f11, 72(t2)\n\t"
        "add t1, t1, t6\n\t"
        "fld f12, 80(t2)\n\t"
        "fld f13, 88(t2)\n\t"
        "fld f14, 96(t2)\n\t"
        "fld f15, 104(t2)\n\t"
        "add t2, t2, 112\n\t"
        "beq t5, zero, .kdone%=\n\t"
        "add t5,t5,-1\n\t"
        "beq t5, zero, .klast%=\n\t"
        "j .kloop%=\n\t"
        ".prefetchc%=:\n\t"
        "prefetch.r 0(s9)\n\t" // B11 is packed, i.e. 14*2*vlen, so we can prefetch in increments of vlen
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        "add s9, s9, s3\n\t"
        "prefetch.r 0(s9)\n\t"
        ".kloop%=:\n\t"
//            "prefetch.r 128(t0)\n\t"
            "vfmacc.vf v1, f2, v29\n\t"
            "vfmacc.vf v2, f2, v30\n\t"
            "vfmacc.vf v3, f3, v29\n\t"
            "fld f19, 24(t2)\n\t"
            "vle64.v v31, (t0)\n\t"
            "vfmacc.vf v4, f3, v30\n\t"
            "vle64.v v0, (t1)\n\t"
            "vfmacc.vf v5, f4, v29\n\t"
            "vfmacc.vf v6, f4, v30\n\t"
            "vfmacc.vf v7, f5, v29\n\t"
            "fld f18, 16(t2)\n\t"
            "vfmacc.vf v8, f5, v30\n\t"
            "vfmacc.vf v9, f6, v29\n\t"
            "fld f16, 0(t2)\n\t"
            "vfmacc.vf v10, f6, v30\n\t"
            "vfmacc.vf v11, f7, v29\n\t"
            "fld f20, 32(t2)\n\t"
            "vfmacc.vf v12, f7, v30\n\t"
            "fld f21, 40(t2)\n\t"
            "vfmacc.vf v13, f8, v29\n\t"
            "add t0, t0, t6\n\t"
            "vfmacc.vf v14, f8, v30\n\t"
            "fld f22, 48(t2)\n\t"
            "vfmacc.vf v15, f9, v29\n\t"
            "fld f17, 8(t2)\n\t"
            "add t1, t1, t6\n\t"
            "vfmacc.vf v16, f9, v30\n\t"
            "vfmacc.vf v17, f10, v29\n\t"
            "fld f23, 56(t2)\n\t"
            "vfmacc.vf v18, f10, v30\n\t"
            "fld f24, 64(t2)\n\t"
            "vfmacc.vf v19, f11, v29\n\t"
            "vfmacc.vf v20, f11, v30\n\t"
            "fld f25, 72(t2)\n\t"
            "add t3, t3, 224\n\t"
            "vfmacc.vf v21, f12, v29\n\t"
            "vfmacc.vf v22, f12, v30\n\t"
            "fld f26, 80(t2)\n\t"
            "vfmacc.vf v23, f13, v29\n\t"
            "fld f9, 56(t3)\n\t"
            "vfmacc.vf v24, f13, v30\n\t"
            "fld f27, 88(t2)\n\t"
            "vfmacc.vf v25, f14, v29\n\t"
            "nop\n\t"
            "vfmacc.vf v26, f14, v30\n\t"
            "fld f28, 96(t2)\n\t"
            "vfmacc.vf v27, f15, v29\n\t"
            "vfmacc.vf v28, f15, v30\n\t"
            "fld f29, 104(t2)\n\t"
            "nop\n\t"
            "vfmacc.vf v1, f16, v31\n\t"
            "fld f3, 8(t3)\n\t"
            "vfmacc.vf v2, f16, v0\n\t"
            "fld f4, 16(t3)\n\t"
            "add t2, t2, 224\n\t"
            "vfmacc.vf v3, f17, v31\n\t"
            "vfmacc.vf v4, f17, v0\n\t"
            "vfmacc.vf v5, f18, v31\n\t"
            "vfmacc.vf v6, f18, v0\n\t"
            "vfmacc.vf v7, f19, v31\n\t"
            "fld f5, 24(t3)\n\t"
            "vle64.v v29, (t0)\n\t"
            "vfmacc.vf v8, f19, v0\n\t"
            "vfmacc.vf v9, f20, v31\n\t"
            "vfmacc.vf v10, f20, v0\n\t"
            "fld f6, 32(t3)\n\t"
            "vfmacc.vf v11, f21, v31\n\t"
            "vfmacc.vf v12, f21, v0\n\t"
            "fld f7, 40(t3)\n\t"
            "vfmacc.vf v13, f22, v31\n\t"
            "vfmacc.vf v14, f22, v0\n\t"
            "fld f2, 0(t3)\n\t"
            //"nop\n\t"
            "vle64.v v30, (t1)\n\t"
            "vfmacc.vf v15, f23, v31\n\t"
            "vfmacc.vf v16, f23, v0\n\t"
            "fld f10, 64(t3)\n\t"
            "add t5,t5,-1\n\t"
            "vfmacc.vf v17, f24, v31\n\t"
            "vfmacc.vf v18, f24, v0\n\t"
            "vfmacc.vf v19, f25, v31\n\t"
            "vfmacc.vf v20, f25, v0\n\t"
            "fld f11, 72(t3)\n\t"
            "vfmacc.vf v21, f26, v31\n\t"
            "add t1, t1, t6\n\t"
            "vfmacc.vf v22, f26, v0\n\t"
            "fld f12, 80(t3)\n\t"
            "vfmacc.vf v23, f27, v31\n\t"
            "vfmacc.vf v24, f27, v0\n\t"
            "fld f13, 88(t3)\n\t"
            "vfmacc.vf v25, f28, v31\n\t"
            "fld f8, 48(t3)\n\t"
            "add t0, t0, t6\n\t"
            "vfmacc.vf v26, f28, v0\n\t"
            "fld f14, 96(t3)\n\t"
            "vfmacc.vf v27, f29, v31\n\t"
            "vfmacc.vf v28, f29, v0\n\t"
            "fld f15, 104(t3)\n\t"
            "beq t5, s6, .prefetchc%=\n\t"
            "bnez t5, .kloop%=\n\t"
        ".klast%=:\n\t"
            "vfmacc.vf v1, f2, v29\n\t"
            "vfmacc.vf v2, f2, v30\n\t"
            "vfmacc.vf v3, f3, v29\n\t"
            "vfmacc.vf v4, f3, v30\n\t"
            "vle64.v v31, (t0)\n\t"
            "vle64.v v0, (t1)\n\t"
            "vfmacc.vf v5, f4, v29\n\t"
            "vfmacc.vf v6, f4, v30\n\t"
            "fld f19, 24(t2)\n\t"
            "vfmacc.vf v7, f5, v29\n\t"
            "fld f18, 16(t2)\n\t"
            "vfmacc.vf v8, f5, v30\n\t"
            "vfmacc.vf v9, f6, v29\n\t"
            "fld f16, 0(t2)\n\t"
            "vfmacc.vf v10, f6, v30\n\t"
            "vfmacc.vf v11, f7, v29\n\t"
            "fld f20, 32(t2)\n\t"
            "vfmacc.vf v12, f7, v30\n\t"
            "fld f21, 40(t2)\n\t"
            "vfmacc.vf v13, f8, v29\n\t"
            "add t0, t0, t6\n\t"
            "vfmacc.vf v14, f8, v30\n\t"
            "fld f22, 48(t2)\n\t"
            "vfmacc.vf v15, f9, v29\n\t"
            "fld f17, 8(t2)\n\t"
            "add t1, t1, t6\n\t"
            "vfmacc.vf v16, f9, v30\n\t"
            "fld f23, 56(t2)\n\t"
            "vfmacc.vf v17, f10, v29\n\t"
            "vfmacc.vf v18, f10, v30\n\t"
            "fld f24, 64(t2)\n\t"
            "vfmacc.vf v19, f11, v29\n\t"
            "vfmacc.vf v20, f11, v30\n\t"
            "fld f25, 72(t2)\n\t"
            "add t3, t3, 224\n\t"
            "vfmacc.vf v21, f12, v29\n\t"
            "vfmacc.vf v22, f12, v30\n\t"
            "fld f26, 80(t2)\n\t"
            "vfmacc.vf v23, f13, v29\n\t"
            "vfmacc.vf v24, f13, v30\n\t"
            "fld f27, 88(t2)\n\t"
            "vfmacc.vf v25, f14, v29\n\t"
            "nop\n\t"
            "vfmacc.vf v26, f14, v30\n\t"
            "fld f28, 96(t2)\n\t"
            "vfmacc.vf v27, f15, v29\n\t"
            "vfmacc.vf v28, f15, v30\n\t"
            "fld f29, 104(t2)\n\t"
            "nop\n\t"
            "vfmacc.vf v1, f16, v31\n\t"
            "vfmacc.vf v2, f16, v0\n\t"
            "vfmacc.vf v3, f17, v31\n\t"
            "vfmacc.vf v4, f17, v0\n\t"
            "vfmacc.vf v5, f18, v31\n\t"
            "vfmacc.vf v6, f18, v0\n\t"
            "vfmacc.vf v7, f19, v31\n\t"
            "vfmacc.vf v8, f19, v0\n\t"
            "vfmacc.vf v9, f20, v31\n\t"
            "vfmacc.vf v10, f20, v0\n\t"
            "vfmacc.vf v11, f21, v31\n\t"
            "vfmacc.vf v12, f21, v0\n\t"
            "vfmacc.vf v13, f22, v31\n\t"
            "vfmacc.vf v14, f22, v0\n\t"
            "nop\n\t"
            "vfmacc.vf v15, f23, v31\n\t"
            "vfmacc.vf v16, f23, v0\n\t"
            "vfmacc.vf v17, f24, v31\n\t"
            "vfmacc.vf v18, f24, v0\n\t"
            "vfmacc.vf v19, f25, v31\n\t"
            "vfmacc.vf v20, f25, v0\n\t"
            "vfmacc.vf v21, f26, v31\n\t"
            "vfmacc.vf v22, f26, v0\n\t"
            "vfmacc.vf v23, f27, v31\n\t"
            "vfmacc.vf v24, f27, v0\n\t"
            "vfmacc.vf v25, f28, v31\n\t"
            "vfmacc.vf v26, f28, v0\n\t"
            "vfmacc.vf v27, f29, v31\n\t"
            "vfmacc.vf v28, f29, v0\n\t"
//            "add t5,t5,-1\n\t"
        ".kdone%=:\n\t"
        "beq s5, zero, .k1done%=\n\t"
        ".k1loop%=:\n\t"
            "vle64.v v29, (t0)\n\t"
            "vle64.v v30, (t1)\n\t"
            "fld f2, 0(t3)\n\t"
            "fld f3, 8(t3)\n\t"
            "fld f4, 16(t3)\n\t"
            "vfmacc.vf v1, f2, v29\n\t"
            "vfmacc.vf v2, f2, v30\n\t"
            "fld f5, 24(t3)\n\t"
            "vfmacc.vf v3, f3, v29\n\t"
            "vfmacc.vf v4, f3, v30\n\t"
            "fld f6, 32(t3)\n\t"
            "vfmacc.vf v5, f4, v29\n\t"
            "vfmacc.vf v6, f4, v30\n\t"
            "fld f7, 40(t3)\n\t"
            "vfmacc.vf v7, f5, v29\n\t"
            "vfmacc.vf v8, f5, v30\n\t"
            "fld f8, 48(t3)\n\t"
            "vfmacc.vf v9, f6, v29\n\t"
            "add t0, t0, t6\n\t"
            "vfmacc.vf v10, f6, v30\n\t"
            "fld f9, 56(t3)\n\t"
            "vfmacc.vf v11, f7, v29\n\t"
            "add t1, t1, t6\n\t"
            "vfmacc.vf v12, f7, v30\n\t"
            "fld f10, 64(t3)\n\t"
            "vfmacc.vf v13, f8, v29\n\t"
            "vfmacc.vf v14, f8, v30\n\t"
            "fld f11, 72(t3)\n\t"
            "vfmacc.vf v15, f9, v29\n\t"
            "vfmacc.vf v16, f9, v30\n\t"
            "fld f12, 80(t3)\n\t"
            "vfmacc.vf v17, f10, v29\n\t"
            "vfmacc.vf v18, f10, v30\n\t"
            "fld f13, 88(t3)\n\t"
            "vfmacc.vf v19, f11, v29\n\t"
            "vfmacc.vf v20, f11, v30\n\t"
            "fld f14, 96(t3)\n\t"
            "vfmacc.vf v21, f12, v29\n\t"
            "add s5,s5,-1\n\t"
            "vfmacc.vf v22, f12, v30\n\t"
            "fld f15, 104(t3)\n\t"
            "vfmacc.vf v23, f13, v29\n\t"
            "vfmacc.vf v24, f13, v30\n\t"
            "add t3, t3, 112\n\t"
            "vfmacc.vf v25, f14, v29\n\t"
            "vfmacc.vf v26, f14, v30\n\t"
            "vfmacc.vf v27, f15, v29\n\t"
            "vfmacc.vf v28, f15, v30\n\t"
            "bnez s5, .k1loop%=\n\t"
        ".k1done%=:\n\t"


        "prefetch.r 0(s7)\n\t"
        "prefetch.r 0(s8)\n\t"

        "add t0, t4, 0\n\t"   // load: B11
                               // From KernelsHowTo.md:        
                               // B11 is stored by rows with   
                               // leading dimension PACKNR,    
                               // where typically PACKNR = NR. 
                               //                              
        "li  s7, 14\n\t"       // PACKNR                       
        "mul t6, s3, s7\n\t"   // vlen*PACKNR*sizeof(double)   
        "slli s7, s7, 3\n\t"   // PACKNR*sizeof(double)
        "add t1, t0, t6\n\t"   // load: B11+vlen*PACKNR        

        // From KernelsHowTo.md:
        // Zero alpha. The microkernel can safely assume that alpha is non-zero;
        // "alpha equals zero" handling is performed at a much higher level,
        // which means that, in such a scenario, the microkernel will never get called.
        //"bnez t5, .alphazero%=\n\t"
        ".alphascale%=:\n\t" // TODO: written manually and unoptimized
            
            "vlse64.v v29, (t0), s7\n\t"  // c0_1
            "vlse64.v v30, (t1), s7\n\t"  // c0_2
            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (t0), s7\n\t"  // c1_1
            "vlse64.v v0, (t1), s7\n\t"  // c1_2
            "vfmsac.vf v1,  f1, v29\n\t" // c0_1
            "vfmsac.vf v2,  f1, v30\n\t" // c0_2

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (t0), s7\n\t" // c2_1
            "vlse64.v v30, (t1), s7\n\t" // c2_2
            "vfmsac.vf v3,  f1, v31\n\t"
            "vfmsac.vf v4,  f1, v0\n\t"


            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (t0), s7\n\t"
            "vlse64.v v0, (t1), s7\n\t"
            "vfmsac.vf v5,  f1, v29\n\t"
            "vfmsac.vf v6,  f1, v30\n\t"

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (t0), s7\n\t"
            "vlse64.v v30, (t1), s7\n\t"
            "vfmsac.vf v7,  f1, v31\n\t"
            "vfmsac.vf v8,  f1, v0\n\t"

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (t0), s7\n\t"
            "vlse64.v v0, (t1), s7\n\t"
            "vfmsac.vf v9,  f1, v29\n\t"
            "vfmsac.vf v10,  f1, v30\n\t"

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (t0), s7\n\t"
            "vlse64.v v30, (t1), s7\n\t"
            "vfmsac.vf v11,  f1, v31\n\t"
            "vfmsac.vf v12,  f1, v0\n\t"

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (t0), s7\n\t"
            "vlse64.v v0, (t1), s7\n\t"
            "vfmsac.vf v13,  f1, v29\n\t"
            "vfmsac.vf v14,  f1, v30\n\t"

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (t0), s7\n\t"
            "vlse64.v v30, (t1), s7\n\t"
            "vfmsac.vf v15,  f1, v31\n\t"
            "vfmsac.vf v16,  f1, v0\n\t"

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (t0), s7\n\t"
            "vlse64.v v0, (t1), s7\n\t"
            "vfmsac.vf v17,  f1, v29\n\t"
            "vfmsac.vf v18,  f1, v30\n\t"

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (t0), s7\n\t"
            "vlse64.v v30, (t1), s7\n\t"
            "vfmsac.vf v19,  f1, v31\n\t"
            "vfmsac.vf v20,  f1, v0\n\t"

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (t0), s7\n\t"
            "vlse64.v v0, (t1), s7\n\t"
            "vfmsac.vf v21,  f1, v29\n\t"
            "vfmsac.vf v22,  f1, v30\n\t"

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (t0), s7\n\t"
            "vlse64.v v30, (t1), s7\n\t"
            "vfmsac.vf v23,  f1, v31\n\t"
            "vfmsac.vf v24,  f1, v0\n\t"

            "add t0, t0, 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (t0), s7\n\t"
            "vlse64.v v0, (t1), s7\n\t"
            "vfmsac.vf v25,  f1, v29\n\t"
            "vfmsac.vf v26,  f1, v30\n\t"

            "vfmsac.vf v27,  f1, v31\n\t"
            "vfmsac.vf v28,  f1, v0\n\t"

                
        // ================= TRSM ===============
        // b11 is in v1-v28 (we need v0 for masking)
        //
        // we have v29-v30 for aiv[12]
        //
        // all f0-f31 are free
        //
        //
        // pointers
        "add t0, s11, 0\n\t"  // load: a11
        "add t1, t0, s3\n\t"  // load: a11 + vlen
                              //  1 2 3 4 | 5 6 7 8
                              //  0 1 2 3 | 4 5 6 7
                              //  0 0 1 2 | 3 4 5 6
                              //  0 0 0 1 | 2 3 4 5
                              //  -----------------
                              //->0 0 0 0 | 1 2 3 4
                              //  0 0 0 0 | 0 1 2 3
                              //  0 0 0 0 | 0 0 1 2
                              //  0 0 0 0 | 0 0 0 1
                              //
        "slli t3, s3, 1\n\t"  // 2*vlen*sizeof(double)

        "add t2, s3, -8\n\t"  // vlen-1

        "add s11, t1, t2\n\t" // a11 + vlen + (vlen-1) = a11+(2*vlen-1)
                              //  1 2 3 4 | 5 6 7 8
                              //  0 1 2 3 | 4 5 6 7
                              //  0 0 1 2 | 3 4 5 6
                              //  0 0 0 1 | 2 3 4 5
                              //  -----------------
                              //  0 0 0 0 | 1 2 3 4
                              //  0 0 0 0 | 0 1 2 3
                              //  0 0 0 0 | 0 0 1 2
                              //->0 0 0 0 | 0 0 0 1
        // counter
        "srli t5, s3, 2\n\t"  // loop counter: vlen*2
                              // (vlen*sizeof(double)/4)
        "srli t6, t5, 1\n\t"  // vlen in elements

        "add t2, t3, -8\n\t"  // 2*vlen -1
        "mul t2, t2, t5\n\t"  // (2*vlen-1)*(2*vlen)

        // Move c11 pointer to last row
        "add s8, t5, -1\n\t" // 2*vlen-1
        "slli s7, s8, 3\n\t" // *sizeof(double)
        "add s10, s10, s7\n\t"
        // Move b11 pointer to last row
        "li s7, 112\n\t" // nr*sizeof(double)
        "mul s7, s7, s8\n\t" // nr*(2*vlen-1)*sizeof(double)
        "add t4, t4, s7\n\t" // 

        "add t0, t0, t2\n\t"
                              //  1 2 3 4 | 5 6 7 ->8
                              //  0 1 2 3 | 4 5 6   7
                              //  0 0 1 2 | 3 4 5   6
                              //  0 0 0 1 | 2 3 4   5
                              //  -------------------
                              //  0 0 0 0 | 1 2 3   4
                              //  0 0 0 0 | 0 1 2   3
                              //  0 0 0 0 | 0 0 1   2
                              //  0 0 0 0 | 0 0 0   1
        "add t1, t1, t2\n\t"
                              //  1 2 3 4 | 5 6 7   8
                              //  0 1 2 3 | 4 5 6   7
                              //  0 0 1 2 | 3 4 5   6
                              //  0 0 0 1 | 2 3 4   5
                              //  -------------------
                              //  0 0 0 0 | 1 2 3 ->4
                              //  0 0 0 0 | 0 1 2   3
                              //  0 0 0 0 | 0 0 1   2
                              //  0 0 0 0 | 0 0 0   1
        "add s11, s11, t2\n\t"// a11 + (2*vlen-1) + (2*vlen-1)*vlen
                              //  1 2 3 4 | 5 6 7   8
                              //  0 1 2 3 | 4 5 6   7
                              //  0 0 1 2 | 3 4 5   6
                              //  0 0 0 1 | 2 3 4   5
                              //  -------------------
                              //  0 0 0 0 | 1 2 3   4
                              //  0 0 0 0 | 0 1 2   3
                              //  0 0 0 0 | 0 0 1   2
                              //  0 0 0 0 | 0 0 0 ->1
        // vector mask for vle and vfnmsac
                              //  |vl|
        "li s8, 1\n\t"        //  0001
        "sll s8, s8, t6\n\t"  // 10000
        "add s8, s8, -1\n\t"  //  1111
        "srli s8, s8, 1\n\t"  //  0111
        "vmv.v.x v0, s8\n\t"

        
        // We don't need to slide the vectors, masking
        // is enough, but we do need to extract the last element
        //
        // b1  
        // 1   
        // 2   
        // 3   
        // 4 -> f

        "add s9, t6, -1\n\t"
        "vrgather.vx v31, v2, s9\n\t"
        "vfmv.f.s f14, v31\n\t"
        "vrgather.vx v31, v4, s9\n\t"
        "vfmv.f.s f15, v31\n\t"
        "vrgather.vx v31, v6, s9\n\t"
        "vfmv.f.s f16, v31\n\t"
        "vrgather.vx v31, v8, s9\n\t"
        "vfmv.f.s f17, v31\n\t"
        "vrgather.vx v31, v10, s9\n\t"
        "vfmv.f.s f18, v31\n\t"
        "vrgather.vx v31, v12, s9\n\t"
        "vfmv.f.s f19, v31\n\t"
        "vrgather.vx v31, v14, s9\n\t"
        "vfmv.f.s f20, v31\n\t"
        "vrgather.vx v31, v16, s9\n\t"
        "vfmv.f.s f21, v31\n\t"
        "vrgather.vx v31, v18, s9\n\t"
        "vfmv.f.s f22, v31\n\t"
        "vrgather.vx v31, v20, s9\n\t"
        "vfmv.f.s f23, v31\n\t"
        "vrgather.vx v31, v22, s9\n\t"
        "vfmv.f.s f24, v31\n\t"
        "vrgather.vx v31, v24, s9\n\t"
        "vfmv.f.s f25, v31\n\t"
        "vrgather.vx v31, v26, s9\n\t"
        "vfmv.f.s f26, v31\n\t"
        "vrgather.vx v31, v28, s9\n\t"
        "vfmv.f.s f27, v31\n\t"

        ".trsmv1%=:\n\t"
            "add t2, s10, 0\n\t"  // fstore: c11

            // f28 < 1/a(n-i)(n-i)
            "fld f28, 0(s11)\n\t"
            #ifdef BLIS_ENABLE_TRSM_PREINVERSION
                // Nothing
            #else
                "li s5, 1\n\t"
                "fcvt.d.l f1, s5\n\t" // 1.0
                "fdiv.d f28, f1, f28\n\t" // 1/a11
            #endif
            "fmul.d f0, f28, f14\n\t"
            "add s11, s11, -8\n\t" // previous row
            "fmul.d f1, f28, f15\n\t"
            "fmul.d f2, f28, f16\n\t"
            "vle64.v v29, (t0)\n\t"
            "fmul.d f3, f28, f17\n\t"
            "fmul.d f4, f28, f18\n\t"
            "vle64.v v30, (t1), v0.t\n\t"
            "fmul.d f5, f28, f19\n\t"
            "sub t0, t0, t3\n\t" // previous column
            "sub s11, s11, t3\n\t" // previous column
            "fmul.d f6, f28, f20\n\t"
            "fmul.d f7, f28, f21\n\t"
            "sub t1, t1, t3\n\t" // previous column
            "fmul.d f8, f28, f22\n\t"
            "fmul.d f9, f28, f23\n\t"
            "add t5, t5, -1\n\t"
            "fmul.d f10, f28, f24\n\t"
            "fmul.d f11, f28, f25\n\t"
            "fmul.d f12, f28, f26\n\t"
            "fmul.d f13, f28, f27\n\t"

            // From KernelsHowTo.d:
            // Output. This microkernel must write its result to two places: 
            // the submatrix B11 of the current packed micropanel of B and 
            // the submatrix C11 of the output matrix C
            "fsd f0, (t2)\n\t"
            "fsd f0, 0(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f1, (t2)\n\t"
            "fsd f1, 8(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f2, (t2)\n\t"
            "fsd f2, 16(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f3, (t2)\n\t"
            "fsd f3, 24(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f4, (t2)\n\t"
            "fsd f4, 32(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f5, (t2)\n\t"
            "fsd f5, 40(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f6, (t2)\n\t"
            "fsd f6, 48(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f7, (t2)\n\t"
            "fsd f7, 56(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f8, (t2)\n\t"
            "fsd f8, 64(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f9, (t2)\n\t"
            "fsd f9, 72(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f10, (t2)\n\t"
            "fsd f10, 80(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f11, (t2)\n\t"
            "fsd f11, 88(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f12, (t2)\n\t"
            "fsd f12, 96(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f13, (t2)\n\t"
            "fsd f13, 104(t4)\n\t"

            "add s10, s10, -8\n\t" // previous row in c11
            "add t4, t4, -112\n\t" // previous row in b11

            "beq t5, t6, .trsmv1end%=\n\t"

            "vfnmsac.vf v1,  f0, v29\n\t"
            "vfnmsac.vf v2,  f0, v30, v0.t\n\t"
            "vfnmsac.vf v3,  f1, v29\n\t"
            "vfnmsac.vf v4,  f1, v30, v0.t\n\t"
            "vfnmsac.vf v5,  f2, v29\n\t"
            "vfnmsac.vf v6,  f2, v30, v0.t\n\t"
            "vfnmsac.vf v7,  f3, v29\n\t"
            "vfnmsac.vf v8,  f3, v30, v0.t\n\t"
            "vfnmsac.vf v9,  f4, v29\n\t"
            "vfnmsac.vf v10, f4, v30, v0.t\n\t"
            "vfnmsac.vf v11, f5, v29\n\t"
            "vfnmsac.vf v12, f5, v30, v0.t\n\t"
            "vfnmsac.vf v13, f6, v29\n\t"
            "vfnmsac.vf v14, f6, v30, v0.t\n\t"
            "vfnmsac.vf v15, f7, v29\n\t"
            "vfnmsac.vf v16, f7, v30, v0.t\n\t"
            "vfnmsac.vf v17, f8, v29\n\t"
            "vfnmsac.vf v18, f8, v30, v0.t\n\t"
            "vfnmsac.vf v19, f9, v29\n\t"
            "vfnmsac.vf v20, f9, v30, v0.t\n\t"
            "vfnmsac.vf v21, f10, v29\n\t"
            "vfnmsac.vf v22, f10, v30, v0.t\n\t"
            "vfnmsac.vf v23, f11, v29\n\t"
            "vfnmsac.vf v24, f11, v30, v0.t\n\t"
            "vfnmsac.vf v25, f12, v29\n\t"
            "vfnmsac.vf v26, f12, v30, v0.t\n\t"
            "vfnmsac.vf v27, f13, v29\n\t"
            "vfnmsac.vf v28, f13, v30, v0.t\n\t"

            "srli s8, s8, 1\n\t"  //  0111
            "vmv.v.x v0, s8\n\t"


            "add s9, s9, -1\n\t"
            "vrgather.vx v31, v2, s9\n\t"
            "vfmv.f.s f14, v31\n\t"
            "vrgather.vx v31, v4, s9\n\t"
            "vfmv.f.s f15, v31\n\t"
            "vrgather.vx v31, v6, s9\n\t"
            "vfmv.f.s f16, v31\n\t"
            "vrgather.vx v31, v8, s9\n\t"
            "vfmv.f.s f17, v31\n\t"
            "vrgather.vx v31, v10, s9\n\t"
            "vfmv.f.s f18, v31\n\t"
            "vrgather.vx v31, v12, s9\n\t"
            "vfmv.f.s f19, v31\n\t"
            "vrgather.vx v31, v14, s9\n\t"
            "vfmv.f.s f20, v31\n\t"
            "vrgather.vx v31, v16, s9\n\t"
            "vfmv.f.s f21, v31\n\t"
            "vrgather.vx v31, v18, s9\n\t"
            "vfmv.f.s f22, v31\n\t"
            "vrgather.vx v31, v20, s9\n\t"
            "vfmv.f.s f23, v31\n\t"
            "vrgather.vx v31, v22, s9\n\t"
            "vfmv.f.s f24, v31\n\t"
            "vrgather.vx v31, v24, s9\n\t"
            "vfmv.f.s f25, v31\n\t"
            "vrgather.vx v31, v26, s9\n\t"
            "vfmv.f.s f26, v31\n\t"
            "vrgather.vx v31, v28, s9\n\t"
            "vfmv.f.s f27, v31\n\t"

            "j .trsmv1%=\n\t"
        ".trsmv1end%=:\n\t"

        "vfnmsac.vf v1,  f0, v29\n\t"
        "vfnmsac.vf v3,  f1, v29\n\t"
        "vfnmsac.vf v5,  f2, v29\n\t"
        "vfnmsac.vf v7,  f3, v29\n\t"
        "vfnmsac.vf v9,  f4, v29\n\t"
        "vfnmsac.vf v11, f5, v29\n\t"
        "vfnmsac.vf v13, f6, v29\n\t"
        "vfnmsac.vf v15, f7, v29\n\t"
        "vfnmsac.vf v17, f8, v29\n\t"
        "vfnmsac.vf v19, f9, v29\n\t"
        "vfnmsac.vf v21, f10, v29\n\t"
        "vfnmsac.vf v23, f11, v29\n\t"
        "vfnmsac.vf v25, f12, v29\n\t"
        "vfnmsac.vf v27, f13, v29\n\t"

                              //  |vl|
        "li s8, 1\n\t"        //  0001
        "sll s8, s8, t6\n\t"  // 10000
        "add s8, s8, -1\n\t"  //  1111
        "srli s8, s8, 1\n\t"  //  0111
        "vmv.v.x v0, s8\n\t"

        "add s9, t6, -1\n\t"
        "vrgather.vx v31, v1, s9\n\t"
        "vfmv.f.s f14, v31\n\t"
        "vrgather.vx v31, v3, s9\n\t"
        "vfmv.f.s f15, v31\n\t"
        "vrgather.vx v31, v5, s9\n\t"
        "vfmv.f.s f16, v31\n\t"
        "vrgather.vx v31, v7, s9\n\t"
        "vfmv.f.s f17, v31\n\t"
        "vrgather.vx v31, v9, s9\n\t"
        "vfmv.f.s f18, v31\n\t"
        "vrgather.vx v31, v11, s9\n\t"
        "vfmv.f.s f19, v31\n\t"
        "vrgather.vx v31, v13, s9\n\t"
        "vfmv.f.s f20, v31\n\t"
        "vrgather.vx v31, v15, s9\n\t"
        "vfmv.f.s f21, v31\n\t"
        "vrgather.vx v31, v17, s9\n\t"
        "vfmv.f.s f22, v31\n\t"
        "vrgather.vx v31, v19, s9\n\t"
        "vfmv.f.s f23, v31\n\t"
        "vrgather.vx v31, v21, s9\n\t"
        "vfmv.f.s f24, v31\n\t"
        "vrgather.vx v31, v23, s9\n\t"
        "vfmv.f.s f25, v31\n\t"
        "vrgather.vx v31, v25, s9\n\t"
        "vfmv.f.s f26, v31\n\t"
        "vrgather.vx v31, v27, s9\n\t"
        "vfmv.f.s f27, v31\n\t"

        ".trsmv2%=:\n\t"
            "add t2, s10, 0\n\t"  // fstore: c11

            "fld f28, 0(s11)\n\t"
            #ifdef BLIS_ENABLE_TRSM_PREINVERSION
                // Nothing
            #else
                "li s5, 1\n\t"
                "fcvt.d.l f1, s5\n\t" // 1.0
                "fdiv.d f28, f1, f28\n\t" // 1/a11
            #endif
            "fmul.d f0, f28, f14\n\t"
            "add s11, s11, -8\n\t"
            "fmul.d f1, f28, f15\n\t"
            "fmul.d f2, f28, f16\n\t"
            "vle64.v v29, (t0), v0.t\n\t"
            "fmul.d f3, f28, f17\n\t"
            "fmul.d f4, f28, f18\n\t"
            "fmul.d f5, f28, f19\n\t"
            "sub t0, t0, t3\n\t" // previous column
            "sub s11, s11, t3\n\t" // previous column
            "fmul.d f6, f28, f20\n\t"
            "fmul.d f7, f28, f21\n\t"
            "fmul.d f8, f28, f22\n\t"
            "fmul.d f9, f28, f23\n\t"
            "add t5, t5, -1\n\t"
            "fmul.d f10, f28, f24\n\t"
            "fmul.d f11, f28, f25\n\t"
            "fmul.d f12, f28, f26\n\t"
            "fmul.d f13, f28, f27\n\t"

            "fsd f0, (t2)\n\t"
            "fsd f0, 0(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f1, (t2)\n\t"
            "fsd f1, 8(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f2, (t2)\n\t"
            "fsd f2, 16(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f3, (t2)\n\t"
            "fsd f3, 24(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f4, (t2)\n\t"
            "fsd f4, 32(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f5, (t2)\n\t"
            "fsd f5, 40(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f6, (t2)\n\t"
            "fsd f6, 48(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f7, (t2)\n\t"
            "fsd f7, 56(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f8, (t2)\n\t"
            "fsd f8, 64(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f9, (t2)\n\t"
            "fsd f9, 72(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f10, (t2)\n\t"
            "fsd f10, 80(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f11, (t2)\n\t"
            "fsd f11, 88(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f12, (t2)\n\t"
            "fsd f12, 96(t4)\n\t"
            "add t2, t2, s4\n\t"
            "fsd f13, (t2)\n\t"
            "fsd f13, 104(t4)\n\t"

            "add s10, s10, -8\n\t" // previous row in c11
            "add t4, t4, -112\n\t" // previous row in b11

            "beq t5, zero, .trsmv2end%=\n\t"


            "vfnmsac.vf v1,  f0, v29, v0.t\n\t"
            "vfnmsac.vf v3,  f1, v29, v0.t\n\t"
            "vfnmsac.vf v5,  f2, v29, v0.t\n\t"
            "vfnmsac.vf v7,  f3, v29, v0.t\n\t"
            "vfnmsac.vf v9,  f4, v29, v0.t\n\t"
            "vfnmsac.vf v11, f5, v29, v0.t\n\t"
            "vfnmsac.vf v13, f6, v29, v0.t\n\t"
            "vfnmsac.vf v15, f7, v29, v0.t\n\t"
            "vfnmsac.vf v17, f8, v29, v0.t\n\t"
            "vfnmsac.vf v19, f9, v29, v0.t\n\t"
            "vfnmsac.vf v21, f10, v29, v0.t\n\t"
            "vfnmsac.vf v23, f11, v29, v0.t\n\t"
            "vfnmsac.vf v25, f12, v29, v0.t\n\t"
            "vfnmsac.vf v27, f13, v29, v0.t\n\t"

        
            "srli s8, s8, 1\n\t"  //  0111
            "vmv.v.x v0, s8\n\t"

            "add s9, s9, -1\n\t"
            "vrgather.vx v31, v1, s9\n\t"
            "vfmv.f.s f14, v31\n\t"
            "vrgather.vx v31, v3, s9\n\t"
            "vfmv.f.s f15, v31\n\t"
            "vrgather.vx v31, v5, s9\n\t"
            "vfmv.f.s f16, v31\n\t"
            "vrgather.vx v31, v7, s9\n\t"
            "vfmv.f.s f17, v31\n\t"
            "vrgather.vx v31, v9, s9\n\t"
            "vfmv.f.s f18, v31\n\t"
            "vrgather.vx v31, v11, s9\n\t"
            "vfmv.f.s f19, v31\n\t"
            "vrgather.vx v31, v13, s9\n\t"
            "vfmv.f.s f20, v31\n\t"
            "vrgather.vx v31, v15, s9\n\t"
            "vfmv.f.s f21, v31\n\t"
            "vrgather.vx v31, v17, s9\n\t"
            "vfmv.f.s f22, v31\n\t"
            "vrgather.vx v31, v19, s9\n\t"
            "vfmv.f.s f23, v31\n\t"
            "vrgather.vx v31, v21, s9\n\t"
            "vfmv.f.s f24, v31\n\t"
            "vrgather.vx v31, v23, s9\n\t"
            "vfmv.f.s f25, v31\n\t"
            "vrgather.vx v31, v25, s9\n\t"
            "vfmv.f.s f26, v31\n\t"
            "vrgather.vx v31, v27, s9\n\t"
            "vfmv.f.s f27, v31\n\t"

            "j .trsmv2%=\n\t"
        ".trsmv2end%=:\n\t"

        : [dummy_c] "+m"(*(double(*)[])c11), [dummy_b] "+m"(*(double(*)[])b11)
        : [inputs] "r" (&ukrinputs)
        : "t0", "t1", "t2", "t3", "t4", "t5", "t6",
        "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
        "f0","f1",
        "f2","f3","f4","f5","f6","f7","f8","f9",
        "f10","f11","f12","f13","f14","f15","f16","f17",
        "f18","f19","f20","f21","f22","f23","f24","f25",
        "f26","f27","f28","f29",
        "v0", "v1","v2","v3","v4","v5","v6","v7","v8",
        "v9","v10","v11","v12","v13","v14","v15","v16",
        "v17","v18","v19","v20","v21","v22","v23","v24",
        "v25","v26","v27","v28","v29","v30","v31"
    );

    GEMMTRSM_UKR_FLUSH_CT(d);
}
