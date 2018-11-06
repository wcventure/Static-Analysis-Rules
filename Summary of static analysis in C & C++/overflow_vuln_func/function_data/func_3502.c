static uint32_t add_weights(uint32_t w1, uint32_t w2)
{
    uint32_t max = (w1 & 0xFF) > (w2 & 0xFF) ? (w1 & 0xFF) : (w2 & 0xFF);

    return ((w1 & 0xFFFFFF00) + (w2 & 0xFFFFFF00)) | (1 + max);
}
