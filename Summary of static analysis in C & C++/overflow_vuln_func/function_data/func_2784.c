static void tcg_out_qemu_st (TCGContext *s, const TCGArg *args, int opc)
{
    int addr_reg, r0, r1, data_reg, data_reg2, bswap, rbase;
#ifdef CONFIG_SOFTMMU
    int mem_index, r2, addr_reg2;
    uint8_t *label_ptr;
#endif

    data_reg = *args++;
    if (opc == 3)
        data_reg2 = *args++;
    else
        data_reg2 = 0;
    addr_reg = *args++;

#ifdef CONFIG_SOFTMMU
#if TARGET_LONG_BITS == 64
    addr_reg2 = *args++;
#else
    addr_reg2 = 0;
#endif
    mem_index = *args;
    r0 = 3;
    r1 = 4;
    r2 = 0;
    rbase = 0;

    tcg_out_tlb_check (
        s, r0, r1, r2, addr_reg, addr_reg2, opc & 3,
        offsetof (CPUArchState, tlb_table[mem_index][0].addr_write),
        offsetof (CPUTLBEntry, addend) - offsetof (CPUTLBEntry, addr_write),
        &label_ptr
        );
#else  /
    r0 = addr_reg;
    r1 = 3;
    rbase = GUEST_BASE ? TCG_GUEST_BASE_REG : 0;
#endif

#ifdef TARGET_WORDS_BIGENDIAN
    bswap = 0;
#else
    bswap = 1;
#endif
    switch (opc) {
    case 0:
        tcg_out32 (s, STBX | SAB (data_reg, rbase, r0));
        break;
    case 1:
        if (bswap)
            tcg_out32 (s, STHBRX | SAB (data_reg, rbase, r0));
        else
            tcg_out32 (s, STHX | SAB (data_reg, rbase, r0));
        break;
    case 2:
        if (bswap)
            tcg_out32 (s, STWBRX | SAB (data_reg, rbase, r0));
        else
            tcg_out32 (s, STWX | SAB (data_reg, rbase, r0));
        break;
    case 3:
        if (bswap) {
            tcg_out32 (s, ADDI | RT (r1) | RA (r0) | 4);
            tcg_out32 (s, STWBRX | SAB (data_reg,  rbase, r0));
            tcg_out32 (s, STWBRX | SAB (data_reg2, rbase, r1));
        }
        else {
#ifdef CONFIG_USE_GUEST_BASE
            tcg_out32 (s, STWX | SAB (data_reg2, rbase, r0));
            tcg_out32 (s, ADDI | RT (r1) | RA (r0) | 4);
            tcg_out32 (s, STWX | SAB (data_reg,  rbase, r1));
#else
            tcg_out32 (s, STW | RS (data_reg2) | RA (r0));
            tcg_out32 (s, STW | RS (data_reg) | RA (r0) | 4);
#endif
        }
        break;
    }

#ifdef CONFIG_SOFTMMU
    add_qemu_ldst_label (s,
                         0,
                         opc,
                         data_reg,
                         data_reg2,
                         addr_reg,
                         addr_reg2,
                         mem_index,
                         s->code_ptr,
                         label_ptr);
#endif
}
