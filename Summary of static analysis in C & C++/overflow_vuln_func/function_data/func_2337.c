void do_POWER_dozo (void)
{
    if (Ts1 > Ts0) {
        T2 = T0;
        T0 = T1 - T0;
        if (((~T2) ^ T1 ^ (-1)) & ((~T2) ^ T0) & (1 << 31)) {
            xer_so = 1;
            xer_ov = 1;
        } else {
            xer_ov = 0;
        }
    } else {
        T0 = 0;
        xer_ov = 0;
    }
}
