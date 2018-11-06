GEN_HANDLER(tlbwe, 0x1F, 0x12, 0x1E, 0x00000001, PPC_EMB_COMMON)
{
#if defined(CONFIG_USER_ONLY)
    RET_PRIVOPC(ctx);
#else
    if (unlikely(!ctx->supervisor)) {
        RET_PRIVOPC(ctx);
        return;
    }
    switch (rB(ctx->opcode)) {
    case 0:
        gen_op_load_gpr_T0(rA(ctx->opcode));
        gen_op_load_gpr_T1(rS(ctx->opcode));
        gen_op_4xx_tlbwe_hi();
        break;
    case 1:
        gen_op_load_gpr_T0(rA(ctx->opcode));
        gen_op_load_gpr_T1(rS(ctx->opcode));
        gen_op_4xx_tlbwe_lo();
        break;
    default:
        RET_INVAL(ctx);
        break;
    }
#endif
}
