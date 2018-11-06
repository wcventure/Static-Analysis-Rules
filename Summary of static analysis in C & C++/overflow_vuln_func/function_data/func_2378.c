GEN_HANDLER(b, 0x12, 0xFF, 0xFF, 0x00000000, PPC_FLOW)
{
    target_ulong li, target;

    /
#if defined(TARGET_PPC64)
    li = ((target_long)LI(ctx->opcode) << 38) >> 38;
#else
    li = ((target_long)LI(ctx->opcode) << 6) >> 6;
#endif
    if (likely(AA(ctx->opcode) == 0))
        target = ctx->nip + li - 4;
    else
        target = li;
    if (LK(ctx->opcode)) {
        gen_op_setlr(ctx->nip);
    }
    gen_goto_tb(ctx, 0, target);
    ctx->exception = EXCP_BRANCH;
}
