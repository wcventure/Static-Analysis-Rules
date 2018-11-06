GEN_HANDLER(mfcr, 0x1F, 0x13, 0x00, 0x00000801, PPC_MISC)
{
#if 0 // XXX: to be tested
    uint32_t crm, crn;
    
    if (likely(ctx->opcode & 0x00100000)) {
        crm = CRM(ctx->opcode);
        if (likely((crm ^ (crm - 1)) == 0)) {
            crn = ffs(crm);
            gen_op_load_cro(7 - crn);
        }
    } else
#endif
        {
            gen_op_load_cr();
        }
    gen_op_store_T0_gpr(rD(ctx->opcode));
}
