GEN_HANDLER(name, opc1, opc2, opc3, inval, PPC_INTEGER)                       \
{                                                                             \
    gen_op_load_gpr_T0(rA(ctx->opcode));                                      \
    gen_op_load_gpr_T1(rB(ctx->opcode));                                      \
    gen_op_##name();                                                          \
    gen_op_store_T0_gpr(rD(ctx->opcode));                                     \
    if (unlikely(Rc(ctx->opcode) != 0))                                       \
        gen_set_Rc0(ctx);                                                     \
}
