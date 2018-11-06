static uint32_t hpet_ram_readl(void *opaque, target_phys_addr_t addr)
{
    HPETState *s = (HPETState *)opaque;
    uint64_t cur_tick, index;

    DPRINTF("qemu: Enter hpet_ram_readl at %" PRIx64 "\n", addr);
    index = addr;
    /
    if (index >= 0x100 && index <= 0x3ff) {
        uint8_t timer_id = (addr - 0x100) / 0x20;
        if (timer_id > HPET_NUM_TIMERS - 1) {
            printf("qemu: timer id out of range\n");
            return 0;
        }
        HPETTimer *timer = &s->timer[timer_id];

        switch ((addr - 0x100) % 0x20) {
            case HPET_TN_CFG:
                return timer->config;
            case HPET_TN_CFG + 4: // Interrupt capabilities
                return timer->config >> 32;
            case HPET_TN_CMP: // comparator register
                return timer->cmp;
            case HPET_TN_CMP + 4:
                return timer->cmp >> 32;
            case HPET_TN_ROUTE:
                return timer->fsb >> 32;
            default:
                DPRINTF("qemu: invalid hpet_ram_readl\n");
                break;
        }
    } else {
        switch (index) {
            case HPET_ID:
                return s->capability;
            case HPET_PERIOD:
                return s->capability >> 32;
            case HPET_CFG:
                return s->config;
            case HPET_CFG + 4:
                DPRINTF("qemu: invalid HPET_CFG + 4 hpet_ram_readl \n");
                return 0;
            case HPET_COUNTER:
                if (hpet_enabled())
                    cur_tick = hpet_get_ticks();
                else
                    cur_tick = s->hpet_counter;
                DPRINTF("qemu: reading counter  = %" PRIx64 "\n", cur_tick);
                return cur_tick;
            case HPET_COUNTER + 4:
                if (hpet_enabled())
                    cur_tick = hpet_get_ticks();
                else
                    cur_tick = s->hpet_counter;
                DPRINTF("qemu: reading counter + 4  = %" PRIx64 "\n", cur_tick);
                return cur_tick >> 32;
            case HPET_STATUS:
                return s->isr;
            default:
                DPRINTF("qemu: invalid hpet_ram_readl\n");
                break;
        }
    }
    return 0;
}
