GEN_HANDLER(st##width##u, opc, 0xFF, 0xFF, 0x00000000, PPC_INTEGER)           \
{                                                                             \
    if (unlikely(rA(ctx->opcode) == 0)) {                                     \
        RET_INVAL(ctx);                                                       \
        return;                                                               \
    }                                                                         \
    gen_addr_imm_index(ctx);                                                  \
    gen_op_load_gpr_T1(rS(ctx->opcode));                                      \
    op_ldst(st##width);                                                       \
    gen_op_store_T0_gpr(rA(ctx->opcode));                                     \
}
