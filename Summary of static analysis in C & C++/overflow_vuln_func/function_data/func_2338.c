void do_POWER_maskg (void)
{
    uint32_t ret;

    if (T0 == T1 + 1) {
        ret = -1;
    } else {
        ret = (((uint32_t)(-1)) >> (T0)) ^
            (((uint32_t)(-1) >> (T1)) >> 1);
        if (T0 > T1)
            ret = ~ret;
    }
    T0 = ret;
}
