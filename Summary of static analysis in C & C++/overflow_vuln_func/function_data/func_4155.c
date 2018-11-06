static void chs_assemble_msbs_lsbs(DCAXllDecoder *s, DCAXllChSet *c, int band)
{
    DCAXllBand *b = &c->bands[band];
    int n, ch, nsamples = s->nframesamples;

    for (ch = 0; ch < c->nchannels; ch++) {
        int shift = chs_get_lsb_width(s, c, band, ch);
        if (shift) {
            int32_t *msb = b->msb_sample_buffer[ch];
            if (b->nscalablelsbs[ch]) {
                int32_t *lsb = b->lsb_sample_buffer[ch];
                int adj = b->bit_width_adjust[ch];
                for (n = 0; n < nsamples; n++)
                    msb[n] = msb[n] * (1 << shift) + (lsb[n] << adj);
            } else {
                for (n = 0; n < nsamples; n++)
                    msb[n] = msb[n] * (1 << shift);
            }
        }
    }
}
