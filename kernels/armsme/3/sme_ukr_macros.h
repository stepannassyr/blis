/*=============================================================================
  sme_ukr_macros.h

  Data-type-generic building blocks for SME gemm micro-kernels.

  A kernel is described by two numbers:

      MR = 2      * SVL_<dt>          always two row halves
      NR = 2*nblk * SVL_<dt>          nblk blocks of two column chunks

  and the accumulator tiles are laid out as  t = i + 2*j,  i in {0,1} the row
  half, j in {0 .. 2*nblk-1} the column chunk.  Block b therefore owns tiles
  4b..4b+3 and column chunks 2b, 2b+1, which is the 4-instruction chunk the
  store macros are built from.

      dt=d, nblk=2   ->  2V x 4V,  8 tiles  (all of ZA at .D)
      dt=d, nblk=1   ->  2V x 2V,  4 tiles
      dt=s, nblk=1   ->  2V x 2V,  4 tiles  (all of ZA at .S)

  ---------------------------------------------------------------------------
  WHY THE INNER MACROS EXIST

  .altmacro's %expr substitution happens while a *macro argument* is being
  parsed.  It does nothing inside an ordinary instruction line, so

        mova z%z_idx.\dt, ...                  <-- does not assemble

  Register and tile numbers are part of the operand name, not an expression
  field, so anything computed has to arrive as a macro argument.  Hence the
  SME_* primitives below: they take plain literals, and callers hand them
  %-evaluated expressions.  Immediate fields are different -- slice offsets,
  MUL VL indices and shift amounts are ordinary absolute expressions, so
  .set symbols can be dropped straight into them with no %.

  ---------------------------------------------------------------------------
  SLICE UNROLLING

  The MOVA slice index is Ws + imm, with imm limited to 16/sizeof(elem)-1:

        .b 0-15    .h 0-7    .s 0-3    .d 0-1

  SVL is a multiple of 16 bytes, so the slice count SVL_<dt> is always an
  exact multiple of that immediate range -- the row loop unrolls by
  .LSME_NSLICE with no remainder, for every element size.

  ---------------------------------------------------------------------------
  REGISTER CONVENTION (shared by all macros here)

      x7  c              x9  rs_c (bytes)     x10 cs_c (bytes)
      x14 SVL_<dt>       w12 ZA slice index   x13 loop counter
      x15 &C[0][*]       x8  &C[SVL][*]       x11 scratch
      x3,x4,x5,x6        column pointers (STORE_COLS only)

      z0 -z15  accumulator read-out     z16-z23  C values
      z24 alpha (bcast)  z25 beta (bcast)  z26-z29 gather offsets  z30 tmp

  .altmacro is enabled below and stays on for the rest of the translation
  unit; note that it also makes <> string delimiters and ! an escape.
=============================================================================*/

#ifndef SME_UKR_MACROS_H
#define SME_UKR_MACROS_H

    .altmacro

/*---------------------------------------------------------------------------
  PSTATE ownership.

  PSTATE.SM and PSTATE.ZA are independent bits, and either may be toggled by
  the ukr or enabled once further out by bli_gemm_ker_var2.c.  The six
  BLIS_SME_HOIST_* variants are derived here into two flags; keep this block
  in sync with the derivation at the top of bli_gemm_ker_var2.c.

  Whatever the ukr does NOT own must already be enabled on entry, or the
  first access traps.
---------------------------------------------------------------------------*/
#ifndef BLIS_SME_SM_AT
#define BLIS_SME_SM_AT 0
#endif
#ifndef BLIS_SME_ZA_AT
#define BLIS_SME_ZA_AT 0
#endif

#if BLIS_SME_SM_AT == 0
#define SME_UKR_OWNS_SM 1
#else
#define SME_UKR_OWNS_SM 0
#endif

#if BLIS_SME_ZA_AT == 0
#define SME_UKR_OWNS_ZA 1
#else
#define SME_UKR_OWNS_ZA 0
#endif

/* Bytes of callee-saved FP spill that UKR_SME_INIT pushes.  The incoming
   stack arguments sit above it; see the offsets in UKR_SME_INIT. */
    .set    .LSME_SPILL,    64

