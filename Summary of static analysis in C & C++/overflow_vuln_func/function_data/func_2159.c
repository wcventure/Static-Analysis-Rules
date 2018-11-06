static int arm946_prbs_write(CPUARMState *env, const ARMCPRegInfo *ri,
                             uint64_t value)
{
    if (ri->crm > 8) {
        return EXCP_UDEF;
    }
    env->cp15.c6_region[ri->crm] = value;
    return 0;
}
