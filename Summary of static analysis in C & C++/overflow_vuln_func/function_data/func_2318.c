void do_divwo (void)
{
    if (likely(!((Ts0 == INT32_MIN && Ts1 == -1) || Ts1 == 0))) {
        xer_ov = 0;
        T0 = (Ts0 / Ts1);
    } else {
        xer_so = 1;
        xer_ov = 1;
        T0 = (-1) * ((uint32_t)T0 >> 31);
    }
}
