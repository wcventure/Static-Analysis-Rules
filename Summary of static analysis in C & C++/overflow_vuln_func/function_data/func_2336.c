void do_POWER_divso (void)
{
    if ((Ts0 == INT32_MIN && Ts1 == -1) || Ts1 == 0) {
        T0 = (long)((-1) * (T0 >> 31));
        env->spr[SPR_MQ] = 0;
        xer_ov = 1;
        xer_so = 1;
    } else {
        T0 = Ts0 / Ts1;
        env->spr[SPR_MQ] = Ts0 % Ts1;
        xer_ov = 0;
    }
}
