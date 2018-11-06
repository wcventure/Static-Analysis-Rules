static int lag_decode_prob(GetBitContext *gb, uint32_t *value)
{
    static const uint8_t series[] = { 1, 2, 3, 5, 8, 13, 21 };
    int i;
    int bit     = 0;
    int bits    = 0;
    int prevbit = 0;
    unsigned val;

    for (i = 0; i < 7; i++) {
        if (prevbit && bit)
            break;
        prevbit = bit;
        bit = get_bits1(gb);
        if (bit && !prevbit)
            bits += series[i];
    }
    bits--;
    if (bits < 0 || bits > 31) {
        *value = 0;
        return -1;
    } else if (bits == 0) {
        *value = 0;
        return 0;
    }

    val  = get_bits_long(gb, bits);
    val |= 1 << bits;

    *value = val - 1;

    return 0;
}
