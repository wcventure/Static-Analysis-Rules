DISAS_INSN(shift_reg)
{
    TCGv reg;
    TCGv shift;

    reg = DREG(insn, 0);
    shift = DREG(insn, 9);
    if (insn & 0x100) {
        gen_helper_shl_cc(reg, cpu_env, reg, shift);
    } else {
        if (insn & 8) {
            gen_helper_shr_cc(reg, cpu_env, reg, shift);
        } else {
            gen_helper_sar_cc(reg, cpu_env, reg, shift);
        }
    }
    set_cc_op(s, CC_OP_FLAGS);
}
