static void cg3_reg_write(void *opaque, hwaddr addr, uint64_t val,
                          unsigned size)
{
    CG3State *s = opaque;
    uint8_t regval;
    int i;

    DPRINTF("write %" PRIx64 " to reg %" HWADDR_PRIx " size %d\n",
            val, addr, size);

    switch (addr) {
    case CG3_REG_BT458_ADDR:
        s->dac_index = val;
        s->dac_state = 0;
        break;
    case CG3_REG_BT458_COLMAP:
        /
        if (size == 1) {
            val <<= 24;
        }

        for (i = 0; i < size; i++) {
            regval = val >> 24;

            switch (s->dac_state) {
            case 0:
                s->r[s->dac_index] = regval;
                s->dac_state++;
                break;
            case 1:
                s->g[s->dac_index] = regval;
                s->dac_state++;
                break;
            case 2:
                s->b[s->dac_index] = regval;
                /
                s->dac_index = (s->dac_index + 1) & 0xff;
            default:
                s->dac_state = 0;
                break;
            }
            val <<= 8;
        }
        s->full_update = 1;
        break;
    case CG3_REG_FBC_CTRL:
        s->regs[0] = val;
        break;
    case CG3_REG_FBC_STATUS:
        if (s->regs[1] & CG3_SR_PENDING_INT) {
            /
            s->regs[1] &= ~CG3_SR_PENDING_INT;
            qemu_irq_lower(s->irq);
        }
        break;
    case CG3_REG_FBC_CURSTART ... CG3_REG_SIZE:
        s->regs[addr - 0x10] = val;
        break;
    default:
        qemu_log_mask(LOG_UNIMP,
                  "cg3: Unimplemented register write "
                  "reg 0x%" HWADDR_PRIx " size 0x%x value 0x%" PRIx64 "\n",
                  addr, size, val);
        break;
    }
}
