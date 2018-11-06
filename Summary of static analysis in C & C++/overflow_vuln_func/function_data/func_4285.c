static void decode_array_0000(APEContext *ctx, GetBitContext *gb,
                              int32_t *out, APERice *rice, int blockstodecode)
{
    int i;
    int ksummax, ksummin;

    rice->ksum = 0;
    for (i = 0; i < 5; i++) {
        out[i] = get_rice_ook(&ctx->gb, 10);
        rice->ksum += out[i];
    }
    rice->k = av_log2(rice->ksum / 10) + 1;
    if (rice->k >= 24)
        return;
    for (; i < 64; i++) {
        out[i] = get_rice_ook(&ctx->gb, rice->k);
        rice->ksum += out[i];
        rice->k = av_log2(rice->ksum / ((i + 1) * 2)) + 1;
        if (rice->k >= 24)
            return;
    }
    ksummax = 1 << rice->k + 7;
    ksummin = rice->k ? (1 << rice->k + 6) : 0;
    for (; i < blockstodecode; i++) {
        out[i] = get_rice_ook(&ctx->gb, rice->k);
        rice->ksum += out[i] - out[i - 64];
        while (rice->ksum < ksummin) {
            rice->k--;
            ksummin = rice->k ? ksummin >> 1 : 0;
            ksummax >>= 1;
        }
        while (rice->ksum >= ksummax) {
            rice->k++;
            if (rice->k > 24)
                return;
            ksummax <<= 1;
            ksummin = ksummin ? ksummin << 1 : 128;
        }
    }

    for (i = 0; i < blockstodecode; i++)
        out[i] = ((out[i] >> 1) ^ ((out[i] & 1) - 1)) + 1;
}
