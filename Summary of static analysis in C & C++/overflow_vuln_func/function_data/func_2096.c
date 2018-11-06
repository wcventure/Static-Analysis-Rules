static void ecc_mem_writel(void *opaque, target_phys_addr_t addr, uint32_t val)
{
    ECCState *s = opaque;

    switch (addr & ECC_ADDR_MASK) {
    case ECC_MER:
        s->regs[0] = (s->regs[0] & (ECC_MER_VER | ECC_MER_IMPL)) |
                     (val & ~(ECC_MER_VER | ECC_MER_IMPL));
        DPRINTF("Write memory enable %08x\n", val);
        break;
    case ECC_MDR:
        s->regs[1] =  val & ECC_MDR_MASK;
        DPRINTF("Write memory delay %08x\n", val);
        break;
    case ECC_MFSR:
        s->regs[2] =  val;
        DPRINTF("Write memory fault status %08x\n", val);
        break;
    case ECC_VCR:
        s->regs[3] =  val;
        DPRINTF("Write slot configuration %08x\n", val);
        break;
    case ECC_DR:
        s->regs[6] =  val;
        DPRINTF("Write diagnosiic %08x\n", val);
        break;
    case ECC_ECR0:
        s->regs[7] =  val;
        DPRINTF("Write event count 1 %08x\n", val);
        break;
    case ECC_ECR1:
        s->regs[7] =  val;
        DPRINTF("Write event count 2 %08x\n", val);
        break;
    }
}
