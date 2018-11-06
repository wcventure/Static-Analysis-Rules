GEN_HANDLER(name, 0x1F, 0x1A, opc, 0x00000000, PPC_INTEGER)                   \
{                                                                             \
    gen_op_load_gpr_T0(rS(ctx->opcode));                                      \
    gen_op_##name();                                                          \
    gen_op_store_T0_gpr(rA(ctx->opcode));                                     \
    if (unlikely(Rc(ctx->opcode) != 0))                                       \
        gen_set_Rc0(ctx);                                                     \
}
