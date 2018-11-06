int32_t idiv32(int32_t *q_ptr, int64_t num, int32_t den)
{
    *q_ptr = num / den;
    return num % den;
}
