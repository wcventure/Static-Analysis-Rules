#if defined(TARGET_PPC)
static target_long monitor_get_ccr (const struct MonitorDef *md, int val)
{
    CPUArchState *env = mon_get_cpu();
    unsigned int u;
    int i;

    u = 0;
    for (i = 0; i < 8; i++)
        u |= env->crf[i] << (32 - (4 * i));

    return u;
}
