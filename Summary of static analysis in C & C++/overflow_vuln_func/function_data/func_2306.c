target_ulong load_40x_pit (CPUState *env);
void OPPROTO op_load_40x_pit (void)
{
    T0 = load_40x_pit(env);
    RETURN();
}
