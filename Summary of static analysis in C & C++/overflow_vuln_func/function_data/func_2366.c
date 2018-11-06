GEN_HANDLER(l##width##x, 0x1F, opc2, opc3, 0x00000001, PPC_INTEGER)           \
{                                                                             \
    gen_addr_reg_index(ctx);                                                  \
    op_ldst(l##width);                                                        \
    gen_op_store_T1_gpr(rD(ctx->opcode));                                     \
}
