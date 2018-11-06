static void decode_subframe_lpc(ShortenContext *s, int channel, int residual_size, int pred_order)
{
    int sum, i, j;
    int coeffs[pred_order];

    for (i=0; i<pred_order; i++)
        coeffs[i] = get_sr_golomb_shorten(&s->gb, LPCQUANT);

    for (i=0; i < s->blocksize; i++) {
        sum = s->lpcqoffset;
        for (j=0; j<pred_order; j++)
            sum += coeffs[j] * s->decoded[channel][i-j-1];
        s->decoded[channel][i] = get_sr_golomb_shorten(&s->gb, residual_size) + (sum >> LPCQUANT);
    }
}
