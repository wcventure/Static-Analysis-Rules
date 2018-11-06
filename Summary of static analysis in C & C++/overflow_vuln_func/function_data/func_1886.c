void arm_sysctl_init(uint32_t base, uint32_t sys_id)
{
    arm_sysctl_state *s;
    int iomemtype;

    s = (arm_sysctl_state *)qemu_mallocz(sizeof(arm_sysctl_state));
    if (!s)
        return;
    s->base = base;
    s->sys_id = sys_id;
    iomemtype = cpu_register_io_memory(0, arm_sysctl_readfn,
                                       arm_sysctl_writefn, s);
    cpu_register_physical_memory(base, 0x00000fff, iomemtype);
    /
}
