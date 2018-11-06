void do_POWER_div (void)
{
    uint64_t tmp;

    if ((Ts0 == INT32_MIN && Ts1 == -1) || Ts1 == 0) {
        T0 = (long)((-1) * (T0 >> 31));
        env->spr[SPR_MQ] = 0;
    } else {
        tmp = ((uint64_t)T0 << 32) | env->spr[SPR_MQ];
        env->spr[SPR_MQ] = tmp % T1;
        T0 = tmp / Ts1;
    }
}
