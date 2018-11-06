void do_POWER_divs (void)
{
    if ((Ts0 == INT32_MIN && Ts1 == -1) || Ts1 == 0) {
        T0 = (long)((-1) * (T0 >> 31));
        env->spr[SPR_MQ] = 0;
    } else {
        env->spr[SPR_MQ] = T0 % T1;
        T0 = Ts0 / Ts1;
    }
}
