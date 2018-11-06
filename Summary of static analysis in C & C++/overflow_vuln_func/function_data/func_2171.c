void do_divwo (void)
{
    if (likely(!(((int32_t)T0 == INT32_MIN && (int32_t)T1 == -1) ||
                 (int32_t)T1 == 0))) {
        xer_ov = 0;
        T0 = (int32_t)T0 / (int32_t)T1;
    } else {
        xer_so = 1;
        xer_ov = 1;
        T0 = (-1) * ((uint32_t)T0 >> 31);
    }
}
