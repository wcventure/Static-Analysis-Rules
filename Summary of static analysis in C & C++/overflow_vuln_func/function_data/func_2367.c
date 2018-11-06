GEN_HANDLER(st##width, opc, 0xFF, 0xFF, 0x00000000, PPC_INTEGER)              \
{                                                                             \
    gen_addr_imm_index(ctx);                                                  \
    gen_op_load_gpr_T1(rS(ctx->opcode));                                      \
    op_ldst(st##width);                                                       \
}
