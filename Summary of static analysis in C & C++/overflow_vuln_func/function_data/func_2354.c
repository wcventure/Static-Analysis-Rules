GEN_HANDLER(addis, 0x0F, 0xFF, 0xFF, 0x00000000, PPC_INTEGER)
{
    target_long simm = SIMM(ctx->opcode);

    if (rA(ctx->opcode) == 0) {
        /
        gen_op_set_T0(simm << 16);
    } else {
        gen_op_load_gpr_T0(rA(ctx->opcode));
        if (likely(simm != 0))
            gen_op_addi(simm << 16);
    }
    gen_op_store_T0_gpr(rD(ctx->opcode));
}
