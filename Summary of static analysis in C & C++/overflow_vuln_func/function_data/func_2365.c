GEN_HANDLER(l##width##ux, 0x1F, 0x17, opc, 0x00000001, PPC_INTEGER)           \
{                                                                             \
    if (unlikely(rA(ctx->opcode) == 0 ||                                      \
                 rA(ctx->opcode) == rD(ctx->opcode))) {                       \
        RET_INVAL(ctx);                                                       \
        return;                                                               \
    }                                                                         \
    gen_addr_reg_index(ctx);                                                  \
    op_ldst(l##width);                                                        \
    gen_op_store_T1_gpr(rD(ctx->opcode));                                     \
    gen_op_store_T0_gpr(rA(ctx->opcode));                                     \
}
