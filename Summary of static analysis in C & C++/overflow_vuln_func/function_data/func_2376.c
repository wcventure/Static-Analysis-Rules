GEN_HANDLER(stswx, 0x1F, 0x15, 0x14, 0x00000001, PPC_INTEGER)
{
    /
    gen_op_update_nip(ctx->nip - 4); 
    gen_addr_reg_index(ctx);
    gen_op_load_xer_bc();
    op_ldsts(stsw, rS(ctx->opcode));
}
