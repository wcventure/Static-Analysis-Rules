void do_405_check_sat (void)
{
    if (!likely(((T1 ^ T2) >> 31) || !((T0 ^ T2) >> 31))) {
        /
        if (T2 >> 31) {
            T0 = INT32_MIN;
        } else {
            T0 = INT32_MAX;
        }
    }
}
