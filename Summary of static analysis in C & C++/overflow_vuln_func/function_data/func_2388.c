GEN_HANDLER(icbi, 0x1F, 0x16, 0x1E, 0x03E00001, PPC_CACHE)
{
    /
    gen_op_update_nip(ctx->nip - 4);
    gen_addr_reg_index(ctx);
    gen_op_icbi();
    RET_STOP(ctx);
}
