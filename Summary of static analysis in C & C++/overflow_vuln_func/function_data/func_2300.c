#if !defined(CONFIG_USER_ONLY)
void cpu_ppc601_store_rtcl (CPUState *env, uint32_t value);
void OPPROTO op_store_601_rtcl (void)
{
    cpu_ppc601_store_rtcl(env, T0);
    RETURN();
}
