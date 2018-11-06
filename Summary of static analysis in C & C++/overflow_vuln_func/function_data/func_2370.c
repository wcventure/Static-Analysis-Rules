GEN_HANDLER(st##width##x, 0x1F, opc2, opc3, 0x00000001, PPC_INTEGER)          \
{                                                                             \
    gen_addr_reg_index(ctx);                                                  \
    gen_op_load_gpr_T1(rS(ctx->opcode));                                      \
    op_ldst(st##width);                                                       \
}
