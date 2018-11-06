uint32_t div32(uint32_t *q_ptr, uint64_t num, uint32_t den)
{
    *q_ptr = num / den;
    return num % den;
}
