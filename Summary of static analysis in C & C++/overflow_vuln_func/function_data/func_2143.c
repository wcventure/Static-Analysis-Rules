static uint32_t cc_calc_abs_64(int64_t dst)
{
    if ((uint64_t)dst == 0x8000000000000000ULL) {
        return 3;
    } else if (dst) {
        return 1;
    } else {
        return 0;
    }
}
