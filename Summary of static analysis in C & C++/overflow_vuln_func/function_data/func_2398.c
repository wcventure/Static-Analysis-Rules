GEN_HANDLER(stfqx, 0x1F, 0x17, 0x1C, 0x00000001, PPC_POWER2)
{
    /
    gen_op_update_nip(ctx->nip - 4);
    gen_addr_reg_index(ctx);
    gen_op_load_fpr_FT0(rS(ctx->opcode));
    gen_op_load_fpr_FT1(rS(ctx->opcode) + 1);
    op_POWER2_stfq();
}
