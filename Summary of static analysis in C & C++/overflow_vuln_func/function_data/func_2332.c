void do_POWER_abso (void)
{
    if (T0 == INT32_MIN) {
        T0 = INT32_MAX;
        xer_ov = 1;
        xer_so = 1;
    } else {
        T0 = -T0;
        xer_ov = 0;
    }
}
