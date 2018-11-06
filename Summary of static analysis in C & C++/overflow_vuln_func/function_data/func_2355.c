GEN_HANDLER(subfic, 0x08, 0xFF, 0xFF, 0x00000000, PPC_INTEGER)
{
    gen_op_load_gpr_T0(rA(ctx->opcode));
    gen_op_subfic(SIMM(ctx->opcode));
    gen_op_store_T0_gpr(rD(ctx->opcode));
}
