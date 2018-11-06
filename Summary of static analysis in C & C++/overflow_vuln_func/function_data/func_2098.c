static void ecc_reset(void *opaque)
{
    ECCState *s = opaque;
    int i;

    s->regs[ECC_MER] &= (ECC_MER_VER | ECC_MER_IMPL);
    s->regs[ECC_MER] |= ECC_MER_MRR;
    s->regs[ECC_MDR] = 0x20;
    s->regs[ECC_MFSR] = 0;
    s->regs[ECC_VCR] = 0;
    s->regs[ECC_MFAR0] = 0x07c00000;
    s->regs[ECC_MFAR1] = 0;
    s->regs[ECC_DR] = 0;
    s->regs[ECC_ECR0] = 0;
    s->regs[ECC_ECR1] = 0;

    for (i = 1; i < ECC_NREGS; i++)
        s->regs[i] = 0;
}
