static void revert_acfilter(WmallDecodeCtx *s, int tile_size)
{
    int ich, pred, i, j;
    int16_t *filter_coeffs = s->acfilter_coeffs;
    int scaling            = s->acfilter_scaling;
    int order              = s->acfilter_order;

    for (ich = 0; ich < s->num_channels; ich++) {
        int *prevvalues = s->acfilter_prevvalues[ich];
        for (i = 0; i < order; i++) {
            pred = 0;
            for (j = 0; j < order; j++) {
                if (i <= j)
                    pred += filter_coeffs[j] * prevvalues[j - i];
                else
                    pred += s->channel_residues[ich][i - j - 1] * filter_coeffs[j];
            }
            pred >>= scaling;
            s->channel_residues[ich][i] += pred;
        }
        for (i = order; i < tile_size; i++) {
            pred = 0;
            for (j = 0; j < order; j++)
                pred += s->channel_residues[ich][i - j - 1] * filter_coeffs[j];
            pred >>= scaling;
            s->channel_residues[ich][i] += pred;
        }
        for (j = 0; j < order; j++)
            prevvalues[j] = s->channel_residues[ich][tile_size - j - 1];
    }
}
