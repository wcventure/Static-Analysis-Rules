void ppc_store_xer (CPUPPCState *env, uint32_t value)
{
    xer_so = (value >> XER_SO) & 0x01;
    xer_ov = (value >> XER_OV) & 0x01;
    xer_ca = (value >> XER_CA) & 0x01;
    xer_cmp = (value >> XER_CMP) & 0xFF;
    xer_bc = (value >> XER_BC) & 0x3F;
}
