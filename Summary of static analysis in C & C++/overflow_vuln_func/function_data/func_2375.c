GEN_HANDLER(stswi, 0x1F, 0x15, 0x16, 0x00000001, PPC_INTEGER)
{
    int nb = NB(ctx->opcode);

    /
    gen_op_update_nip(ctx->nip - 4);
    gen_addr_register(ctx);
    if (nb == 0)
        nb = 32;
    gen_op_set_T1(nb);
    op_ldsts(stsw, rS(ctx->opcode));
}
