__asm__ (
    "add s2, %[inputs], 0\n\t"
    PREPARE_SCALAR
    "ld s3, " I_VLEN "(s2)\n\t" // vlen
    "vsetvli s3, s3, e" SIZEBITS ", m2, ta, ma\n\t"
    PREPARE_STRIDEX("t6", "s5", I_INCX, SIZESHIFT)
    PREPARE_STRIDEY("t5", "s4", I_INCY, SIZESHIFT)


    "ld t0, " I_X "(s2)\n\t" // x
    "ld t2, " I_Y "(s2)\n\t" // y
    "add t3, t2, 0\n\t"

    PREPARE_LDIMX("t6", I_LDIMX, SIZESHIFT)
    PREPARE_LDIMY("t5", I_LDIMY, SIZESHIFT)

    "ld t4, " I_NITER "(s2)\n\t" //niter
    "beq t4, zero, ." LABELPREFIX "7vm2end%=\n\t"
    "." LABELPREFIX "7vm2loop%=:\n\t"

        VLOADX("v0","t0")
        VLOADY(VXTOY("v0","v2"),"t2")
        VTRANSFORM(VXTOY("v0", "v2"), "v0")
        "add t0, t0, t6\n\t"
        "add t2, t2, t5\n\t"

        VLOADX("v4","t0")
        VLOADY(VXTOY("v4","v6"),"t2")
        VTRANSFORM(VXTOY("v4", "v6"), "v4")
        "add t0, t0, t6\n\t"
        "add t2, t2, t5\n\t"

        VLOADX("v8","t0")
        VLOADY(VXTOY("v8","v10"),"t2")
        VTRANSFORM(VXTOY("v8", "v10"), "v8")
        "add t0, t0, t6\n\t"
        "add t2, t2, t5\n\t"

        VLOADX("v12","t0")
        VLOADY(VXTOY("v12","v14"),"t2")
        VTRANSFORM(VXTOY("v12", "v14"), "v12")
        "add t0, t0, t6\n\t"
        "add t2, t2, t5\n\t"

        VLOADX("v16","t0")
        VLOADY(VXTOY("v16","v18"),"t2")
        VTRANSFORM(VXTOY("v16", "v18"), "v12")
        "add t0, t0, t6\n\t"
        "add t2, t2, t5\n\t"

        VLOADX("v20","t0")
        VLOADY(VXTOY("v20","v22"),"t2")
        VTRANSFORM(VXTOY("v20", "v22"), "v12")
        "add t0, t0, t6\n\t"
        "add t2, t2, t5\n\t"

        VLOADX("v24","t0")
        VLOADY(VXTOY("v24","v26"),"t2")
        VTRANSFORM(VXTOY("v24", "v26"), "v12")
        "add t0, t0, t6\n\t"
        "add t2, t2, t5\n\t"

        VSTOREY(VXTOY("v0","v2"),"t3")
        "add t3, t3, t5\n\t"
        VSTOREY(VXTOY("v4","v6"),"t3")
        "add t3, t3, t5\n\t"
        VSTOREY(VXTOY("v8","v10"),"t3")
        "add t3, t3, t5\n\t"
        VSTOREY(VXTOY("v12","v14"),"t3")
        "add t3, t3, t5\n\t"
        VSTOREY(VXTOY("v16","v18"),"t3")
        "add t3, t3, t5\n\t"
        VSTOREY(VXTOY("v20","v22"),"t3")
        "add t3, t3, t5\n\t"
        VSTOREY(VXTOY("v24","v26"),"t3")
        "add t3, t3, t5\n\t"

        "add t4, t4, -1\n\t"
        "bnez t4, ." LABELPREFIX "7vm2loop%=\n\t"

    "." LABELPREFIX "7vm2end%=:\n\t"
    "ld t4, " I_NLEFT "(s2)\n\t" // nleft
    "beq t4, zero, ." LABELPREFIX "end%=\n\t"
    "." LABELPREFIX "loop%=:\n\t"
        "vsetvli s3, t4, e" SIZEBITS ", m2, ta, ma\n\t"
        PREPARE_LDIMX("t6", I_LDIMX, SIZESHIFT)
        PREPARE_LDIMY("t5", I_LDIMY, SIZESHIFT)

        VLOADX("v0","t0")
        VLOADY(VXTOY("v0","v2"),"t2")
        "add t0, t0, t6\n\t"
        VTRANSFORM(VXTOY("v0", "v2"), "v0")
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
      "v12", "v13", "v14", "v15",
      "v16", "v17", "v18", "v19",
      "v20", "v21", "v22", "v23",
      "v24", "v25", "v26", "v27"
);
