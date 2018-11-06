DISAS_INSN(shift_im)
{
    TCGv reg;
    int tmp;
    TCGv shift;

    set_cc_op(s, CC_OP_FLAGS);

    reg = DREG(insn, 0);
    tmp = (insn >> 9) & 7;
    if (tmp == 0)
        tmp = 8;
    shift = tcg_const_i32(tmp);
    /
    if (insn & 0x100) {
        gen_helper_shl_cc(reg, cpu_env, reg, shift);
    } else {
        if (insn & 8) {
            gen_helper_shr_cc(reg, cpu_env, reg, shift);
        } else {
            gen_helper_sar_cc(reg, cpu_env, reg, shift);
        }
    }
}
