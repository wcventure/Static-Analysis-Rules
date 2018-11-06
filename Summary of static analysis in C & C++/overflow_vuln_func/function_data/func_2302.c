void OPPROTO op_POWER_doz (void)
{
    if (Ts1 > Ts0)
        T0 = T1 - T0;
    else
        T0 = 0;
    RETURN();
}
