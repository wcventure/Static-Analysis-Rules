GEN_HANDLER(addic, 0x0C, 0xFF, 0xFF, 0x00000000, PPC_INTEGER)
{
    target_long simm = SIMM(ctx->opcode);

    gen_op_load_gpr_T0(rA(ctx->opcode));
    if (likely(simm != 0))
        gen_op_addic(SIMM(ctx->opcode));
    gen_op_store_T0_gpr(rD(ctx->opcode));
}
