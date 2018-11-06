DISAS_INSN(divw)
{
    TCGv reg;
    TCGv tmp;
    TCGv src;
    int sign;

    sign = (insn & 0x100) != 0;
    reg = DREG(insn, 9);
    if (sign) {
        tcg_gen_ext16s_i32(QREG_DIV1, reg);
    } else {
        tcg_gen_ext16u_i32(QREG_DIV1, reg);
    }
    SRC_EA(env, src, OS_WORD, sign, NULL);
    tcg_gen_mov_i32(QREG_DIV2, src);
    if (sign) {
        gen_helper_divs(cpu_env, tcg_const_i32(1));
    } else {
        gen_helper_divu(cpu_env, tcg_const_i32(1));
    }

    tmp = tcg_temp_new();
    src = tcg_temp_new();
    tcg_gen_ext16u_i32(tmp, QREG_DIV1);
    tcg_gen_shli_i32(src, QREG_DIV2, 16);
    tcg_gen_or_i32(reg, tmp, src);

    set_cc_op(s, CC_OP_FLAGS);
}
