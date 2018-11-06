GEN_HANDLER(lmw, 0x2E, 0xFF, 0xFF, 0x00000000, PPC_INTEGER)
{
    /
    gen_op_update_nip(ctx->nip - 4);
    gen_addr_imm_index(ctx);
    op_ldstm(lmw, rD(ctx->opcode));
}
