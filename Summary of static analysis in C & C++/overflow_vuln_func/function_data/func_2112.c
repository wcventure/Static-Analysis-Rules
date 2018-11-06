void do_405_check_ov (void)
{
    if (likely((((uint32_t)T1 ^ (uint32_t)T2) >> 31) ||
               !(((uint32_t)T0 ^ (uint32_t)T2) >> 31))) {
        xer_ov = 0;
    } else {
        xer_ov = 1;
        xer_so = 1;
    }
}
