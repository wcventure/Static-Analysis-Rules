uint64_t HELPER(neon_abdl_s64)(uint32_t a, uint32_t b)
{
    uint64_t result;
    DO_ABD(result, a, b, int32_t);
    return result;
}
