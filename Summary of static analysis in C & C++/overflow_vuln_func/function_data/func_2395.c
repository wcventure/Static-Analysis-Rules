GEN_HANDLER(stfq, 0x3C, 0xFF, 0xFF, 0x00000003, PPC_POWER2)
{
    /
    gen_op_update_nip(ctx->nip - 4);
    gen_addr_imm_index(ctx);
    gen_op_load_fpr_FT0(rS(ctx->opcode));
    gen_op_load_fpr_FT1(rS(ctx->opcode) + 1);
    op_POWER2_stfq();
}
