    l1 = gen_new_label();
    l2 = gen_new_label();
#if defined(TARGET_PPC64)
    if (ctx->sf_mode) {
        tcg_gen_brcondi_tl(TCG_COND_EQ, arg1, INT64_MIN, l1);
    } else {
        TCGv t0 = tcg_temp_new(TCG_TYPE_TL);
	tcg_gen_ext32s_tl(t0, arg1);
        tcg_gen_brcondi_tl(TCG_COND_EQ, t0, INT32_MIN, l1);
    }
#else
        tcg_gen_brcondi_tl(TCG_COND_EQ, arg1, INT32_MIN, l1);
#endif
    tcg_gen_neg_tl(ret, arg1);
    if (ov_check) {
        tcg_gen_andi_tl(cpu_xer, cpu_xer, ~(1 << XER_OV));
    }
    tcg_gen_br(l2);
    gen_set_label(l1);
    tcg_gen_mov_tl(ret, arg1);
    if (ov_check) {
        tcg_gen_ori_tl(cpu_xer, cpu_xer, (1 << XER_OV) | (1 << XER_SO));
    }
    gen_set_label(l2);
    if (unlikely(Rc(ctx->opcode) != 0))
        gen_set_Rc0(ctx, ret);
}
GEN_HANDLER(neg, 0x1F, 0x08, 0x03, 0x0000F800, PPC_INTEGER)
{
    gen_op_neg(ctx, cpu_gpr[rD(ctx->opcode)], cpu_gpr[rA(ctx->opcode)], 0);
}
GEN_HANDLER(nego, 0x1F, 0x08, 0x13, 0x0000F800, PPC_INTEGER)
{
    gen_op_neg(ctx, cpu_gpr[rD(ctx->opcode)], cpu_gpr[rA(ctx->opcode)], 1);
}
