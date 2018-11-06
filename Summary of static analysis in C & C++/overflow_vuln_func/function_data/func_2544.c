static inline hwaddr
get_dma_address(uint32_t base, uint32_t x, uint32_t y)
{
    return base + 8 * (128 * y + x);
}
