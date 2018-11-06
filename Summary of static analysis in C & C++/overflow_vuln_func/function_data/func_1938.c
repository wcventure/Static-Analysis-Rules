static uint32_t hpet_time_after(uint64_t a, uint64_t b)
{
    return ((int32_t)(b) - (int32_t)(a) < 0);
}
