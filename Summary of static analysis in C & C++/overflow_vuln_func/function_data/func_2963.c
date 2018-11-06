uint64_t HELPER(neon_abdl_u64)(uint32_t a, uint32_t b)
{
    uint64_t result;
    DO_ABD(result, a, b, uint32_t);
    return result;
}
