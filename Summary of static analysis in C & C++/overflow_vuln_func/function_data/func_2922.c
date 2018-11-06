static void zynq_xadc_write(void *opaque, hwaddr offset, uint64_t val,
                            unsigned size)
{
    ZynqXADCState *s = (ZynqXADCState *)opaque;
    int reg = offset / 4;
    int xadc_reg;
    int xadc_cmd;
    int xadc_data;

    if (!zynq_xadc_check_offset(reg, false)) {
        qemu_log_mask(LOG_GUEST_ERROR, "zynq_xadc: Invalid write access "
                      "to addr %" HWADDR_PRIx "\n", offset);
        return;
    }

    switch (reg) {
    case CFG:
        s->regs[CFG] = val;
        break;
    case INT_STS:
        s->regs[INT_STS] &= ~val;
        break;
    case INT_MASK:
        s->regs[INT_MASK] = val & INT_ALL;
        break;
    case CMDFIFO:
        xadc_cmd  = extract32(val, 26,  4);
        xadc_reg  = extract32(val, 16, 10);
        xadc_data = extract32(val,  0, 16);

        if (s->regs[MCTL] & MCTL_RESET) {
            qemu_log_mask(LOG_GUEST_ERROR, "zynq_xadc: Sending command "
                          "while comm channel held in reset: %" PRIx32 "\n",
                          (uint32_t) val);
            break;
        }

        if (xadc_reg > ZYNQ_XADC_NUM_ADC_REGS && xadc_cmd != CMD_NOP) {
            qemu_log_mask(LOG_GUEST_ERROR, "read/write op to invalid xadc "
                          "reg 0x%x\n", xadc_reg);
            break;
        }

        switch (xadc_cmd) {
        case CMD_READ:
            xadc_push_dfifo(s, s->xadc_regs[xadc_reg]);
            break;
        case CMD_WRITE:
            s->xadc_regs[xadc_reg] = xadc_data;
            /
        case CMD_NOP:
            xadc_push_dfifo(s, 0);
            break;
        }
        break;
    case MCTL:
        s->regs[MCTL] = val & 0x00fffeff;
        break;
    }
    zynq_xadc_update_ints(s);
}
