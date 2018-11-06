GEN_HANDLER(bclr, 0x13, 0x10, 0x00, 0x00000000, PPC_FLOW)
{                                                                             
    gen_bcond(ctx, BCOND_LR);
}
