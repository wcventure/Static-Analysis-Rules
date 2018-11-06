static void superio_ioport_writeb(void *opaque, hwaddr addr, uint64_t data,
                                  unsigned size)
{
    int can_write;
    SuperIOConfig *superio_conf = opaque;

    DPRINTF("superio_ioport_writeb  address 0x%x  val 0x%x\n", addr, data);
    if (addr == 0x3f0) {
        superio_conf->index = data & 0xff;
    } else {
        /
        switch (superio_conf->index) {
        case 0x00 ... 0xdf:
        case 0xe4:
        case 0xe5:
        case 0xe9 ... 0xed:
        case 0xf3:
        case 0xf5:
        case 0xf7:
        case 0xf9 ... 0xfb:
        case 0xfd ... 0xff:
            can_write = 0;
            break;
        default:
            can_write = 1;

            if (can_write) {
                switch (superio_conf->index) {
                case 0xe7:
                    if ((data & 0xff) != 0xfe) {
                        DPRINTF("chage uart 1 base. unsupported yet\n");
                    }
                    break;
                case 0xe8:
                    if ((data & 0xff) != 0xbe) {
                        DPRINTF("chage uart 2 base. unsupported yet\n");
                    }
                    break;

                default:
                    superio_conf->config[superio_conf->index] = data & 0xff;
                }
            }
        }
        superio_conf->config[superio_conf->index] = data & 0xff;
    }
}
