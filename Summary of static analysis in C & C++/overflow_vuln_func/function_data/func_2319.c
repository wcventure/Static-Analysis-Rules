void do_mullwo (void)
{
    int64_t res = (int64_t)Ts0 * (int64_t)Ts1;

    if (likely((int32_t)res == res)) {
        xer_ov = 0;
    } else {
        xer_ov = 1;
        xer_so = 1;
    }
    T0 = (int32_t)res;
}
