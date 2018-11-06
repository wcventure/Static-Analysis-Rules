static void icp_control_init(uint32_t base)
{
    int iomemtype;
    icp_control_state *s;

    s = (icp_control_state *)qemu_mallocz(sizeof(icp_control_state));
    iomemtype = cpu_register_io_memory(0, icp_control_readfn,
                                       icp_control_writefn, s);
    cpu_register_physical_memory(base, 0x007fffff, iomemtype);
    s->base = base;
    /
}
