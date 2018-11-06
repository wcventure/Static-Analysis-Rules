uint64_t HELPER(neon_abdl_u16)(uint32_t a, uint32_t b)
{
    uint64_t tmp;
    uint64_t result;
    DO_ABD(result, a, b, uint8_t);
    DO_ABD(tmp, a >> 8, b >> 8, uint8_t);
    result |= tmp << 16;
    DO_ABD(tmp, a >> 16, b >> 16, uint8_t);
    result |= tmp << 32;
    DO_ABD(tmp, a >> 24, b >> 24, uint8_t);
    result |= tmp << 48;
    return result;
}
