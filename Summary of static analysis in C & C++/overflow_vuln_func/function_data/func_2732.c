static void ppc4xx_pob_reset (void *opaque)
{
    ppc4xx_pob_t *pob;

    pob = opaque;
    /
    pob->bear = 0x00000000;
    pob->besr[0] = 0x0000000;
    pob->besr[1] = 0x0000000;
}
