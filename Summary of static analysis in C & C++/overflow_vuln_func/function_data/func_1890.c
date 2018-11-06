static qemu_irq *icp_pic_init(uint32_t base,
                              qemu_irq parent_irq, qemu_irq parent_fiq)
{
    icp_pic_state *s;
    int iomemtype;
    qemu_irq *qi;

    s = (icp_pic_state *)qemu_mallocz(sizeof(icp_pic_state));
    if (!s)
        return NULL;
    qi = qemu_allocate_irqs(icp_pic_set_irq, s, 32);
    s->base = base;
    s->parent_irq = parent_irq;
    s->parent_fiq = parent_fiq;
    iomemtype = cpu_register_io_memory(0, icp_pic_readfn,
                                       icp_pic_writefn, s);
    cpu_register_physical_memory(base, 0x007fffff, iomemtype);
    /
    return qi;
}
