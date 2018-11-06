static uint32_t cc_calc_abs_32(int32_t dst)
{
    if ((uint32_t)dst == 0x80000000UL) {
        return 3;
    } else if (dst) {
        return 1;
    } else {
        return 0;
    }
}
