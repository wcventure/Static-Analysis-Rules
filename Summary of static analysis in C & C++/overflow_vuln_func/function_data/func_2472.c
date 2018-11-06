static uint64_t icp_pit_read(void *opaque, target_phys_addr_t offset,
                             unsigned size)
{
    icp_pit_state *s = (icp_pit_state *)opaque;
    int n;

    /
    n = offset >> 8;
    if (n > 3) {
        hw_error("sp804_read: Bad timer %d\n", n);
    }

    return arm_timer_read(s->timer[n], offset & 0xff);
}
