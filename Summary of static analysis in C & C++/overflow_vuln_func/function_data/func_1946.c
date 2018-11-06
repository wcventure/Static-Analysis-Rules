static OfDpaFlow *of_dpa_flow_alloc(uint64_t cookie)
{
    OfDpaFlow *flow;
    int64_t now = qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) / 1000;

    flow = g_malloc0(sizeof(OfDpaFlow));
    if (!flow) {
        return NULL;
    }

    flow->cookie = cookie;
    flow->mask.tbl_id = 0xffffffff;

    flow->stats.install_time = flow->stats.refresh_time = now;

    return flow;
}
