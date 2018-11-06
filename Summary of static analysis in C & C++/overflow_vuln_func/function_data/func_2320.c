void do_nego (void)
{
    if (likely(T0 != INT32_MIN)) {
        xer_ov = 0;
        T0 = -Ts0;
    } else {
        xer_ov = 1;
        xer_so = 1;
    }
}
