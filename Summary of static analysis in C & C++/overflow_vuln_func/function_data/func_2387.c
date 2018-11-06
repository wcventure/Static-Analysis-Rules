GEN_HANDLER(mtmsr, 0x1F, 0x12, 0x04, 0x001FF801, PPC_MISC)
{
#if defined(CONFIG_USER_ONLY)
    RET_PRIVREG(ctx);
#else
    if (unlikely(!ctx->supervisor)) {
        RET_PRIVREG(ctx);
        return;
    }
    gen_op_update_nip((ctx)->nip);
    gen_op_load_gpr_T0(rS(ctx->opcode));
    gen_op_store_msr();
    /
    RET_CHG_FLOW(ctx);
#endif
}
