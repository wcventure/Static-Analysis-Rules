void do_divwuo (void)
{
    if (likely((uint32_t)T1 != 0)) {
        xer_ov = 0;
        T0 = (uint32_t)T0 / (uint32_t)T1;
    } else {
        xer_so = 1;
        xer_ov = 1;
        T0 = 0;
    }
}
