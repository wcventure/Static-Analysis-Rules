static inline void tcg_out_qemu_st(TCGContext *s, int cond,
                const TCGArg *args, int opc)
{
    int addr_reg, data_reg, data_reg2;
#ifdef CONFIG_SOFTMMU
    int mem_index, s_bits;
# if TARGET_LONG_BITS == 64
    int addr_reg2;
# endif
    uint32_t *label_ptr;
#endif

    data_reg = *args++;
    if (opc == 3)
        data_reg2 = *args++;
    else
        data_reg2 = 0; /
    addr_reg = *args++;
#if TARGET_LONG_BITS == 64
    addr_reg2 = *args++;
#endif
#ifdef CONFIG_SOFTMMU
    mem_index = *args;
    s_bits = opc & 3;

    /
    tcg_out_dat_reg(s, COND_AL, ARITH_MOV,
                    8, 0, addr_reg, SHIFT_IMM_LSR(TARGET_PAGE_BITS));
    tcg_out_dat_imm(s, COND_AL, ARITH_AND,
                    0, 8, CPU_TLB_SIZE - 1);
    tcg_out_dat_reg(s, COND_AL, ARITH_ADD,
                    0, TCG_AREG0, 0, SHIFT_IMM_LSL(CPU_TLB_ENTRY_BITS));
    /
    if (mem_index)
        tcg_out_dat_imm(s, COND_AL, ARITH_ADD, 0, 0,
                        (mem_index << (TLB_SHIFT & 1)) |
                        ((16 - (TLB_SHIFT >> 1)) << 8));
    tcg_out_ld32_12(s, COND_AL, 1, 0,
                    offsetof(CPUState, tlb_table[0][0].addr_write));
    tcg_out_dat_reg(s, COND_AL, ARITH_CMP,
                    0, 1, 8, SHIFT_IMM_LSL(TARGET_PAGE_BITS));
    /
    if (s_bits)
        tcg_out_dat_imm(s, COND_EQ, ARITH_TST,
                        0, addr_reg, (1 << s_bits) - 1);
#  if TARGET_LONG_BITS == 64
    /
    tcg_out_ld32_12(s, COND_EQ, 1, 0,
                    offsetof(CPUState, tlb_table[0][0].addr_write)
                    + 4);
    tcg_out_dat_reg(s, COND_EQ, ARITH_CMP,
                    0, 1, addr_reg2, SHIFT_IMM_LSL(0));
#  endif
    tcg_out_ld32_12(s, COND_EQ, 1, 0,
                    offsetof(CPUState, tlb_table[0][0].addend));

    switch (opc) {
    case 0:
        tcg_out_st8_r(s, COND_EQ, data_reg, addr_reg, 1);
        break;
    case 0 | 4:
        tcg_out_st8s_r(s, COND_EQ, data_reg, addr_reg, 1);
        break;
    case 1:
        tcg_out_st16u_r(s, COND_EQ, data_reg, addr_reg, 1);
        break;
    case 1 | 4:
        tcg_out_st16s_r(s, COND_EQ, data_reg, addr_reg, 1);
        break;
    case 2:
    default:
        tcg_out_st32_r(s, COND_EQ, data_reg, addr_reg, 1);
        break;
    case 3:
        tcg_out_st32_rwb(s, COND_EQ, data_reg, 1, addr_reg);
        tcg_out_st32_12(s, COND_EQ, data_reg2, 1, 4);
        break;
    }

    label_ptr = (void *) s->code_ptr;
    tcg_out_b(s, COND_EQ, 8);

    /
    if (addr_reg)
        tcg_out_dat_reg(s, cond, ARITH_MOV,
                        0, 0, addr_reg, SHIFT_IMM_LSL(0));
# if TARGET_LONG_BITS == 32
    switch (opc) {
    case 0:
        tcg_out_dat_imm(s, cond, ARITH_AND, 1, data_reg, 0xff);
        tcg_out_dat_imm(s, cond, ARITH_MOV, 2, 0, mem_index);
        break;
    case 1:
        tcg_out_dat_reg(s, cond, ARITH_MOV,
                        1, 0, data_reg, SHIFT_IMM_LSL(16));
        tcg_out_dat_reg(s, cond, ARITH_MOV,
                        1, 0, 1, SHIFT_IMM_LSR(16));
        tcg_out_dat_imm(s, cond, ARITH_MOV, 2, 0, mem_index);
        break;
    case 2:
        if (data_reg != 1)
            tcg_out_dat_reg(s, cond, ARITH_MOV,
                            1, 0, data_reg, SHIFT_IMM_LSL(0));
        tcg_out_dat_imm(s, cond, ARITH_MOV, 2, 0, mem_index);
        break;
    case 3:
        if (data_reg != 1)
            tcg_out_dat_reg(s, cond, ARITH_MOV,
                            1, 0, data_reg, SHIFT_IMM_LSL(0));
        if (data_reg2 != 2)
            tcg_out_dat_reg(s, cond, ARITH_MOV,
                            2, 0, data_reg2, SHIFT_IMM_LSL(0));
        tcg_out_dat_imm(s, cond, ARITH_MOV, 3, 0, mem_index);
        break;
    }
# else
    if (addr_reg2 != 1)
        tcg_out_dat_reg(s, cond, ARITH_MOV,
                        1, 0, addr_reg2, SHIFT_IMM_LSL(0));
    switch (opc) {
    case 0:
        tcg_out_dat_imm(s, cond, ARITH_AND, 2, data_reg, 0xff);
        tcg_out_dat_imm(s, cond, ARITH_MOV, 3, 0, mem_index);
        break;
    case 1:
        tcg_out_dat_reg(s, cond, ARITH_MOV,
                        2, 0, data_reg, SHIFT_IMM_LSL(16));
        tcg_out_dat_reg(s, cond, ARITH_MOV,
                        2, 0, 2, SHIFT_IMM_LSR(16));
        tcg_out_dat_imm(s, cond, ARITH_MOV, 3, 0, mem_index);
        break;
    case 2:
        if (data_reg != 2)
            tcg_out_dat_reg(s, cond, ARITH_MOV,
                            2, 0, data_reg, SHIFT_IMM_LSL(0));
        tcg_out_dat_imm(s, cond, ARITH_MOV, 3, 0, mem_index);
        break;
    case 3:
        tcg_out_dat_imm(s, cond, ARITH_MOV, 8, 0, mem_index);
        tcg_out32(s, (cond << 28) | 0x052d8010); /
        if (data_reg != 2)
            tcg_out_dat_reg(s, cond, ARITH_MOV,
                            2, 0, data_reg, SHIFT_IMM_LSL(0));
        if (data_reg2 != 3)
            tcg_out_dat_reg(s, cond, ARITH_MOV,
                            3, 0, data_reg2, SHIFT_IMM_LSL(0));
        break;
    }
# endif

# ifdef SAVE_LR
    tcg_out_dat_reg(s, cond, ARITH_MOV, 8, 0, 14, SHIFT_IMM_LSL(0));
# endif

    tcg_out_bl(s, cond, (tcg_target_long) qemu_st_helpers[s_bits] -
                    (tcg_target_long) s->code_ptr);

# if TARGET_LONG_BITS == 64
    if (opc == 3)
        tcg_out_dat_imm(s, cond, ARITH_ADD, 13, 13, 0x10);
# endif

# ifdef SAVE_LR
    tcg_out_dat_reg(s, cond, ARITH_MOV, 14, 0, 8, SHIFT_IMM_LSL(0));
# endif

    *label_ptr += ((void *) s->code_ptr - (void *) label_ptr - 8) >> 2;
#else
    switch (opc) {
    case 0:
        tcg_out_st8_12(s, COND_AL, data_reg, addr_reg, 0);
        break;
    case 0 | 4:
        tcg_out_st8s_8(s, COND_AL, data_reg, addr_reg, 0);
        break;
    case 1:
        tcg_out_st16u_8(s, COND_AL, data_reg, addr_reg, 0);
        break;
    case 1 | 4:
        tcg_out_st16s_8(s, COND_AL, data_reg, addr_reg, 0);
        break;
    case 2:
    default:
        tcg_out_st32_12(s, COND_AL, data_reg, addr_reg, 0);
        break;
    case 3:
        /
        tcg_out_st32_12(s, COND_AL, data_reg, addr_reg, 0);
        tcg_out_st32_12(s, COND_AL, data_reg2, addr_reg, 4);
        break;
    }
#endif
}
