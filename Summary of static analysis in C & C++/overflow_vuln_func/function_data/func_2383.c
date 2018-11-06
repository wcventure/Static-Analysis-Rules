GEN_HANDLER(rfi, 0x13, 0x12, 0x01, 0x03FF8001, PPC_FLOW)
{
#if defined(CONFIG_USER_ONLY)
    RET_PRIVOPC(ctx);
#else
    /
    if (unlikely(!ctx->supervisor)) {
        RET_PRIVOPC(ctx);
        return;
    }
    gen_op_rfi();
    RET_CHG_FLOW(ctx);
#endif
}
