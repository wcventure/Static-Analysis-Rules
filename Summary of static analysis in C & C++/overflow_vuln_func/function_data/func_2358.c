GEN_HANDLER(cmpli, 0x0A, 0xFF, 0xFF, 0x00400000, PPC_INTEGER)
{
    gen_op_load_gpr_T0(rA(ctx->opcode));
    gen_op_cmpli(UIMM(ctx->opcode));
    gen_op_store_T0_crf(crfD(ctx->opcode));
}
