uint32_t cpu_ppc601_load_rtcl (CPUState *env);
void OPPROTO op_load_601_rtcl (void)
{
    T0 = cpu_ppc601_load_rtcl(env);
    RETURN();
}