/*---------------------------------------------------------------------------
  Type table.  Call once per kernel before anything else.
---------------------------------------------------------------------------*/
.macro SME_DT_SETUP dt
    .set    .LSME_ES,       0   // element size, bytes
    .set    .LSME_ESH,  0   // log2(element size)
    .set    .LSME_NSLICE,   0   // slices reachable by the MOVA immediate
    .set    .LSME_NSLICE_LOG2, 0
  .ifc \dt, d
    .set    .LSME_ES,       8
    .set    .LSME_ESH,  3
    .set    .LSME_NSLICE,   2
    .set    .LSME_NSLICE_LOG2, 1
  .endif
  .ifc \dt, s
    .set    .LSME_ES,       4
    .set    .LSME_ESH,  2
    .set    .LSME_NSLICE,   4
    .set    .LSME_NSLICE_LOG2, 2
  .endif
  .if .LSME_ES == 0
    .error  "SME_DT_SETUP: dt must be d or s"
  .endif
.endm

/*---------------------------------------------------------------------------
  Primitives.  Every index argument must arrive as a literal.
---------------------------------------------------------------------------*/
/* x\xd == 0  iff  *\xb == +-0.0.  Reads exactly one element: a d-sized load
   of an fp32 beta would run off the end of the caller's scalar. */
.macro SME_BETA_ZERO_TEST xd, xb, dt
  .ifc \dt, d
    ldr x\xd, [\xb]
    lsl x\xd, x\xd, #1              // drop the sign bit
  .endif
  .ifc \dt, s
    ldr w\xd, [\xb]
    lsl w\xd, w\xd, #1              // (w-write zero-extends)
  .endif
.endm

.macro SME_MOVA_H zd, t, off, dt            // tile row -> vector
    mova    z\zd\().\dt, p0/m, za\t\()h.\dt[w12, \off]
.endm

.macro SME_MOVA_V zd, t, off, dt            // tile col -> vector
    mova    z\zd\().\dt, p0/m, za\t\()v.\dt[w12, \off]
.endm

