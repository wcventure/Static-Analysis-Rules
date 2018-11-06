GEN_HANDLER(bc, 0x10, 0xFF, 0xFF, 0x00000000, PPC_FLOW)
{                                                                             
    gen_bcond(ctx, BCOND_IM);
}
