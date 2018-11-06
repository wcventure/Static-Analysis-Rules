void OPPROTO op_POWER_sre (void)
{
    T1 &= 0x1FUL;
    env->spr[SPR_MQ] = rotl32(T0, 32 - T1);
    T0 = Ts0 >> T1;
    RETURN();
}
