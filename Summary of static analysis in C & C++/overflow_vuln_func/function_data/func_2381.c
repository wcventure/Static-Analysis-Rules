GEN_HANDLER(bcctr, 0x13, 0x10, 0x10, 0x00000000, PPC_FLOW)
{                                                                             
    gen_bcond(ctx, BCOND_CTR);
}
