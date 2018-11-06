void OPPROTO op_POWER_sraq (void)
{
    env->spr[SPR_MQ] = rotl32(T0, 32 - (T1 & 0x1FUL));
    if (T1 & 0x20UL)
        T0 = -1L;
    else
        T0 = Ts0 >> T1;
    RETURN();
}