.macro SME_LD zd, pg, xb, vl, dt            // contiguous load
  .ifc \dt, d
    ld1d    {z\zd\().d}, \pg/z, [\xb, #\vl, MUL VL]
  .endif
  .ifc \dt, s
    ld1w    {z\zd\().s}, \pg/z, [\xb, #\vl, MUL VL]
  .endif
.endm

.macro SME_ST zd, pg, xb, vl, dt            // contiguous store
  .ifc \dt, d
    st1d    {z\zd\().d}, \pg, [\xb, #\vl, MUL VL]
  .endif
  .ifc \dt, s
    st1w    {z\zd\().s}, \pg, [\xb, #\vl, MUL VL]
  .endif
.endm

/* Scalar+vector.  NOTE: a 32-bit element gather takes 32-bit offsets and the
   UXTW modifier is mandatory -- [Xn, Zm.s] alone is rejected.  That caps the
   reachable span at 4 GiB from the base, i.e. cs_c*4*(NR-1) < 2^32.
   Also remember these are illegal in streaming mode without FEAT_SME_FA64. */
.macro SME_LD_GS zd, pg, xb, zo, dt
  .ifc \dt, d
    ld1d    {z\zd\().d}, \pg/z, [\xb, z\zo\().d]
  .endif
  .ifc \dt, s
    ld1w    {z\zd\().s}, \pg/z, [\xb, z\zo\().s, uxtw]
  .endif
.endm

.macro SME_ST_GS zd, pg, xb, zo, dt
  .ifc \dt, d
    st1d    {z\zd\().d}, \pg, [\xb, z\zo\().d]
  .endif
  .ifc \dt, s
    st1w    {z\zd\().s}, \pg, [\xb, z\zo\().s, uxtw]
  .endif
.endm

.macro SME_LD1R zd, xb, dt              // broadcast a scalar
  .ifc \dt, d
    ld1rd   {z\zd\().d}, p0/z, [\xb]
  .endif
  .ifc \dt, s
    ld1rw   {z\zd\().s}, p0/z, [\xb]
  .endif
.endm

.macro SME_CNT xd, dt                   // SVL in elements
  .ifc \dt, d
    cntd    \xd
  .endif
  .ifc \dt, s
    cntw    \xd
  .endif
.endm

.macro SME_WHILELT pd, xa, xb, dt
    whilelt \pd\().\dt, \xa, \xb
.endm

.macro SME_FMOPA t, pa, pb, za_, zb_, dt
    fmopa   za\t\().\dt, \pa/m, \pb/m, z\za_\().\dt, z\zb_\().\dt
.endm

.macro SME_ZERO_TILE t, dt
    zero    {za\t\().\dt}
.endm

/* x\xd == 0  iff  *\xb == 1.0.  Unlike the zero test there is no sign
   ambiguity: -1.0 is 0xBFF0.. and correctly reads as "not one".        */
.macro SME_FP_ONE_TEST xd, xb, dt
  .ifc \dt, d
    ldr x\xd, [\xb]
    eor x\xd, x\xd, #0x3FF0000000000000
  .endif
  .ifc \dt, s
    ldr w\xd, [\xb]
    eor w\xd, w\xd, #0x3F800000
  .endif
.endm

/* bmode 0 general: C := beta*C + alpha*acc
   bmode 1 beta==0: C := alpha*acc
   bmode 2 beta==1: C := C + alpha*acc                                   */
.macro SME_SCALE_MUL zc, zacc, bmode, dt
  .if \bmode == 1
    fmul    z\zc\().\dt, z\zacc\().\dt, z24.\dt
  .elseif \bmode == 2
                    /* nothing: the fmla does it all */
  .else
    fmul    z\zc\().\dt, z\zc\().\dt, z25.\dt
  .endif
.endm

.macro SME_SCALE_MLA zc, zacc, bmode, dt
  .if \bmode != 1
    fmla    z\zc\().\dt, p0/m, z\zacc\().\dt, z24.\dt
  .endif
.endm

/*---------------------------------------------------------------------------
  DIRECT ZA <-> C  (alpha == 1 and beta in {0,1} only)

  Verified against binutils 2.42: the ZA tile-slice LD1D/ST1D forms accept
  only [Xn] and [Xn, Xm, LSL #esh] -- there is NO "#imm, MUL VL" form.  Xm is
  an ELEMENT index, so column chunk j needs a register holding j*SVL.
  Slice-immediate range and the w12-w15 selector constraint are identical to
  MOVA, so the traversal below is the same shape as SME_STORE_ROWS/COLS.
---------------------------------------------------------------------------*/
/* Branch to \lbl unless C is unit-stride in one dimension.               */
.macro SME_IF_STRIDED lbl
    cmp x10, #.LSME_ES
    ccmp    x9,  #.LSME_ES, #4, ne
    b.ne    \lbl
.endm

.macro SME_ZA_LDST_H dir, t, xb, xo, sl, dt
  .ifc \dt, d
    .if \dir
    ld1d    {za\t\()h.d[w12, \sl]}, p0/z, [\xb, x\xo, lsl #3]
    .else
    st1d    {za\t\()h.d[w12, \sl]}, p0,   [\xb, x\xo, lsl #3]
    .endif
  .endif
  .ifc \dt, s
    .if \dir
    ld1w    {za\t\()h.s[w12, \sl]}, p0/z, [\xb, x\xo, lsl #2]
    .else
    st1w    {za\t\()h.s[w12, \sl]}, p0,   [\xb, x\xo, lsl #2]
    .endif
  .endif
.endm

.macro SME_ZA_LDST_V dir, t, xb, xo, sl, dt
  .ifc \dt, d
    .if \dir
    ld1d    {za\t\()v.d[w12, \sl]}, p0/z, [\xb, x\xo, lsl #3]
    .else
    st1d    {za\t\()v.d[w12, \sl]}, p0,   [\xb, x\xo, lsl #3]
    .endif
  .endif
  .ifc \dt, s
    .if \dir
    ld1w    {za\t\()v.s[w12, \sl]}, p0/z, [\xb, x\xo, lsl #2]
    .else
    st1w    {za\t\()v.s[w12, \sl]}, p0,   [\xb, x\xo, lsl #2]
    .endif
  .endif
.endm

/* index registers: x3..x6 hold {0, SVL, 2*SVL, 3*SVL} elements (chunk j) */
.macro SME_ZA_XOFF nblk
    mov x3, #0
    mov x4, x14
    .if \nblk > 1
    lsl x5, x14, #1
    add x6, x5, x14
    .endif
.endm

.macro SME_ZA_ROWS_GROUP dir, tb, vl0, sl, dt
    SME_ZA_LDST_H \dir, %(\tb+0), x15, %(3+\vl0),   \sl, \dt
    SME_ZA_LDST_H \dir, %(\tb+2), x15, %(3+\vl0+1), \sl, \dt
    SME_ZA_LDST_H \dir, %(\tb+1), x8,  %(3+\vl0),   \sl, \dt
    SME_ZA_LDST_H \dir, %(\tb+3), x8,  %(3+\vl0+1), \sl, \dt
.endm

/* dir 0 = ZA -> C (the only direction used), dir 1 = C -> ZA.  cs_c == 1.
   dir 1 is assembled-correct but not wired up; see the note in the .S.    */
.macro SME_ZA_ROWS dir, dt, nblk
    SME_ZA_XOFF \nblk
    mov x15, x7
    madd    x8, x14, x9, x7
    mov w12, #0
    lsr x13, x14, #.LSME_NSLICE_LOG2
.Lzr\@:
    .set    .L_sl, 0
    .rept   .LSME_NSLICE
      .set  .L_sb, 0
      .rept \nblk
        SME_ZA_ROWS_GROUP \dir, %(4*.L_sb), %(2*.L_sb), %.L_sl, \dt
        .set .L_sb, .L_sb+1
      .endr
    add x15, x15, x9
    add x8,  x8,  x9
      .set  .L_sl, .L_sl+1
    .endr
    add w12, w12, #.LSME_NSLICE
    subs    x13, x13, #1
    b.ne    .Lzr\@
.endm

/* rs_c == 1.  Chunk pointers x3..x6, row-half element offsets in x11/x17. */
.macro SME_ZA_COLS_GROUP dir, tb, xb, sl, dt
    SME_ZA_LDST_V \dir, %(\tb+0), \xb, 8,  \sl, \dt
    SME_ZA_LDST_V \dir, %(\tb+1), \xb, 11, \sl, \dt
.endm

.macro SME_ZA_COLS dir, dt, nblk
    mov x8,  #0                 // row half 0 element offset
    mov x11, x14                // row half 1 element offset
    mov x3, x7
    madd    x4, x14, x10, x3
    .if \nblk > 1
    madd    x5, x14, x10, x4
    madd    x6, x14, x10, x5
    .endif
    mov w12, #0
    lsr x13, x14, #.LSME_NSLICE_LOG2
.Lzc\@:
    .set    .L_sl, 0
    .rept   .LSME_NSLICE
        SME_ZA_COLS_GROUP \dir, 0, x3, %.L_sl, \dt
        SME_ZA_COLS_GROUP \dir, 2, x4, %.L_sl, \dt
      .if \nblk > 1
        SME_ZA_COLS_GROUP \dir, 4, x5, %.L_sl, \dt
        SME_ZA_COLS_GROUP \dir, 6, x6, %.L_sl, \dt
      .endif
    add x3, x3, x10
    add x4, x4, x10
      .if \nblk > 1
    add x5, x5, x10
    add x6, x6, x10
      .endif
      .set  .L_sl, .L_sl+1
    .endr
    add w12, w12, #.LSME_NSLICE
    subs    x13, x13, #1
    b.ne    .Lzc\@
.endm

/*===========================================================================
  FUNCTION INTRO / OUTRO

  UKR_SME_INIT dt
      - spills d8-d15 (SMSTART/SMSTOP zero Z, and v8-v15 low halves are
        callee-saved under the base PCS regardless of who owns SM)
      - loads rs_c / cs_c from the incoming stack args and converts to bytes
      - enters whichever PSTATE bits the ukr owns
      - clears ZA when it does NOT own ZA, since the tile then persists
        across calls and FMOPA only accumulates
      - sets up p0 and x14 = SVL in elements

      On exit: x9 rs_c bytes, x10 cs_c bytes, x14 SVL_<dt>, p0 all-true.
      Stack args, valid until UKR_SME_DEINIT:
          [sp, #.LSME_SPILL+0]  rs_c     [sp, #.LSME_SPILL+8]  cs_c
          [sp, #.LSME_SPILL+16] data     [sp, #.LSME_SPILL+24] cntx

  UKR_SME_DEINIT
      Mirror image.  Does not emit "ret" -- keep that visible in the kernel.

  Ordering is load-bearing in both directions: the spill must precede
  SMSTART and the restore must follow SMSTOP, because each transition zeroes
  the Z registers that d8-d15 alias.
===========================================================================*/
.macro UKR_SME_INIT dt
    stp d8,  d9,  [sp, #-.LSME_SPILL]!
    stp d10, d11, [sp, #16]
    stp d12, d13, [sp, #32]
    stp d14, d15, [sp, #48]

    ldr x9,  [sp, #(.LSME_SPILL + 0)]   // rs_c
    ldr x10, [sp, #(.LSME_SPILL + 8)]   // cs_c
    lsl x9,  x9,  #.LSME_ESH        // -> bytes
    lsl x10, x10, #.LSME_ESH

#if SME_UKR_OWNS_SM && SME_UKR_OWNS_ZA
    smstart                 // PSTATE.SM=1, PSTATE.ZA=1 (ZA zeroed)
#elif SME_UKR_OWNS_SM
    smstart sm              // ZA already enabled by the hoist
#elif SME_UKR_OWNS_ZA
    smstart za              // SM already on; ZA zeroed here
#endif

#if !SME_UKR_OWNS_ZA
    zero    {za}                // ZA persists across calls, so clear the
                        // accumulator per microtile.  Placed above
                        // the dispatch: redundant for the tail,
                        // which re-zeroes tile 0 itself, but cheap
                        // and robust if the tail ever grows.
#endif

    ptrue   p0.\dt
    SME_CNT x14, \dt            // x14 = SVL_<dt>
.endm

.macro UKR_SME_DEINIT
#if SME_UKR_OWNS_SM && SME_UKR_OWNS_ZA
    smstop                  // PSTATE.SM=0, PSTATE.ZA=0
#elif SME_UKR_OWNS_SM
    smstop  sm
#elif SME_UKR_OWNS_ZA
    smstop  za
#endif
    ldp d8,  d9,  [sp, #0]
    ldp d10, d11, [sp, #16]
    ldp d12, d13, [sp, #32]
    ldp d14, d15, [sp, #48]
    add sp, sp, #.LSME_SPILL
.endm


/*===========================================================================
  K-LOOP
  SME_KLOOP dt, nblk, nk
      x2 k, x4 a, x5 b, x11 second b base, x13 counter.
      Per k-step: 2 A vectors + 2*nblk B vectors + 4*nblk outer products.
      Registers: A -> z0..z(2*nk-1), B -> z(2*nk)..  Loads for step p+2 are
      issued right after the outer products of step p.
===========================================================================*/
.macro SME_KLOADS p, dt, nblk, nk
    SME_LD %(2*\p),   p0, x4, %(2*\p),   \dt        // A, row half 0
    SME_LD %(2*\p+1), p0, x4, %(2*\p+1), \dt        // A, row half 1
    .set    .L_kj, 0
    .rept   2*\nblk
      .set  .L_kvl, \p*2*\nblk + .L_kj          // B vector index
      .set  .L_kz,  2*\nk + .L_kvl
      .if .L_kvl < 8
    SME_LD %.L_kz, p0, x5,  %.L_kvl,     \dt
      .else
    SME_LD %.L_kz, p0, x11, %(.L_kvl-8), \dt
      .endif
      .set  .L_kj, .L_kj+1
    .endr
.endm

.macro SME_KFMOPA p, dt, nblk, nk
    .set    .L_fj, 0
    .rept   2*\nblk
      .set  .L_fb, 2*\nk + \p*2*\nblk + .L_fj       // B register
    SME_FMOPA %(2*.L_fj),   p0, p0, %(2*\p),   %.L_fb, \dt  // tile 2j   (i=0)
    SME_FMOPA %(2*.L_fj+1), p0, p0, %(2*\p+1), %.L_fb, \dt  // tile 2j+1 (i=1)
      .set  .L_fj, .L_fj+1
    .endr
.endm

.macro SME_KLOOP dt, nblk, nk
    .if (2*\nk + 2*\nblk*\nk) > 32
      .error "SME_KLOOP: k-unroll x nblk needs more than 32 z registers"
    .endif
    .set    .L_nkl2, 0
    .if \nk == 2
      .set  .L_nkl2, 1
    .endif
    .if \nk == 4
      .set  .L_nkl2, 2
    .endif
    .if \nk == 8
      .set  .L_nkl2, 3
    .endif
    .if (1 << .L_nkl2) != \nk
      .error "SME_KLOOP: nk must be 1, 2, 4 or 8"
    .endif

    .if (2*\nblk*\nk) > 8               // B needs a second base
    addvl   x11, x5, #8
    .endif
    lsr x13, x2, #.L_nkl2               // k / nk
    .if \nk > 1
    and x2, x2, #(\nk-1)            // k % nk
    .endif
    cbz x13, 1f
0:  // ---- main loop, \nk k-steps ----------------------------------------
    SME_KLOADS 0, \dt, \nblk, \nk
    .if \nk > 1
    SME_KLOADS 1, \dt, \nblk, \nk
    .endif
    .set    .L_kp, 0
    .rept   \nk
    SME_KFMOPA %.L_kp, \dt, \nblk, \nk
      .if (.L_kp+2) < \nk
    SME_KLOADS %(.L_kp+2), \dt, \nblk, \nk
      .endif
      .set  .L_kp, .L_kp+1
    .endr
    addvl   x4, x4, #(2*\nk)
    addvl   x5, x5, #(2*\nblk*\nk)
    .if (2*\nblk*\nk) > 8
    addvl   x11, x11, #(2*\nblk*\nk)
    .endif
    subs    x13, x13, #1
    b.ne    0b
1:  // ---- k remainder ---------------------------------------------------
    .if \nk > 1
    cbz x2, 2f
3:  SME_KLOADS 0, \dt, \nblk, 1
    SME_KFMOPA 0, \dt, \nblk, 1
    addvl   x4, x4, #2
    addvl   x5, x5, #(2*\nblk)
    subs    x2, x2, #1
    b.ne    3b
2:
    .endif
.endm


/*===========================================================================
  STORE, HORIZONTAL SLICES  (one C row per slice; wants cs_c == 1)

  SME_STORE_ROWS_GROUP is the 4-instruction chunk: one block b, one slice.
  It covers row halves i=0 (at x15) and i=1 (at x8) crossed with column
  chunks 2b and 2b+1.
===========================================================================*/
.macro SME_STORE_ROWS_GROUP tb, zacc, zc, vl0, sl, gs, bmode, dt
    SME_MOVA_H %(\zacc+0), %(\tb+0), \sl, \dt   // i=0, j=vl0
    SME_MOVA_H %(\zacc+1), %(\tb+2), \sl, \dt   // i=0, j=vl0+1
    SME_MOVA_H %(\zacc+2), %(\tb+1), \sl, \dt   // i=1, j=vl0
    SME_MOVA_H %(\zacc+3), %(\tb+3), \sl, \dt   // i=1, j=vl0+1
  .if \bmode != 1
    .if \gs == 0
    SME_LD %(\zc+0), p0, x15, %(\vl0),   \dt
    SME_LD %(\zc+1), p0, x15, %(\vl0+1), \dt
    SME_LD %(\zc+2), p0, x8,  %(\vl0),   \dt
    SME_LD %(\zc+3), p0, x8,  %(\vl0+1), \dt
    .else
    SME_LD_GS %(\zc+0), p0, x15, %(26+\vl0),   \dt
    SME_LD_GS %(\zc+1), p0, x15, %(26+\vl0+1), \dt
    SME_LD_GS %(\zc+2), p0, x8,  %(26+\vl0),   \dt
    SME_LD_GS %(\zc+3), p0, x8,  %(26+\vl0+1), \dt
    .endif
  .endif
    SME_SCALE_MUL %(\zc+0), %(\zacc+0), \bmode, \dt
    SME_SCALE_MUL %(\zc+1), %(\zacc+1), \bmode, \dt
    SME_SCALE_MUL %(\zc+2), %(\zacc+2), \bmode, \dt
    SME_SCALE_MUL %(\zc+3), %(\zacc+3), \bmode, \dt

    SME_SCALE_MLA %(\zc+0), %(\zacc+0), \bmode, \dt
    SME_SCALE_MLA %(\zc+1), %(\zacc+1), \bmode, \dt
    SME_SCALE_MLA %(\zc+2), %(\zacc+2), \bmode, \dt
    SME_SCALE_MLA %(\zc+3), %(\zacc+3), \bmode, \dt
  .if \gs == 0
    SME_ST %(\zc+0), p0, x15, %(\vl0),   \dt
    SME_ST %(\zc+1), p0, x15, %(\vl0+1), \dt
    SME_ST %(\zc+2), p0, x8,  %(\vl0),   \dt
    SME_ST %(\zc+3), p0, x8,  %(\vl0+1), \dt
  .else
    SME_ST_GS %(\zc+0), p0, x15, %(26+\vl0),   \dt
    SME_ST_GS %(\zc+1), p0, x15, %(26+\vl0+1), \dt
    SME_ST_GS %(\zc+2), p0, x8,  %(26+\vl0),   \dt
    SME_ST_GS %(\zc+3), p0, x8,  %(26+\vl0+1), \dt
  .endif
.endm

/* 2*nblk index vectors in z26.., one per column chunk: [j*SVL*cs_c + l*cs_c] */
.macro SME_GS_OFFSETS nblk, dt
    mul x11, x14, x10               // SVL * cs_c bytes
  .ifc \dt, d
    index   z26.d, #0, x10
    dup z30.d, x11
  .endif
  .ifc \dt, s
    index   z26.s, #0, w10
    dup z30.s, w11
  .endif
    .set    .L_gj, 1
    .rept   2*\nblk - 1
      SME_GS_ADD %(26+.L_gj), %(25+.L_gj), \dt
      .set  .L_gj, .L_gj+1
    .endr
.endm

.macro SME_GS_ADD zd, zn, dt
    add z\zd\().\dt, z\zn\().\dt, z30.\dt
.endm

.macro SME_STORE_ROWS gs, bmode, dt, nblk
    mov x15, x7                 // &C[0][0]
    madd    x8, x14, x9, x7             // &C[SVL][0]
    mov w12, #0
    lsr x13, x14, #.LSME_NSLICE_LOG2        // SVL/NSLICE iterations
.Lsr\@:
    .set    .L_sl, 0
    .rept   .LSME_NSLICE
      .set  .L_sb, 0
      .rept \nblk
        .set .L_g, .L_sl*\nblk + .L_sb          // group index
        SME_STORE_ROWS_GROUP %(4*.L_sb), %(4*(.L_g % 4)), %(16+4*(.L_g % 2)), \
                             %(2*.L_sb), %.L_sl, \gs, \bmode, \dt
        .set .L_sb, .L_sb+1
      .endr
    add x15, x15, x9                // next C row
    add x8,  x8,  x9
      .set  .L_sl, .L_sl+1
    .endr
    add w12, w12, #.LSME_NSLICE
    subs    x13, x13, #1
    b.ne    .Lsr\@
.endm


/*===========================================================================
  STORE, VERTICAL SLICES  (one C column per slice; wants rs_c == 1)

  Column chunk j lives in tiles 2j (rows 0..SVL-1) and 2j+1 (rows SVL..2SVL-1),
  which are contiguous in m, so both halves are plain MUL VL 0 / 1 accesses off
  that chunk's column pointer.  Chunk pointers are x3, x4, x5, x6.
===========================================================================*/
.macro SME_STORE_COLS_GROUP tb, zacc, zc, xb0, xb1, sl, bmode, dt
    SME_MOVA_V %(\zacc+0), %(\tb+0), \sl, \dt   // chunk 2b, rows 0
    SME_MOVA_V %(\zacc+1), %(\tb+1), \sl, \dt   // chunk 2b, rows SVL
    SME_MOVA_V %(\zacc+2), %(\tb+2), \sl, \dt   // chunk 2b+1, rows 0
    SME_MOVA_V %(\zacc+3), %(\tb+3), \sl, \dt   // chunk 2b+1, rows SVL
  .if \bmode != 1
    SME_LD %(\zc+0), p0, \xb0, 0, \dt
    SME_LD %(\zc+1), p0, \xb0, 1, \dt
    SME_LD %(\zc+2), p0, \xb1, 0, \dt
    SME_LD %(\zc+3), p0, \xb1, 1, \dt
  .endif
    SME_SCALE_MUL %(\zc+0), %(\zacc+0), \bmode, \dt
    SME_SCALE_MUL %(\zc+1), %(\zacc+1), \bmode, \dt
    SME_SCALE_MUL %(\zc+2), %(\zacc+2), \bmode, \dt
    SME_SCALE_MUL %(\zc+3), %(\zacc+3), \bmode, \dt

    SME_SCALE_MLA %(\zc+0), %(\zacc+0), \bmode, \dt
    SME_SCALE_MLA %(\zc+1), %(\zacc+1), \bmode, \dt
    SME_SCALE_MLA %(\zc+2), %(\zacc+2), \bmode, \dt
    SME_SCALE_MLA %(\zc+3), %(\zacc+3), \bmode, \dt
    SME_ST %(\zc+0), p0, \xb0, 0, \dt
    SME_ST %(\zc+1), p0, \xb0, 1, \dt
    SME_ST %(\zc+2), p0, \xb1, 0, \dt
    SME_ST %(\zc+3), p0, \xb1, 1, \dt
.endm

.macro SME_STORE_COLS bmode, dt, nblk
    .if \nblk > 2
      .error "SME_STORE_COLS: only x3..x6 are reserved for column pointers"
    .endif
    mov x3, x7                  // chunk 0
    madd    x4, x14, x10, x3            // chunk 1
    .if \nblk > 1
    madd    x5, x14, x10, x4            // chunk 2
    madd    x6, x14, x10, x5            // chunk 3
    .endif
    mov w12, #0
    lsr x13, x14, #.LSME_NSLICE_LOG2
.Lsc\@:
    .set    .L_sl, 0
    .rept   .LSME_NSLICE
        .set .L_g, .L_sl*\nblk
        SME_STORE_COLS_GROUP 0, %(4*(.L_g % 4)), %(16+4*(.L_g % 2)), \
                             x3, x4, %.L_sl, \bmode, \dt
      .if \nblk > 1
        .set .L_g, .L_g+1
        SME_STORE_COLS_GROUP 4, %(4*(.L_g % 4)), %(16+4*(.L_g % 2)), \
                             x5, x6, %.L_sl, \bmode, \dt
      .endif
    add x3, x3, x10             // next C column
    add x4, x4, x10
      .if \nblk > 1
    add x5, x5, x10
    add x6, x6, x10
      .endif
      .set  .L_sl, .L_sl+1
    .endr
    add w12, w12, #.LSME_NSLICE
    subs    x13, x13, #1
    b.ne    .Lsc\@
.endm

/*===========================================================================
  TAIL  (m < MR and/or n < NR)

  One tile, one subtile of the (<=2) x (2*nblk) grid at a time.  p1 masks A
  rows, p2 masks B columns, so nothing outside the live m x n block of C is
  touched and BLIS's GEMM_UKR_SETUP_CT_ANY is not needed.

      x16 mi   x17 nj   x11 a-run   x13 b-run   x15 k / col count
      x3  &C[mi][nj]    x6 != 0 iff beta != 0   x8 rows in this subtile
===========================================================================*/
.macro SME_SCALAR_UPDATE dt             // one C element, scalar FP
  .ifc \dt, d
    ldr d0, [x11], #8
    fmul    d0, d0, d24
    cbz x6, 44f
    ldr d1, [x13]
    fmadd   d0, d1, d25, d0
44: str d0, [x13]
  .endif
  .ifc \dt, s
    ldr s0, [x11], #4
    fmul    s0, s0, s24
    cbz x6, 44f
    ldr s1, [x13]
    fmadd   s0, s1, s25, s0
44: str s0, [x13]
  .endif
.endm

.macro SME_TAIL dt, nblk
    SME_LD1R 24, x3, \dt                // alpha (d24/s24 too)
    SME_LD1R 25, x6, \dt                // beta
    SME_BETA_ZERO_TEST 6, x6, \dt           // x6 != 0 iff beta != 0
#if SME_FA64
  .ifc \dt, d
    index   z26.d, #0, x10              // C column byte offsets
  .endif
  .ifc \dt, s
    index   z26.s, #0, w10
  .endif
#else
    addvl   sp, sp, #-1             // one-vector staging buffer
#endif
    mov x16, #0                 // mi
10: cmp x16, x0
    b.ge    19f
    SME_WHILELT p1, x16, x0, \dt            // live rows
    mov x17, #0                 // nj
11: cmp x17, x1
    b.ge    18f
    SME_WHILELT p2, x17, x1, \dt            // live columns

    // ---- accumulate one subtile ----------------------------------------
    SME_ZERO_TILE 0, \dt
    add x11, x4, x16, lsl #.LSME_ESH        // a + mi
    add x13, x5, x17, lsl #.LSME_ESH        // b + nj
    mov x15, x2                 // k
    cbz x15, 13f
12: SME_LD 0, p1, x11, 0, \dt
    SME_LD 1, p2, x13, 0, \dt
    SME_FMOPA 0, p1, p2, 0, 1, \dt
    addvl   x11, x11, #2                // A: MR elems per k
    addvl   x13, x13, #(2*\nblk)            // B: NR elems per k
    subs    x15, x15, #1
    b.ne    12b

    // ---- scale + store the live part -----------------------------------
13: sub x8, x0, x16
    cmp x8, x14
    csel    x8, x8, x14, lt             // rows = min(m-mi, SVL)
    cbz x8, 17f
    madd    x3, x16, x9, x7             // &C[mi][nj]
    madd    x3, x17, x10, x3
    mov w12, #0
14: SME_MOVA_H 0, 0, 0, \dt
    cmp x10, #.LSME_ES              // cs_c == 1 ?
    b.ne    15f
    fmul    z0.\dt, p2/m, z0.\dt, z24.\dt
    cbz x6, 141f
    SME_LD 1, p2, x3, 0, \dt
    fmla    z0.\dt, p2/m, z1.\dt, z25.\dt
141:    SME_ST 0, p2, x3, 0, \dt
    b   16f
15:
#if SME_FA64
    fmul    z0.\dt, p2/m, z0.\dt, z24.\dt
    cbz x6, 151f
    SME_LD_GS 1, p2, x3, 26, \dt
    fmla    z0.\dt, p2/m, z1.\dt, z25.\dt
151:    SME_ST_GS 0, p2, x3, 26, \dt
#else
    // no FA64: stage the row contiguously, then scalar-FP scatter it out
    SME_ST 0, p2, sp, 0, \dt
    mov x11, sp
    mov x13, x3
    sub x15, x1, x17
    cmp x15, x14
    csel    x15, x15, x14, lt           // live columns
152:    SME_SCALAR_UPDATE \dt
    add x13, x13, x10
    subs    x15, x15, #1
    b.ne    152b
#endif
16: add x3, x3, x9
    add w12, w12, #1
    subs    x8, x8, #1
    b.ne    14b
17: add x17, x17, x14
    b   11b
18: add x16, x16, x14
    b   10b
19:
#if !SME_FA64
    addvl   sp, sp, #1
#endif
.endm

#endif /* SME_UKR_MACROS_H */
