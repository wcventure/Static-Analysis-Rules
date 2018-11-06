GEN_HANDLER(lswi, 0x1F, 0x15, 0x12, 0x00000001, PPC_INTEGER)
{
    int nb = NB(ctx->opcode);
    int start = rD(ctx->opcode);
    int ra = rA(ctx->opcode);
    int nr;

    if (nb == 0)
        nb = 32;
    nr = nb / 4;
    if (unlikely(((start + nr) > 32  &&
                  start <= ra && (start + nr - 32) > ra) ||
                 ((start + nr) <= 32 && start <= ra && (start + nr) > ra))) {
        RET_EXCP(ctx, EXCP_PROGRAM, EXCP_INVAL | EXCP_INVAL_LSWX);
        return;
    }
    /
    gen_op_update_nip(ctx->nip - 4);
    gen_addr_register(ctx);
    gen_op_set_T1(nb);
    op_ldsts(lswi, start);
}
