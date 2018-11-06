uint64_t HELPER(neon_qrshl_s64)(CPUState *env, uint64_t valop, uint64_t shiftop)
{
    int8_t shift = (uint8_t)shiftop;
    int64_t val = valop;

    if (shift < 0) {
        val = (val + (1 << (-1 - shift))) >> -shift;
    } else {
        int64_t tmp = val;;
        val <<= shift;
        if ((val >> shift) != tmp) {
            SET_QC();
            val = tmp >> 31;
        }
    }
    return val;
}
