/*
 * - copied from 2vx14 dgemm kernel + adapted + TRSM manually added
 * - TRSM vectorization with fma along mr is awkward, requiring 
 *   vslide1down.vx, general-stride loads and using non-vec FP stores
 * TODO: dgemm and dgemmtrsm vectorization along nr
 */

#include "blis.h"


//typedef struct
//{
//    uint64_t k;        // 0
//    uint64_t kc;       // 8
//    uint64_t kleft;    // 16
//    uint64_t rs_c;     // 24
//    uint64_t cs_c;     // 32
//    uint64_t vlen;     // 40
//    const void* alpha; // 48
//    const void* a10;   // 56
//    const void* a11;   // 64
//    const void* b01;   // 72
//    void* b11;         // 80
//    void* c11;         // 88
//    const void* a_next;// 96
//    const void* b_next;// 104
//} ukrinputs_t;

void bli_dgemmtrsm_l_x60_2vx14(
       dim_t               m,
       dim_t               n,
       dim_t               k,
       const void*     restrict alpha,
       const void*     restrict a10,
       const void*     restrict a11,
       const void*     restrict b01,
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

    //printf("gemmtrsm_l called with m= %llu, n=%llu, k=%llu\n", m, n, k);
    // vlen should be half of MR
    uint64_t vlen = bli_cntx_get_blksz_def_dt( BLIS_DOUBLE, BLIS_MR, cntx )/2;

    vlen *= sizeof(double);

    // override vlen
    __asm__(
            //"csrr %[vlen],vlenb\n\t"
            "vsetvli %[vlen], %[vlen], e8, m1, ta, ma\n\t"
            : [vlen] "+r" (vlen)
            :
            :
       );

    vlen = vlen/sizeof(double);

    // TODO: vestigial kernels
    GEMMTRSM_UKR_SETUP_CT(d, vlen*2, 14, false);

    //volatile ukrinputs_t ukrinputs;
    //ukrinputs.k     = k;
    //ukrinputs.kc    = kc;
    //ukrinputs.kleft = kleft;
    //ukrinputs.rs_c  = rs_c;
    //ukrinputs.cs_c  = cs_c;
    //ukrinputs.vlen  = vlen;
    //ukrinputs.alpha = alpha;
    //ukrinputs.a10 = a10;
    //ukrinputs.a11 = a11;
    //ukrinputs.b01 = b01;
    //ukrinputs.b11 = b11;
    //ukrinputs.c11 = c11;
    //ukrinputs.a_next = a_next;
    //ukrinputs.b_next = b_next;

    uint64_t anext_cstride = (uint64_t)a_next;
    uint64_t bnext_vdown = (uint64_t)b_next;

    // init ptr11 to a10
    // it will be used as a10, b11 and a11
    void* ptr11 = a10;
    void* ptr12 = NULL;

    // init ptr21 to b01
    // it will be used as b01 and c11
    void* ptr21 = b01;
    void* ptr22 = NULL;

    void* alpha_pref_ptr = alpha;
    uint64_t rs_c_pref_dist = rs_c;

    uint64_t counter = 0;

    uint64_t unroll_vlenxn = unroll;
    __asm__ (
        
        //"ld %[a11], 64(s2)\n\t" // a11
        //"ld %[c11], 88(s2)\n\t" // c11

        //"ld %[cs_c], 24(s2)\n\t" // rs_c
        "add %[rs_c_pref_dist], %[rs_c_pref_dist], -1\n\t"
        "beq %[rs_c_pref_dist], zero, .rscokay%=\n\t"
        "unimp\n\t" // Fail if rs_c != 1
        ".rscokay%=:"
        // load pref_dist
        "li %[rs_c_pref_dist], 7\n\t"
        //"ld %[anext_cstride], 96(s2)\n\t" // a_next
        //"ld %[bnext_vdown], 104(s2)\n\t" // b_next
        //"ld %[vlen], 40(s2)\n\t"  // vlen
        "vsetvli %[vlen], %[vlen], e64, m1, ta, ma\n\t"
        "slli %[vlen],%[vlen],3\n\t"

        // scalars
        //"ld %[cs_c], 48(s2)\n\t" //alpha
        "fld f1, 0(%[alpha_pref_ptr])\n\t"
        
        // B11-tile
        //"ld %[b11], 80(s2)\n\t" // b11
        "mv %[alpha_pref_ptr], %[b11]\n\t"
        //"ld %[cs_c], 32(s2)\n\t" // cs_c
        "slli %[cs_c], %[cs_c], 3\n\t"

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
        // "ld t0, 0(s2)\n\t" // k
        "beq %[k], zero, .k1done%=\n\t" // skip the gemm
        //"ld t0, 8(s2)\n\t" // kc
        //"ld s5, 16(s2)\n\t" //kleft
        "divu t0, %[k], %[unroll_vlenxn]\n\t"
        "remu %[k], %[k], %[unroll_vlenxn]\n\t"
        
        // Now reuse unroll for vlenxn
        "slli %[unroll_vlenxn],%[vlen],1\n\t"

        // pointers
        //"ld %[ptr21], 72(s2)\n\t" // b01
        "add t2, %[ptr21], 0\n\t"
        //"ld %[ptr11], 56(s2)\n\t" // a10
        "add t1, %[ptr11], %[vlen]\n\t"


        // preload
        "vle64.v v29, (%[ptr11])\n\t"
        "fld f2, 0(%[ptr21])\n\t"
        "fld f3, 8(%[ptr21])\n\t"
        "fld f4, 16(%[ptr21])\n\t"
        "fld f5, 24(%[ptr21])\n\t"
        "vle64.v v30, (t1)\n\t"
        "fld f6, 32(%[ptr21])\n\t"
        "fld f7, 40(%[ptr21])\n\t"
        "fld f8, 48(%[ptr21])\n\t"
        "fld f9, 56(%[ptr21])\n\t"
        "fld f10, 64(%[ptr21])\n\t"
        "fld f11, 72(%[ptr21])\n\t"
        "fld f12, 80(%[ptr21])\n\t"
        "fld f13, 88(%[ptr21])\n\t"
        "fld f14, 96(%[ptr21])\n\t"
        "fld f15, 104(%[ptr21])\n\t"
        "beq t0, zero, .kdone%=\n\t"
        "add %[ptr11], %[ptr11], %[unroll_vlenxn]\n\t"
        "add t1, t1, %[unroll_vlenxn]\n\t"
        "add %[ptr21], %[ptr21], 112\n\t"
        "add t0,t0,-1\n\t"
        "beq t0, zero, .klast%=\n\t"
        "j .kloop%=\n\t"
        ".prefetchc%=:\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t" // B11 is packed, i.e. 14*2*vlen, so we can prefetch in increments of vlen
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        "add %[alpha_pref_ptr], %[alpha_pref_ptr], %[vlen]\n\t"
        "prefetch.r 0(%[alpha_pref_ptr])\n\t"
        ".kloop%=:\n\t"
//            "prefetch.r 128(%[ptr11])\n\t"
            "vfmacc.vf v1, f2, v29\n\t"
            "vfmacc.vf v2, f2, v30\n\t"
            "vfmacc.vf v3, f3, v29\n\t"
            "fld f19, 24(%[ptr21])\n\t"
            "vle64.v v31, (%[ptr11])\n\t"
            "vfmacc.vf v4, f3, v30\n\t"
            "vle64.v v0, (t1)\n\t"
            "vfmacc.vf v5, f4, v29\n\t"
            "vfmacc.vf v6, f4, v30\n\t"
            "vfmacc.vf v7, f5, v29\n\t"
            "fld f18, 16(%[ptr21])\n\t"
            "vfmacc.vf v8, f5, v30\n\t"
            "vfmacc.vf v9, f6, v29\n\t"
            "fld f16, 0(%[ptr21])\n\t"
            "vfmacc.vf v10, f6, v30\n\t"
            "vfmacc.vf v11, f7, v29\n\t"
            "fld f20, 32(%[ptr21])\n\t"
            "vfmacc.vf v12, f7, v30\n\t"
            "fld f21, 40(%[ptr21])\n\t"
            "vfmacc.vf v13, f8, v29\n\t"
            "add %[ptr11], %[ptr11], %[unroll_vlenxn]\n\t"
            "vfmacc.vf v14, f8, v30\n\t"
            "fld f22, 48(%[ptr21])\n\t"
            "vfmacc.vf v15, f9, v29\n\t"
            "fld f17, 8(%[ptr21])\n\t"
            "add t1, t1, %[unroll_vlenxn]\n\t"
            "vfmacc.vf v16, f9, v30\n\t"
            "vfmacc.vf v17, f10, v29\n\t"
            "fld f23, 56(%[ptr21])\n\t"
            "vfmacc.vf v18, f10, v30\n\t"
            "fld f24, 64(%[ptr21])\n\t"
            "vfmacc.vf v19, f11, v29\n\t"
            "vfmacc.vf v20, f11, v30\n\t"
            "fld f25, 72(%[ptr21])\n\t"
            "add t2, t2, 224\n\t"
            "vfmacc.vf v21, f12, v29\n\t"
            "vfmacc.vf v22, f12, v30\n\t"
            "fld f26, 80(%[ptr21])\n\t"
            "vfmacc.vf v23, f13, v29\n\t"
            "fld f9, 56(t2)\n\t"
            "vfmacc.vf v24, f13, v30\n\t"
            "fld f27, 88(%[ptr21])\n\t"
            "vfmacc.vf v25, f14, v29\n\t"
            "nop\n\t"
            "vfmacc.vf v26, f14, v30\n\t"
            "fld f28, 96(%[ptr21])\n\t"
            "vfmacc.vf v27, f15, v29\n\t"
            "vfmacc.vf v28, f15, v30\n\t"
            "fld f29, 104(%[ptr21])\n\t"
            "nop\n\t"
            "vfmacc.vf v1, f16, v31\n\t"
            "fld f3, 8(t2)\n\t"
            "vfmacc.vf v2, f16, v0\n\t"
            "fld f4, 16(t2)\n\t"
            "add %[ptr21], %[ptr21], 224\n\t"
            "vfmacc.vf v3, f17, v31\n\t"
            "vfmacc.vf v4, f17, v0\n\t"
            "vfmacc.vf v5, f18, v31\n\t"
            "vfmacc.vf v6, f18, v0\n\t"
            "vfmacc.vf v7, f19, v31\n\t"
            "fld f5, 24(t2)\n\t"
            "vle64.v v29, (%[ptr11])\n\t"
            "vfmacc.vf v8, f19, v0\n\t"
            "vfmacc.vf v9, f20, v31\n\t"
            "vfmacc.vf v10, f20, v0\n\t"
            "fld f6, 32(t2)\n\t"
            "vfmacc.vf v11, f21, v31\n\t"
            "vfmacc.vf v12, f21, v0\n\t"
            "fld f7, 40(t2)\n\t"
            "vfmacc.vf v13, f22, v31\n\t"
            "vfmacc.vf v14, f22, v0\n\t"
            "fld f2, 0(t2)\n\t"
            //"nop\n\t"
            "vle64.v v30, (t1)\n\t"
            "vfmacc.vf v15, f23, v31\n\t"
            "vfmacc.vf v16, f23, v0\n\t"
            "fld f10, 64(t2)\n\t"
            "add t0,t0,-1\n\t"
            "vfmacc.vf v17, f24, v31\n\t"
            "vfmacc.vf v18, f24, v0\n\t"
            "vfmacc.vf v19, f25, v31\n\t"
            "vfmacc.vf v20, f25, v0\n\t"
            "fld f11, 72(t2)\n\t"
            "vfmacc.vf v21, f26, v31\n\t"
            "add t1, t1, %[unroll_vlenxn]\n\t"
            "vfmacc.vf v22, f26, v0\n\t"
            "fld f12, 80(t2)\n\t"
            "vfmacc.vf v23, f27, v31\n\t"
            "vfmacc.vf v24, f27, v0\n\t"
            "fld f13, 88(t2)\n\t"
            "vfmacc.vf v25, f28, v31\n\t"
            "fld f8, 48(t2)\n\t"
            "add %[ptr11], %[ptr11], %[unroll_vlenxn]\n\t"
            "vfmacc.vf v26, f28, v0\n\t"
            "fld f14, 96(t2)\n\t"
            "vfmacc.vf v27, f29, v31\n\t"
            "vfmacc.vf v28, f29, v0\n\t"
            "fld f15, 104(t2)\n\t"
            "beq t0, %[rs_c_pref_dist], .prefetchc%=\n\t"
            "bnez t0, .kloop%=\n\t"
        ".klast%=:\n\t"
            "vfmacc.vf v1, f2, v29\n\t"
            "vfmacc.vf v2, f2, v30\n\t"
            "vfmacc.vf v3, f3, v29\n\t"
            "vfmacc.vf v4, f3, v30\n\t"
            "vle64.v v31, (%[ptr11])\n\t"
            "vle64.v v0, (t1)\n\t"
            "vfmacc.vf v5, f4, v29\n\t"
            "vfmacc.vf v6, f4, v30\n\t"
            "fld f19, 24(%[ptr21])\n\t"
            "vfmacc.vf v7, f5, v29\n\t"
            "fld f18, 16(%[ptr21])\n\t"
            "vfmacc.vf v8, f5, v30\n\t"
            "vfmacc.vf v9, f6, v29\n\t"
            "fld f16, 0(%[ptr21])\n\t"
            "vfmacc.vf v10, f6, v30\n\t"
            "vfmacc.vf v11, f7, v29\n\t"
            "fld f20, 32(%[ptr21])\n\t"
            "vfmacc.vf v12, f7, v30\n\t"
            "fld f21, 40(%[ptr21])\n\t"
            "vfmacc.vf v13, f8, v29\n\t"
            "add %[ptr11], %[ptr11], %[unroll_vlenxn]\n\t"
            "vfmacc.vf v14, f8, v30\n\t"
            "fld f22, 48(%[ptr21])\n\t"
            "vfmacc.vf v15, f9, v29\n\t"
            "fld f17, 8(%[ptr21])\n\t"
            "add t1, t1, %[unroll_vlenxn]\n\t"
            "vfmacc.vf v16, f9, v30\n\t"
            "fld f23, 56(%[ptr21])\n\t"
            "vfmacc.vf v17, f10, v29\n\t"
            "vfmacc.vf v18, f10, v30\n\t"
            "fld f24, 64(%[ptr21])\n\t"
            "vfmacc.vf v19, f11, v29\n\t"
            "vfmacc.vf v20, f11, v30\n\t"
            "fld f25, 72(%[ptr21])\n\t"
            "add t2, t2, 224\n\t"
            "vfmacc.vf v21, f12, v29\n\t"
            "vfmacc.vf v22, f12, v30\n\t"
            "fld f26, 80(%[ptr21])\n\t"
            "vfmacc.vf v23, f13, v29\n\t"
            "vfmacc.vf v24, f13, v30\n\t"
            "fld f27, 88(%[ptr21])\n\t"
            "vfmacc.vf v25, f14, v29\n\t"
            "nop\n\t"
            "vfmacc.vf v26, f14, v30\n\t"
            "fld f28, 96(%[ptr21])\n\t"
            "vfmacc.vf v27, f15, v29\n\t"
            "vfmacc.vf v28, f15, v30\n\t"
            "fld f29, 104(%[ptr21])\n\t"
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
//            "add t0,t0,-1\n\t"
        ".kdone%=:\n\t"
        // move remainder into counter
        "mv t0,%[k]\n\t"
        "beq t0, zero, .k1done%=\n\t"
        ".k1loop%=:\n\t"
            "vle64.v v29, (%[ptr11])\n\t"
            "vle64.v v30, (t1)\n\t"
            "fld f2, 0(t2)\n\t"
            "fld f3, 8(t2)\n\t"
            "fld f4, 16(t2)\n\t"
            "vfmacc.vf v1, f2, v29\n\t"
            "vfmacc.vf v2, f2, v30\n\t"
            "fld f5, 24(t2)\n\t"
            "vfmacc.vf v3, f3, v29\n\t"
            "vfmacc.vf v4, f3, v30\n\t"
            "fld f6, 32(t2)\n\t"
            "vfmacc.vf v5, f4, v29\n\t"
            "vfmacc.vf v6, f4, v30\n\t"
            "fld f7, 40(t2)\n\t"
            "vfmacc.vf v7, f5, v29\n\t"
            "vfmacc.vf v8, f5, v30\n\t"
            "fld f8, 48(t2)\n\t"
            "vfmacc.vf v9, f6, v29\n\t"
            "add %[ptr11], %[ptr11], %[unroll_vlenxn]\n\t"
            "vfmacc.vf v10, f6, v30\n\t"
            "fld f9, 56(t2)\n\t"
            "vfmacc.vf v11, f7, v29\n\t"
            "add t1, t1, %[unroll_vlenxn]\n\t"
            "vfmacc.vf v12, f7, v30\n\t"
            "fld f10, 64(t2)\n\t"
            "vfmacc.vf v13, f8, v29\n\t"
            "vfmacc.vf v14, f8, v30\n\t"
            "fld f11, 72(t2)\n\t"
            "vfmacc.vf v15, f9, v29\n\t"
            "vfmacc.vf v16, f9, v30\n\t"
            "fld f12, 80(t2)\n\t"
            "vfmacc.vf v17, f10, v29\n\t"
            "vfmacc.vf v18, f10, v30\n\t"
            "fld f13, 88(t2)\n\t"
            "vfmacc.vf v19, f11, v29\n\t"
            "vfmacc.vf v20, f11, v30\n\t"
            "fld f14, 96(t2)\n\t"
            "vfmacc.vf v21, f12, v29\n\t"
            "add t0,t0,-1\n\t"
            "vfmacc.vf v22, f12, v30\n\t"
            "fld f15, 104(t2)\n\t"
            "vfmacc.vf v23, f13, v29\n\t"
            "vfmacc.vf v24, f13, v30\n\t"
            "add t2, t2, 112\n\t"
            "vfmacc.vf v25, f14, v29\n\t"
            "vfmacc.vf v26, f14, v30\n\t"
            "vfmacc.vf v27, f15, v29\n\t"
            "vfmacc.vf v28, f15, v30\n\t"
            "bnez t0, .k1loop%=\n\t"
        ".k1done%=:\n\t"


        "prefetch.r 0(%[anext_cstride])\n\t"
        "prefetch.r 0(%[bnext_vdown])\n\t"

        "add %[ptr11], %[b11], 0\n\t"   // load: B11
                               // From KernelsHowTo.md:        
                               // B11 is stored by rows with   
                               // leading dimension PACKNR,    
                               // where typically PACKNR = NR. 
                               //                              
        "li  %[anext_cstride], 14\n\t"       // PACKNR                       
        "mul %[unroll_vlenxn], %[vlen], %[anext_cstride]\n\t"   // vlen*PACKNR*sizeof(double)   
        "slli %[anext_cstride], %[anext_cstride], 3\n\t"   // PACKNR*sizeof(double)
        "add t1, %[ptr11], %[unroll_vlenxn]\n\t"   // load: B11+vlen*PACKNR        

        // From KernelsHowTo.md:
        // Zero alpha. The microkernel can safely assume that alpha is non-zero;
        // "alpha equals zero" handling is performed at a much higher level,
        // which means that, in such a scenario, the microkernel will never get called.
        //"bnez t0, .alphazero%=\n\t"
        ".alphascale%=:\n\t" // TODO: written manually and unoptimized
            
            "vlse64.v v29, (%[ptr11]), %[anext_cstride]\n\t"  // c0_1
            "vlse64.v v30, (t1), %[anext_cstride]\n\t"  // c0_2
            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (%[ptr11]), %[anext_cstride]\n\t"  // c1_1
            "vlse64.v v0, (t1), %[anext_cstride]\n\t"  // c1_2
            "vfmsac.vf v1,  f1, v29\n\t" // c0_1
            "vfmsac.vf v2,  f1, v30\n\t" // c0_2

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (%[ptr11]), %[anext_cstride]\n\t" // c2_1
            "vlse64.v v30, (t1), %[anext_cstride]\n\t" // c2_2
            "vfmsac.vf v3,  f1, v31\n\t"
            "vfmsac.vf v4,  f1, v0\n\t"


            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v0, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v5,  f1, v29\n\t"
            "vfmsac.vf v6,  f1, v30\n\t"

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v30, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v7,  f1, v31\n\t"
            "vfmsac.vf v8,  f1, v0\n\t"

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v0, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v9,  f1, v29\n\t"
            "vfmsac.vf v10,  f1, v30\n\t"

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v30, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v11,  f1, v31\n\t"
            "vfmsac.vf v12,  f1, v0\n\t"

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v0, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v13,  f1, v29\n\t"
            "vfmsac.vf v14,  f1, v30\n\t"

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v30, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v15,  f1, v31\n\t"
            "vfmsac.vf v16,  f1, v0\n\t"

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v0, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v17,  f1, v29\n\t"
            "vfmsac.vf v18,  f1, v30\n\t"

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v30, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v19,  f1, v31\n\t"
            "vfmsac.vf v20,  f1, v0\n\t"

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v0, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v21,  f1, v29\n\t"
            "vfmsac.vf v22,  f1, v30\n\t"

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v29, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v30, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v23,  f1, v31\n\t"
            "vfmsac.vf v24,  f1, v0\n\t"

            "add %[ptr11], %[ptr11], 8\n\t"
            "add t1, t1, 8\n\t"
            "vlse64.v v31, (%[ptr11]), %[anext_cstride]\n\t"
            "vlse64.v v0, (t1), %[anext_cstride]\n\t"
            "vfmsac.vf v25,  f1, v29\n\t"
            "vfmsac.vf v26,  f1, v30\n\t"

            "vfmsac.vf v27,  f1, v31\n\t"
            "vfmsac.vf v28,  f1, v0\n\t"

                
        // ================= TRSM ===============
        // b11 is in v1-v28 (we need v0 for masking)
        //
        // we have v29-v31 for aiv[12]
        //
        // all f0-f31 are free
        //
        //
        // pointers
        "add %[ptr11], %[a11], 0\n\t"  // load: a11
        "add t1, %[ptr11], %[vlen]\n\t"  // load: a11 + vlen
        // offsets
        "slli t2, %[vlen], 1\n\t"  // 2*vlen*sizeof(double)
        // counter
        "srli t0, %[vlen], 2\n\t"  // loop counter: vlen*2
                              // (vlen*sizeof(double)/4)
        "srli %[unroll_vlenxn], t0, 1\n\t"  // vlen in elements
        // vector mask for vle and vfnmsac

        "add %[bnext_vdown], %[unroll_vlenxn], -1\n\t"
        
        // b1         |    b1
        // 1   -> f   |    2
        // 2          | -> 3
        // 3          |    4
        // 4          |    0
        //
        //
        
        "vfmv.f.s f14, v1\n\t"
        "vfmv.f.s f15, v3\n\t"
        "vslide1down.vx v1, v1, zero\n\t"
        "vfmv.f.s f16, v5\n\t"
        "vslide1down.vx v3, v3, zero\n\t"
        "vfmv.f.s f17, v7\n\t"
        "vslide1down.vx v5, v5, zero\n\t"
        "vfmv.f.s f18, v9\n\t"
        "vslide1down.vx v7, v7, zero\n\t"
        "vfmv.f.s f19, v11\n\t"
        "vslide1down.vx v9, v9, zero\n\t"
        "vfmv.f.s f20, v13\n\t"
        "vslide1down.vx v11, v11, zero\n\t"
        "vfmv.f.s f21, v15\n\t"
        "vslide1down.vx v13, v13, zero\n\t"
        "vfmv.f.s f22, v17\n\t"
        "vslide1down.vx v15, v15, zero\n\t"
        "vfmv.f.s f23, v19\n\t"
        "vslide1down.vx v17, v17, zero\n\t"
        "vfmv.f.s f24, v21\n\t"
        "vslide1down.vx v19, v19, zero\n\t"
        "vfmv.f.s f25, v23\n\t"
        "vslide1down.vx v21, v21, zero\n\t"
        "vfmv.f.s f26, v25\n\t"
        "vslide1down.vx v23, v23, zero\n\t"
        "vfmv.f.s f27, v27\n\t"
        "vslide1down.vx v25, v25, zero\n\t"
        "vslide1down.vx v27, v27, zero\n\t"
        ".trsmv1%=:\n\t"

            "vsetvli %[vlen], %[bnext_vdown], e64, m1, ta, ma\n\t"
            "add %[ptr21], %[c11], 0\n\t"  // fstore: c11

            // f28 < 1/aii
            "fld f28, 0(%[ptr11])\n\t"
            #ifdef BLIS_ENABLE_TRSM_PREINVERSION
                // Nothing
            #else
                // we don't need k anymore so reuse it
                "li %[k], 1\n\t"
                "fcvt.d.l f1, %[k]\n\t" // 1.0
                "fdiv.d f28, f1, f28\n\t" // 1/a11
            #endif
            "fmul.d f0, f28, f14\n\t"
            "add %[ptr11], %[ptr11], 8\n\t" // next row
            "fmul.d f1, f28, f15\n\t"
            "fmul.d f2, f28, f16\n\t"
            "vle64.v v29, (%[ptr11])\n\t"
            "fmul.d f3, f28, f17\n\t"
            "fmul.d f4, f28, f18\n\t"
            "fmul.d f5, f28, f19\n\t"
            "add %[ptr11], %[ptr11], t2\n\t" // next column (next aii)
            "fmul.d f6, f28, f20\n\t"
            "fmul.d f7, f28, f21\n\t"
            "fmul.d f8, f28, f22\n\t"
            "fmul.d f9, f28, f23\n\t"
            "add t0, t0, -1\n\t"
            "fmul.d f10, f28, f24\n\t"
            "fmul.d f11, f28, f25\n\t"
            "fmul.d f12, f28, f26\n\t"
            "fmul.d f13, f28, f27\n\t"

            // From KernelsHowTo.d:
            // Output. This microkernel must write its result to two places: 
            // the submatrix B11 of the current packed micropanel of B and 
            // the submatrix C11 of the output matrix C
            "fsd f0, (%[ptr21])\n\t"
            "fsd f0, 0(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f1, (%[ptr21])\n\t"
            "fsd f1, 8(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f2, (%[ptr21])\n\t"
            "fsd f2, 16(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f3, (%[ptr21])\n\t"
            "fsd f3, 24(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f4, (%[ptr21])\n\t"
            "fsd f4, 32(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f5, (%[ptr21])\n\t"
            "fsd f5, 40(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f6, (%[ptr21])\n\t"
            "fsd f6, 48(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f7, (%[ptr21])\n\t"
            "fsd f7, 56(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f8, (%[ptr21])\n\t"
            "fsd f8, 64(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f9, (%[ptr21])\n\t"
            "fsd f9, 72(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f10, (%[ptr21])\n\t"
            "fsd f10, 80(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f11, (%[ptr21])\n\t"
            "fsd f11, 88(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f12, (%[ptr21])\n\t"
            "fsd f12, 96(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f13, (%[ptr21])\n\t"
            "fsd f13, 104(%[b11])\n\t"

            "add %[c11], %[c11], 8\n\t" // next row in c11
            "add %[b11], %[b11], 112\n\t" // next row in b11

            "beq t0, %[unroll_vlenxn], .trsmv1end%=\n\t"

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

            "vsetvli %[vlen], %[unroll_vlenxn], e64, m1, ta, ma\n\t"

            "vle64.v v30, (t1)\n\t"
            "add t1, t1, t2\n\t" // next column (next aii+vlen)

            "vfnmsac.vf v2,  f0, v30\n\t"
            "vfnmsac.vf v4,  f1, v30\n\t"
            "vfnmsac.vf v6,  f2, v30\n\t"
            "vfnmsac.vf v8,  f3, v30\n\t"
            "vfnmsac.vf v10, f4, v30\n\t"
            "vfnmsac.vf v12, f5, v30\n\t"
            "vfnmsac.vf v14, f6, v30\n\t"
            "vfnmsac.vf v16, f7, v30\n\t"
            "vfnmsac.vf v18, f8, v30\n\t"
            "vfnmsac.vf v20, f9, v30\n\t"
            "vfnmsac.vf v22, f10, v30\n\t"
            "vfnmsac.vf v24, f11, v30\n\t"
            "vfnmsac.vf v26, f12, v30\n\t"
            "vfnmsac.vf v28, f13, v30\n\t"

            "add %[bnext_vdown], %[bnext_vdown], -1\n\t"

            "vfmv.f.s f14, v1\n\t"
            "vfmv.f.s f15, v3\n\t"
            "vslide1down.vx v1, v1, zero\n\t"
            "vfmv.f.s f16, v5\n\t"
            "vslide1down.vx v3, v3, zero\n\t"
            "vfmv.f.s f17, v7\n\t"
            "vslide1down.vx v5, v5, zero\n\t"
            "vfmv.f.s f18, v9\n\t"
            "vslide1down.vx v7, v7, zero\n\t"
            "vfmv.f.s f19, v11\n\t"
            "vslide1down.vx v9, v9, zero\n\t"
            "vfmv.f.s f20, v13\n\t"
            "vslide1down.vx v11, v11, zero\n\t"
            "vfmv.f.s f21, v15\n\t"
            "vslide1down.vx v13, v13, zero\n\t"
            "vfmv.f.s f22, v17\n\t"
            "vslide1down.vx v15, v15, zero\n\t"
            "vfmv.f.s f23, v19\n\t"
            "vslide1down.vx v17, v17, zero\n\t"
            "vfmv.f.s f24, v21\n\t"
            "vslide1down.vx v19, v19, zero\n\t"
            "vfmv.f.s f25, v23\n\t"
            "vslide1down.vx v21, v21, zero\n\t"
            "vfmv.f.s f26, v25\n\t"
            "vslide1down.vx v23, v23, zero\n\t"
            "vfmv.f.s f27, v27\n\t"
            "vslide1down.vx v25, v25, zero\n\t"
            "vslide1down.vx v27, v27, zero\n\t"


            "j .trsmv1%=\n\t"
        ".trsmv1end%=:\n\t"
        
        "vsetvli %[vlen], %[unroll_vlenxn], e64, m1, ta, ma\n\t"

        "vle64.v v30, (t1)\n\t"
        "add t1, t1, t2\n\t" // next column (next aii+vlen)

        "vfnmsac.vf v2, f0, v30\n\t"
        "vfnmsac.vf v4, f1, v30\n\t"
        "vfnmsac.vf v6, f2, v30\n\t"
        "vfnmsac.vf v8, f3, v30\n\t"
        "vfnmsac.vf v10, f4, v30\n\t"
        "vfnmsac.vf v12, f5, v30\n\t"
        "vfnmsac.vf v14, f6, v30\n\t"
        "vfnmsac.vf v16, f7, v30\n\t"
        "vfnmsac.vf v18, f8, v30\n\t"
        "vfnmsac.vf v20, f9, v30\n\t"
        "vfnmsac.vf v22, f10, v30\n\t"
        "vfnmsac.vf v24, f11, v30\n\t"
        "vfnmsac.vf v26, f12, v30\n\t"
        "vfnmsac.vf v28, f13, v30\n\t"

        "add %[bnext_vdown], %[unroll_vlenxn], -1\n\t"

        "vfmv.f.s f14, v2\n\t"
        "vfmv.f.s f15, v4\n\t"
        "vslide1down.vx v2, v2, zero\n\t"
        "vfmv.f.s f16, v6\n\t"
        "vslide1down.vx v4, v4, zero\n\t"
        "vfmv.f.s f17, v8\n\t"
        "vslide1down.vx v6, v6, zero\n\t"
        "vfmv.f.s f18, v10\n\t"
        "vslide1down.vx v8, v8, zero\n\t"
        "vfmv.f.s f19, v12\n\t"
        "vslide1down.vx v10, v10, zero\n\t"
        "vfmv.f.s f20, v14\n\t"
        "vslide1down.vx v12, v12, zero\n\t"
        "vfmv.f.s f21, v16\n\t"
        "vslide1down.vx v14, v14, zero\n\t"
        "vfmv.f.s f22, v18\n\t"
        "vslide1down.vx v16, v16, zero\n\t"
        "vfmv.f.s f23, v20\n\t"
        "vslide1down.vx v18, v18, zero\n\t"
        "vfmv.f.s f24, v22\n\t"
        "vslide1down.vx v20, v20, zero\n\t"
        "vfmv.f.s f25, v24\n\t"
        "vslide1down.vx v22, v22, zero\n\t"
        "vfmv.f.s f26, v26\n\t"
        "vslide1down.vx v24, v24, zero\n\t"
        "vfmv.f.s f27, v28\n\t"
        "vslide1down.vx v26, v26, zero\n\t"
        "vslide1down.vx v28, v28, zero\n\t"
        ".trsmv2%=:\n\t"
            "vsetvli %[vlen], %[bnext_vdown], e64, m1, ta, ma\n\t"

            "add %[ptr21], %[c11], 0\n\t"  // fstore: c11

            "fld f28, 0(%[ptr11])\n\t"
            #ifdef BLIS_ENABLE_TRSM_PREINVERSION
                // Nothing
            #else
                // we don't need k anymore so reuse it
                "li %[k], 1\n\t"
                "fcvt.d.l f1, %[k]\n\t" // 1.0
                "fdiv.d f28, f1, f28\n\t" // 1/a11
            #endif
            "fmul.d f0, f28, f14\n\t"
            "add %[ptr11], %[ptr11], 8\n\t" // next row
            "fmul.d f1, f28, f15\n\t"
            "fmul.d f2, f28, f16\n\t"
            "vle64.v v30, (%[ptr11])\n\t"
            "fmul.d f3, f28, f17\n\t"
            "fmul.d f4, f28, f18\n\t"
            "fmul.d f5, f28, f19\n\t"
            "add %[ptr11], %[ptr11], t2\n\t" // next column
            "fmul.d f6, f28, f20\n\t"
            "fmul.d f7, f28, f21\n\t"
            "fmul.d f8, f28, f22\n\t"
            "fmul.d f9, f28, f23\n\t"
            "add t0, t0, -1\n\t"
            "fmul.d f10, f28, f24\n\t"
            "fmul.d f11, f28, f25\n\t"
            "fmul.d f12, f28, f26\n\t"
            "fmul.d f13, f28, f27\n\t"

            "fsd f0, (%[ptr21])\n\t"
            "fsd f0, 0(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f1, (%[ptr21])\n\t"
            "fsd f1, 8(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f2, (%[ptr21])\n\t"
            "fsd f2, 16(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f3, (%[ptr21])\n\t"
            "fsd f3, 24(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f4, (%[ptr21])\n\t"
            "fsd f4, 32(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f5, (%[ptr21])\n\t"
            "fsd f5, 40(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f6, (%[ptr21])\n\t"
            "fsd f6, 48(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f7, (%[ptr21])\n\t"
            "fsd f7, 56(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f8, (%[ptr21])\n\t"
            "fsd f8, 64(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f9, (%[ptr21])\n\t"
            "fsd f9, 72(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f10, (%[ptr21])\n\t"
            "fsd f10, 80(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f11, (%[ptr21])\n\t"
            "fsd f11, 88(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f12, (%[ptr21])\n\t"
            "fsd f12, 96(%[b11])\n\t"
            "add %[ptr21], %[ptr21], %[cs_c]\n\t"
            "fsd f13, (%[ptr21])\n\t"
            "fsd f13, 104(%[b11])\n\t"

            "add %[c11], %[c11], 8\n\t" // next row in c11
            "add %[b11], %[b11], 112\n\t" // next row in b11

            "beq t0, zero, .trsmv2end%=\n\t"

            "vfnmsac.vf v2, f0, v30\n\t"
            "vfnmsac.vf v4, f1, v30\n\t"
            "vfnmsac.vf v6, f2, v30\n\t"
            "vfnmsac.vf v8, f3, v30\n\t"
            "vfnmsac.vf v10, f4, v30\n\t"
            "vfnmsac.vf v12, f5, v30\n\t"
            "vfnmsac.vf v14, f6, v30\n\t"
            "vfnmsac.vf v16, f7, v30\n\t"
            "vfnmsac.vf v18, f8, v30\n\t"
            "vfnmsac.vf v20, f9, v30\n\t"
            "vfnmsac.vf v22, f10, v30\n\t"
            "vfnmsac.vf v24, f11, v30\n\t"
            "vfnmsac.vf v26, f12, v30\n\t"
            "vfnmsac.vf v28, f13, v30\n\t"

            "add %[bnext_vdown], %[bnext_vdown], -1\n\t"

            "vfmv.f.s f14, v2\n\t"
            "vfmv.f.s f15, v4\n\t"
            "vslide1down.vx v2, v2, zero\n\t"
            "vfmv.f.s f16, v6\n\t"
            "vslide1down.vx v4, v4, zero\n\t"
            "vfmv.f.s f17, v8\n\t"
            "vslide1down.vx v6, v6, zero\n\t"
            "vfmv.f.s f18, v10\n\t"
            "vslide1down.vx v8, v8, zero\n\t"
            "vfmv.f.s f19, v12\n\t"
            "vslide1down.vx v10, v10, zero\n\t"
            "vfmv.f.s f20, v14\n\t"
            "vslide1down.vx v12, v12, zero\n\t"
            "vfmv.f.s f21, v16\n\t"
            "vslide1down.vx v14, v14, zero\n\t"
            "vfmv.f.s f22, v18\n\t"
            "vslide1down.vx v16, v16, zero\n\t"
            "vfmv.f.s f23, v20\n\t"
            "vslide1down.vx v18, v18, zero\n\t"
            "vfmv.f.s f24, v22\n\t"
            "vslide1down.vx v20, v20, zero\n\t"
            "vfmv.f.s f25, v24\n\t"
            "vslide1down.vx v22, v22, zero\n\t"
            "vfmv.f.s f26, v26\n\t"
            "vslide1down.vx v24, v24, zero\n\t"
            "vfmv.f.s f27, v28\n\t"
            "vslide1down.vx v26, v26, zero\n\t"
            "vslide1down.vx v28, v28, zero\n\t"


            "j .trsmv2%=\n\t"
        ".trsmv2end%=:\n\t"

        : [dummy_c] "+m"(*(double(*)[])c11), [dummy_b] "+m"(*(double(*)[])b11),
          [c11] "+r" (c11), [b11] "+r" (b11),
          [ptr11] "+r" (ptr11), //[ptr12] "=r" (ptr12),
          [ptr21] "+r" (ptr21), //[ptr22] "=r" (ptr22),
          [anext_cstride] "+r" (anext_cstride), [bnext_vdown] "+r" (bnext_vdown),
          [vlen] "+r" (vlen), [unroll_vlenxn] "+r" (unroll_vlenxn),
          [k] "+r" (k),
          [alpha_pref_ptr] "+r" (alpha_pref_ptr), [rs_c_pref_dist] "+r" (rs_c_pref_dist),
          [cs_c] "+r" (cs_c)
        : [a11] "r" (a11) 
        : "t0", "t1", "t2",
          "f0","f1","f2","f3","f4","f5","f6","f7",
          "f8","f9","f10","f11","f12","f13","f14","f15",
          "f16","f17","f18","f19","f20","f21","f22","f23",
          "f24","f25","f26","f27","f28","f29",
          "v0", "v1","v2","v3","v4","v5","v6","v7","v8",
          "v9","v10","v11","v12","v13","v14","v15","v16",
          "v17","v18","v19","v20","v21","v22","v23","v24",
          "v25","v26","v27","v28","v29","v30","v31"
    );

    GEMMTRSM_UKR_FLUSH_CT(d);
}
