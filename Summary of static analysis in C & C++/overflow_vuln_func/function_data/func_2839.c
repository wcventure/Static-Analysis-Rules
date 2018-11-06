uint64_t HELPER(neon_rshl_u64)(uint64_t val, uint64_t shiftop)
{
    int8_t shift = (uint8_t)shiftop;
    if (shift >= 64 || shift < 64) {
        val = 0;
    } else if (shift == -64) {
        /
        val >>= 63;
    } if (shift < 0) {
        val = (val + ((uint64_t)1 << (-1 - shift))) >> -shift;
        val >>= -shift;
    } else {
        val <<= shift;
    }
    return val;
}
