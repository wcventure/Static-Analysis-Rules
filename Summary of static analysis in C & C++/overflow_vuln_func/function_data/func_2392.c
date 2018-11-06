GEN_HANDLER(lfqu, 0x39, 0xFF, 0xFF, 0x00000003, PPC_POWER2)
{
    int ra = rA(ctx->opcode);

    /
    gen_op_update_nip(ctx->nip - 4);
    gen_addr_imm_index(ctx);
    op_POWER2_lfq();
    gen_op_store_FT0_fpr(rD(ctx->opcode));
    gen_op_store_FT1_fpr(rD(ctx->opcode) + 1);
    if (ra != 0)
        gen_op_store_T0_gpr(ra);
}
