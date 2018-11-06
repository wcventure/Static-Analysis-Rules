GEN_HANDLER(l##width, opc, 0xFF, 0xFF, 0x00000000, PPC_INTEGER)               \
{                                                                             \
    gen_addr_imm_index(ctx);                                                  \
    op_ldst(l##width);                                                        \
    gen_op_store_T1_gpr(rD(ctx->opcode));                                     \
}
