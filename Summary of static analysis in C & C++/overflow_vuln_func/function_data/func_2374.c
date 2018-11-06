GEN_HANDLER(lswx, 0x1F, 0x15, 0x10, 0x00000001, PPC_INTEGER)
{
    int ra = rA(ctx->opcode);
    int rb = rB(ctx->opcode);

    /
    gen_op_update_nip(ctx->nip - 4);
    gen_addr_reg_index(ctx);
    if (ra == 0) {
        ra = rb;
    }
    gen_op_load_xer_bc();
    op_ldstsx(lswx, rD(ctx->opcode), ra, rb);
}
