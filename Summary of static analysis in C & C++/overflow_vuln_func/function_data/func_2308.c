void store_booke_tcr (CPUState *env, target_ulong val);
void OPPROTO op_store_booke_tcr (void)
{
    store_booke_tcr(env, T0);
    RETURN();
}
