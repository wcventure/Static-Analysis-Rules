static uint32_t pm_ioport_readw(void *opaque, uint32_t addr)
{
    VT686PMState *s = opaque;
    uint32_t val;

    addr &= 0x0f;
    switch (addr) {
    case 0x00:
        val = acpi_pm1_evt_get_sts(&s->ar, s->ar.tmr.overflow_time);
        break;
    case 0x02:
        val = s->ar.pm1.evt.en;
        break;
    case 0x04:
        val = s->ar.pm1.cnt.cnt;
        break;
    default:
        val = 0;
        break;
    }
    DPRINTF("PM readw port=0x%04x val=0x%02x\n", addr, val);
    return val;
}
