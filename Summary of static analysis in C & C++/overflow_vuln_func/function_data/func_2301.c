void cpu_ppc601_store_rtcu (CPUState *env, uint32_t value);
void OPPROTO op_store_601_rtcu (void)
{
    cpu_ppc601_store_rtcu(env, T0);
    RETURN();
}
