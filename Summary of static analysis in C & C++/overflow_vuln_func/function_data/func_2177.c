void do_POWER_dozo (void)
{
    if ((int32_t)T1 > (int32_t)T0) {
        T2 = T0;
        T0 = T1 - T0;
        if (((uint32_t)(~T2) ^ (uint32_t)T1 ^ UINT32_MAX) &
            ((uint32_t)(~T2) ^ (uint32_t)T0) & (1UL << 31)) {
            xer_so = 1;
            xer_ov = 1;
        } else {
            xer_ov = 0;
        }
    } else {
        T0 = 0;
        xer_ov = 0;
    }
}
