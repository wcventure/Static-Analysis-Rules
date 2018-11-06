static uint64_t cg3_reg_read(void *opaque, hwaddr addr, unsigned size)
{
    CG3State *s = opaque;
    int val;

    switch (addr) {
    case CG3_REG_BT458_ADDR:
    case CG3_REG_BT458_COLMAP:
        val = 0;
        break;
    case CG3_REG_FBC_CTRL:
        val = s->regs[0];
        break;
    case CG3_REG_FBC_STATUS:
        /
        val = s->regs[1] | CG3_SR_1152_900_76_B | CG3_SR_ID_COLOR;
        break;
    case CG3_REG_FBC_CURSTART ... CG3_REG_SIZE:
        val = s->regs[addr - 0x10];
        break;
    default:
        qemu_log_mask(LOG_UNIMP,
                  "cg3: Unimplemented register read "
                  "reg 0x%" HWADDR_PRIx " size 0x%x\n",
                  addr, size);
        val = 0;
        break;
    }
    DPRINTF("read %02x from reg %" HWADDR_PRIx "\n", val, addr);
    return val;
}
