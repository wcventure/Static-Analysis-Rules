uint64_t HELPER(neon_rshl_s64)(uint64_t valop, uint64_t shiftop)
{
    int8_t shift = (int8_t)shiftop;
    int64_t val = valop;
    if (shift >= 64) {
        val = 0;
    } else if (shift < -64) {
        val >>= 63;
    } else if (shift == -63) {
        val >>= 63;
        val++;
        val >>= 1;
    } else if (shift < 0) {
        val = (val + ((int64_t)1 << (-1 - shift))) >> -shift;
    } else {
        val <<= shift;
    }
    return val;
}
