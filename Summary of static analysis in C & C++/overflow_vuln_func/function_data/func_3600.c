static int scale_vector(int16_t *vector, int length)
{
    int bits, max = 0;
    int64_t scale;
    int i;


    for (i = 0; i < length; i++)
        max = FFMAX(max, FFABS(vector[i]));

    max   = FFMIN(max, 0x7FFF);
    bits  = normalize_bits(max, 15);
    scale = (bits == 15) ? 0x7FFF : (1 << bits);

    for (i = 0; i < length; i++)
        vector[i] = av_clipl_int32(vector[i] * scale << 1) >> 4;

    return bits - 3;
}
