GEN_HANDLER(tlbiva, 0x1F, 0x12, 0x18, 0x03FFF801, PPC_BOOKE)
{
#if defined(CONFIG_USER_ONLY)
    RET_PRIVOPC(ctx);
#else
    if (unlikely(!ctx->supervisor)) {
        RET_PRIVOPC(ctx);
        return;
    }
    gen_addr_reg_index(ctx);
    /
    gen_op_tlbie();
    RET_STOP(ctx);
#endif
}
