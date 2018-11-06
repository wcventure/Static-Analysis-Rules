static uint32_t hpet_time_after64(uint64_t a, uint64_t b)
{
    return ((int64_t)(b) - (int64_t)(a) < 0);
}
