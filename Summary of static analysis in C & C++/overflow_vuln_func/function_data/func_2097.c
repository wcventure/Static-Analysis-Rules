static uint32_t ecc_mem_readl(void *opaque, target_phys_addr_t addr)
{
    ECCState *s = opaque;
    uint32_t ret = 0;

    switch (addr & ECC_ADDR_MASK) {
    case ECC_MER:
        ret = s->regs[0];
        DPRINTF("Read memory enable %08x\n", ret);
        break;
    case ECC_MDR:
        ret = s->regs[1];
        DPRINTF("Read memory delay %08x\n", ret);
        break;
    case ECC_MFSR:
        ret = s->regs[2];
        DPRINTF("Read memory fault status %08x\n", ret);
        break;
    case ECC_VCR:
        ret = s->regs[3];
        DPRINTF("Read slot configuration %08x\n", ret);
        break;
    case ECC_MFAR0:
        ret = s->regs[4];
        DPRINTF("Read memory fault address 0 %08x\n", ret);
        break;
    case ECC_MFAR1:
        ret = s->regs[5];
        DPRINTF("Read memory fault address 1 %08x\n", ret);
        break;
    case ECC_DR:
        ret = s->regs[6];
        DPRINTF("Read diagnostic %08x\n", ret);
        break;
    case ECC_ECR0:
        ret = s->regs[7];
        DPRINTF("Read event count 1 %08x\n", ret);
        break;
    case ECC_ECR1:
        ret = s->regs[7];
        DPRINTF("Read event count 2 %08x\n", ret);
        break;
    }
    return ret;
}
