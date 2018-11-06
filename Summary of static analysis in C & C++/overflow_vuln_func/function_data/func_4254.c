static int read_quant_table(RangeCoder *c, int16_t *quant_table, int scale)
{
    int v;
    int i = 0;
    uint8_t state[CONTEXT_SIZE];

    memset(state, 128, sizeof(state));

    for (v = 0; i < 128; v++) {
        unsigned len = get_symbol(c, state, 0) + 1;

        if (len > 128 - i || !len)
            return AVERROR_INVALIDDATA;

        while (len--) {
            quant_table[i] = scale * v;
            i++;
        }
    }

    for (i = 1; i < 128; i++)
        quant_table[256 - i] = -quant_table[i];
    quant_table[128] = -quant_table[127];

    return 2 * v - 1;
}
