void do_subfe (void)
{
    T0 = T1 + ~T0 + xer_ca;
    if (likely(T0 >= T1 && (xer_ca == 0 || T0 != T1))) {
        xer_ca = 0;
    } else {
        xer_ca = 1;
    }
}
