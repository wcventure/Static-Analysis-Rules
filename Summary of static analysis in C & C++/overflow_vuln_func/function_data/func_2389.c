GEN_HANDLER(tlbie, 0x1F, 0x12, 0x09, 0x03FF0001, PPC_MEM_TLBIE)
{
#if defined(CONFIG_USER_ONLY)
    RET_PRIVOPC(ctx);
#else
    if (unlikely(!ctx->supervisor)) {
        RET_PRIVOPC(ctx);
        return;
    }
    gen_op_load_gpr_T0(rB(ctx->opcode));
    gen_op_tlbie();
    RET_STOP(ctx);
#endif
}
