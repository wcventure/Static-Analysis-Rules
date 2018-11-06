static int arm946_prbs_read(CPUARMState *env, const ARMCPRegInfo *ri,
                            uint64_t *value)
{
    if (ri->crm > 8) {
        return EXCP_UDEF;
    }
    *value = env->cp15.c6_region[ri->crm];
    return 0;
}
