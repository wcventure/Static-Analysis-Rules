void qemu_bh_schedule(QEMUBH *bh)
{
    CPUState *env = cpu_single_env;
    if (bh->scheduled)
        return;
    bh->scheduled = 1;
    bh->next = first_bh;
    first_bh = bh;

    /
    if (env) {
        cpu_interrupt(env, CPU_INTERRUPT_EXIT);
    }
}
