uint64_t HELPER(neon_qrshl_u64)(CPUState *env, uint64_t val, uint64_t shiftop)
{
    int8_t shift = (int8_t)shiftop;
    if (shift < 0) {
        val = (val + (1 << (-1 - shift))) >> -shift;
    } else { \
        uint64_t tmp = val;
        val <<= shift;
        if ((val >> shift) != tmp) {
            SET_QC();
            val = ~0;
        }
    }
    return val;
}
