void do_405_check_ov (void)
{
    if (likely(((T1 ^ T2) >> 31) || !((T0 ^ T2) >> 31))) {
        xer_ov = 0;
    } else {
        xer_ov = 1;
        xer_so = 1;
    }
}
