GEN_HANDLER(srawi, 0x1F, 0x18, 0x19, 0x00000000, PPC_INTEGER)
{
    gen_op_load_gpr_T0(rS(ctx->opcode));
    if (SH(ctx->opcode) != 0)
        gen_op_srawi(SH(ctx->opcode), MASK(32 - SH(ctx->opcode), 31));
    gen_op_store_T0_gpr(rA(ctx->opcode));
    if (unlikely(Rc(ctx->opcode) != 0))
        gen_set_Rc0(ctx);
}
