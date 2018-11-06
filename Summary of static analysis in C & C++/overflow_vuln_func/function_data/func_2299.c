uint32_t cpu_ppc601_load_rtcu (CPUState *env);
void OPPROTO op_load_601_rtcu (void)
{
    T0 = cpu_ppc601_load_rtcu(env);
    RETURN();
}
