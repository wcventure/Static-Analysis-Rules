void store_40x_pit (CPUState *env, target_ulong val);
void OPPROTO op_store_40x_pit (void)
{
    store_40x_pit(env, T0);
    RETURN();
}
