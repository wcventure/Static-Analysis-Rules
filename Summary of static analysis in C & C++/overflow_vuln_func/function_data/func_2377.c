static inline void gen_goto_tb(DisasContext *ctx, int n, target_ulong dest)
{
    TranslationBlock *tb;
    tb = ctx->tb;
    if ((tb->pc & TARGET_PAGE_MASK) == (dest & TARGET_PAGE_MASK)) {
        if (n == 0)
            gen_op_goto_tb0(TBPARAM(tb));
        else
            gen_op_goto_tb1(TBPARAM(tb));
        gen_op_set_T1(dest);
        gen_op_b_T1();
        gen_op_set_T0((long)tb + n);
        if (ctx->singlestep_enabled)
            gen_op_debug();
        gen_op_exit_tb();
    } else {
        gen_op_set_T1(dest);
        gen_op_b_T1();
        gen_op_reset_T0();
        if (ctx->singlestep_enabled)
            gen_op_debug();
        gen_op_exit_tb();
    }
}
