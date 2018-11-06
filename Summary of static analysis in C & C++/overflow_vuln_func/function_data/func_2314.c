void do_adde (void)
{
    T2 = T0;
    T0 += T1 + xer_ca;
    if (likely(!(T0 < T2 || (xer_ca == 1 && T0 == T2)))) {
        xer_ca = 0;
    } else {
        xer_ca = 1;
    }
}
