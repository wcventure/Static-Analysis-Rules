GEN_HANDLER(twi, 0x03, 0xFF, 0xFF, 0x00000000, PPC_FLOW)
{
    gen_op_load_gpr_T0(rA(ctx->opcode));
    gen_op_set_T1(SIMM(ctx->opcode));
    gen_op_tw(TO(ctx->opcode));
}
