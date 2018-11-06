void store_booke_tsr (CPUState *env, target_ulong val);
void OPPROTO op_store_booke_tsr (void)
{
    store_booke_tsr(env, T0);
    RETURN();
}
