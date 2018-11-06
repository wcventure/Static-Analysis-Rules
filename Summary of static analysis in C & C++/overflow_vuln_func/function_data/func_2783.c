static void tcg_out_qemu_ld (TCGContext *s, const TCGArg *args, int opc)
{
    int addr_reg, data_reg, data_reg2, r0, r1, rbase, bswap;
#ifdef CONFIG_SOFTMMU
    int mem_index, s_bits, r2, addr_reg2;
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
    s_bits = opc & 3;
    r0 = 3;
    r1 = 4;
    r2 = 0;
    rbase = 0;

    tcg_out_tlb_check (
        s, r0, r1, r2, addr_reg, addr_reg2, s_bits,
        offsetof (CPUArchState, tlb_table[mem_index][0].addr_read),
        offsetof (CPUTLBEntry, addend) - offsetof (CPUTLBEntry, addr_read),
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
    default:
    case 0:
        tcg_out32 (s, LBZX | TAB (data_reg, rbase, r0));
        break;
    case 0|4:
        tcg_out32 (s, LBZX | TAB (data_reg, rbase, r0));
        tcg_out32 (s, EXTSB | RA (data_reg) | RS (data_reg));
        break;
    case 1:
        if (bswap)
            tcg_out32 (s, LHBRX | TAB (data_reg, rbase, r0));
        else
            tcg_out32 (s, LHZX | TAB (data_reg, rbase, r0));
        break;
    case 1|4:
        if (bswap) {
            tcg_out32 (s, LHBRX | TAB (data_reg, rbase, r0));
            tcg_out32 (s, EXTSH | RA (data_reg) | RS (data_reg));
        }
        else tcg_out32 (s, LHAX | TAB (data_reg, rbase, r0));
        break;
    case 2:
        if (bswap)
            tcg_out32 (s, LWBRX | TAB (data_reg, rbase, r0));
        else
            tcg_out32 (s, LWZX | TAB (data_reg, rbase, r0));
        break;
    case 3:
        if (bswap) {
            tcg_out32 (s, ADDI | RT (r1) | RA (r0) | 4);
            tcg_out32 (s, LWBRX | TAB (data_reg, rbase, r0));
            tcg_out32 (s, LWBRX | TAB (data_reg2, rbase, r1));
        }
        else {
#ifdef CONFIG_USE_GUEST_BASE
            tcg_out32 (s, ADDI | RT (r1) | RA (r0) | 4);
            tcg_out32 (s, LWZX | TAB (data_reg2, rbase, r0));
            tcg_out32 (s, LWZX | TAB (data_reg, rbase, r1));
#else
            if (r0 == data_reg2) {
                tcg_out32 (s, LWZ | RT (0) | RA (r0));
                tcg_out32 (s, LWZ | RT (data_reg) | RA (r0) | 4);
                tcg_out_mov (s, TCG_TYPE_I32, data_reg2, 0);
            }
            else {
                tcg_out32 (s, LWZ | RT (data_reg2) | RA (r0));
                tcg_out32 (s, LWZ | RT (data_reg) | RA (r0) | 4);
            }
#endif
        }
        break;
    }
#ifdef CONFIG_SOFTMMU
    add_qemu_ldst_label (s,
                         1,
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
