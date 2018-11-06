static void integratorcm_init(int memsz, uint32_t flash_offset)
{
    int iomemtype;
    integratorcm_state *s;

    s = (integratorcm_state *)qemu_mallocz(sizeof(integratorcm_state));
    s->cm_osc = 0x01000048;
    /
    s->cm_auxosc = 0x0007feff;
    s->cm_sdram = 0x00011122;
    if (memsz >= 256) {
        integrator_spd[31] = 64;
        s->cm_sdram |= 0x10;
    } else if (memsz >= 128) {
        integrator_spd[31] = 32;
        s->cm_sdram |= 0x0c;
    } else if (memsz >= 64) {
        integrator_spd[31] = 16;
        s->cm_sdram |= 0x08;
    } else if (memsz >= 32) {
        integrator_spd[31] = 4;
        s->cm_sdram |= 0x04;
    } else {
        integrator_spd[31] = 2;
    }
    memcpy(integrator_spd + 73, "QEMU-MEMORY", 11);
    s->cm_init = 0x00000112;
    s->flash_offset = flash_offset;

    iomemtype = cpu_register_io_memory(0, integratorcm_readfn,
                                       integratorcm_writefn, s);
    cpu_register_physical_memory(0x10000000, 0x007fffff, iomemtype);
    integratorcm_do_remap(s, 1);
    /
}
