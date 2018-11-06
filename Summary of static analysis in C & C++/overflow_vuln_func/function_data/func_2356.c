GEN_HANDLER(name, 0x1F, 0x00, opc, 0x00400000, PPC_INTEGER)                   \
{                                                                             \
    gen_op_load_gpr_T0(rA(ctx->opcode));                                      \
    gen_op_load_gpr_T1(rB(ctx->opcode));                                      \
    gen_op_##name();                                                          \
    gen_op_store_T0_crf(crfD(ctx->opcode));                                   \
}
