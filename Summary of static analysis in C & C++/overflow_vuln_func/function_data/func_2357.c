GEN_HANDLER(cmpi, 0x0B, 0xFF, 0xFF, 0x00400000, PPC_INTEGER)
{
    gen_op_load_gpr_T0(rA(ctx->opcode));
    gen_op_cmpi(SIMM(ctx->opcode));
    gen_op_store_T0_crf(crfD(ctx->opcode));
}
