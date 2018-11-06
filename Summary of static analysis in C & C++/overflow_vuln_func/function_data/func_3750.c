static void create_default_qtables(uint8_t *qtables, uint8_t q)
{
    int factor = q;
    int i;

    factor = av_clip(q, 1, 99);

    if (q < 50)
        q = 5000 / factor;
    else
        q = 200 - factor * 2;

    for (i = 0; i < 128; i++) {
        int val = (default_quantizers[i] * q + 50) / 100;

        /
        val = av_clip(val, 1, 255);
        qtables[i] = val;
    }
}
