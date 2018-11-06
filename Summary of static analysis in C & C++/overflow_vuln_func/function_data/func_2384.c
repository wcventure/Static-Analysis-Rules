GEN_HANDLER(tw, 0x1F, 0x04, 0x00, 0x00000001, PPC_FLOW)
{
    gen_op_load_gpr_T0(rA(ctx->opcode));
    gen_op_load_gpr_T1(rB(ctx->opcode));
    /
    gen_op_update_nip(ctx->nip);
    gen_op_tw(TO(ctx->opcode));
}
