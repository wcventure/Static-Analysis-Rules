DISAS_INSN(divl)
{
    TCGv num;
    TCGv den;
    TCGv reg;
    uint16_t ext;

    ext = read_im16(env, s);
    if (ext & 0x87f8) {
        gen_exception(s, s->pc - 4, EXCP_UNSUPPORTED);
        return;
    }
    num = DREG(ext, 12);
    reg = DREG(ext, 0);
    tcg_gen_mov_i32(QREG_DIV1, num);
    SRC_EA(env, den, OS_LONG, 0, NULL);
    tcg_gen_mov_i32(QREG_DIV2, den);
    if (ext & 0x0800) {
        gen_helper_divs(cpu_env, tcg_const_i32(0));
    } else {
        gen_helper_divu(cpu_env, tcg_const_i32(0));
    }
    if ((ext & 7) == ((ext >> 12) & 7)) {
        /
        tcg_gen_mov_i32 (reg, QREG_DIV1);
    } else {
        /
        tcg_gen_mov_i32 (reg, QREG_DIV2);
    }
    set_cc_op(s, CC_OP_FLAGS);
}
