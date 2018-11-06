GEN_HANDLER(l##width##u, opc, 0xFF, 0xFF, 0x00000000, PPC_INTEGER)            \
{                                                                             \
    if (unlikely(rA(ctx->opcode) == 0 ||                                      \
                 rA(ctx->opcode) == rD(ctx->opcode))) {                       \
        RET_INVAL(ctx);                                                       \
        return;                                                               \
    }                                                                         \
    gen_addr_imm_index(ctx);                                                  \
    op_ldst(l##width);                                                        \
    gen_op_store_T1_gpr(rD(ctx->opcode));                                     \
    gen_op_store_T0_gpr(rA(ctx->opcode));                                     \
}
