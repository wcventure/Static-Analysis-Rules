void do_store_xer (void)
{
    xer_so = (T0 >> XER_SO) & 0x01;
    xer_ov = (T0 >> XER_OV) & 0x01;
    xer_ca = (T0 >> XER_CA) & 0x01;
    xer_cmp = (T0 >> XER_CMP) & 0xFF;
    xer_bc = (T0 >> XER_BC) & 0x3F;
}
