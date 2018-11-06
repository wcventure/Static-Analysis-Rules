GEN_HANDLER(st##width##ux, 0x1F, 0x17, opc, 0x00000001, PPC_INTEGER)          \
{                                                                             \
    if (unlikely(rA(ctx->opcode) == 0)) {                                     \
        RET_INVAL(ctx);                                                       \
        return;                                                               \
    }                                                                         \
    gen_addr_reg_index(ctx);                                                  \
    gen_op_load_gpr_T1(rS(ctx->opcode));                                      \
    op_ldst(st##width);                                                       \
    gen_op_store_T0_gpr(rA(ctx->opcode));                                     \
}
