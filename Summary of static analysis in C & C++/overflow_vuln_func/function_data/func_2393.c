GEN_HANDLER(lfqux, 0x1F, 0x17, 0x19, 0x00000001, PPC_POWER2)
{
    int ra = rA(ctx->opcode);

    /
    gen_op_update_nip(ctx->nip - 4);
    gen_addr_reg_index(ctx);
    op_POWER2_lfq();
    gen_op_store_FT0_fpr(rD(ctx->opcode));
    gen_op_store_FT1_fpr(rD(ctx->opcode) + 1);
    if (ra != 0)
        gen_op_store_T0_gpr(ra);
}
