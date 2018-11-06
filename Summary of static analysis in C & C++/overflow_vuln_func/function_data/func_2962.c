uint64_t HELPER(neon_abdl_s32)(uint32_t a, uint32_t b)
{
    uint64_t tmp;
    uint64_t result;
    DO_ABD(result, a, b, int16_t);
    DO_ABD(tmp, a >> 16, b >> 16, int16_t);
    return result | (tmp << 32);
}
