__asm__ (
    "add s2, %[inputs], 0\n\t"
    PREPARE_SCALAR
    "ld s3, " I_VLEN "(s2)\n\t" // vlen
    "vsetvli s3, s3, e64, m1, ta, ma\n\t"
    PREPARE_STRIDEX("t6", "s5", I_INCX, SIZESHIFT)
    PREPARE_STRIDEY("t5", "s4", I_INCY, SIZESHIFT)
    "ld t0, " I_X "(s2)\n\t" // x
    "add t1, t0, t6\n\t"
    "ld t2, " I_Y "(s2)\n\t" // y
    "add t3, t2, t5\n\t"

    "slli t5, t5, 1\n\t"
    "slli t6, t6, 1\n\t"

    "ld t4, 8(s2)\n\t" //niter
    "beq t4, zero, ." LABELPREFIX "8end%=\n\t"
    "." LABELPREFIX "8loop%=:\n\t"

        VLOADX("v0","t0")
        VLOADX("v1","t1")
        VLOADY(VXTOY("v0","v2"),"t2")
        VLOADY(VXTOY("v1","v3"),"t3")
        VTRANSFORM("v2", "v0")
        VTRANSFORM("v3", "v1")
        "add t0, t0, t6\n\t"
        "add t1, t1, t6\n\t"
        VSTOREY(VXTOY("v0","v2"),"t2")
        VSTOREY(VXTOY("v1","v3"),"t3")

        VLOADX("v4","t0")
        VLOADX("v5","t1")
        "add t2, t2, t5\n\t"
        "add t3, t3, t5\n\t"
        VLOADY(VXTOY("v4","v6"),"t2")
        VLOADY(VXTOY("v5","v7"),"t3")
        VTRANSFORM("v6", "v4")
        VTRANSFORM("v7", "v5")
        "add t0, t0, t6\n\t"
        "add t1, t1, t6\n\t"
        VSTOREY(VXTOY("v4","v6"),"t2")
        VSTOREY(VXTOY("v5","v7"),"t3")

        VLOADX("v8","t0")
        VLOADX("v9","t1")
        "add t2, t2, t5\n\t"
        "add t3, t3, t5\n\t"
        VLOADY(VXTOY("v8","v10"),"t2")
        VLOADY(VXTOY("v9","v11"),"t3")
        VTRANSFORM("v10", "v8")
        VTRANSFORM("v11", "v9")
        "add t0, t0, t6\n\t"
        "add t1, t1, t6\n\t"
        VSTOREY(VXTOY("v8","v10"),"t2")
        VSTOREY(VXTOY("v9","v11"),"t3")

        VLOADX("v12","t0")
        VLOADX("v13","t1")
        "add t2, t2, t5\n\t"
        "add t3, t3, t5\n\t"
        VLOADY(VXTOY("v12","v14"),"t2")
        VLOADY(VXTOY("v13","v15"),"t3")
        VTRANSFORM("v14", "v12")
        VTRANSFORM("v15", "v13")
        "add t0, t0, t6\n\t"
        "add t1, t1, t6\n\t"
        VSTOREY(VXTOY("v12","v14"),"t2")
        VSTOREY(VXTOY("v13","v15"),"t3")

        "add t2, t2, t5\n\t"
        "add t3, t3, t5\n\t"

        "add t4, t4, -1\n\t"
        "bnez t4, ." LABELPREFIX "8loop%=\n\t"

    "." LABELPREFIX "8end%=:\n\t"
    "ld t4, 16(s2)\n\t" // nleft
    "beq t4, zero, ." LABELPREFIX "end%=\n\t"
    "." LABELPREFIX "loop%=:\n\t"
        "vsetvli s3, t4, e64, m1, ta, ma\n\t"
        VSTRIDE_FROM_1STRIDEX("t6", "s5", SIZESHIFT)
        VSTRIDE_FROM_1STRIDEY("t5", "s4", SIZESHIFT)
        VLOADX("v0","t0")
        VLOADY(VXTOY("v0","v2"),"t2")
        "add t0, t0, t6\n\t"
        VTRANSFORM("v2", "v0")
        VSTOREY(VXTOY("v0","v2"),"t2")
        "add t2, t2, t5\n\t"
        "sub t4, t4, s3\n\t"
        "bnez t4, ." LABELPREFIX "loop%=\n\t"
    "." LABELPREFIX "end%=:\n\t"
    : [dummy_y] "+m"(*(double(*)[])y)
    : [inputs] "r" (&ukrinputs)
    : "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s2", "s3", "s4", "s5",
      "v0", "v1", "v2", "v3",
      "v4", "v5", "v6", "v7",
      "v8", "v9", "v10", "v11",
      "v12", "v13", "v14", "v15"
);
