static uint32_t dcr_read_pob (void *opaque, int dcrn)
{
    ppc4xx_pob_t *pob;
    uint32_t ret;

    pob = opaque;
    switch (dcrn) {
    case POB0_BEAR:
        ret = pob->bear;
        break;
    case POB0_BESR0:
    case POB0_BESR1:
        ret = pob->besr[dcrn - POB0_BESR0];
        break;
    default:
        /
        ret = 0;
        break;
    }

    return ret;
}
