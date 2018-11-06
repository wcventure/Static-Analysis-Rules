GEN_HANDLER(lscbx, 0x1F, 0x15, 0x08, 0x00000000, PPC_POWER_BR)
{
    int ra = rA(ctx->opcode);
    int rb = rB(ctx->opcode);

    gen_addr_reg_index(ctx);
    if (ra == 0) {
        ra = rb;
    }
    /
    gen_op_update_nip(ctx->nip - 4);
    gen_op_load_xer_bc();
    gen_op_load_xer_cmp();
    op_POWER_lscbx(rD(ctx->opcode), ra, rb);
    gen_op_store_xer_bc();
    if (unlikely(Rc(ctx->opcode) != 0))
        gen_set_Rc0(ctx);
}
