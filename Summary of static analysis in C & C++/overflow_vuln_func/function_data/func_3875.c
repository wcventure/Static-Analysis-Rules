static void qmf_32_subbands(DCAContext * s, int chans,
                            float samples_in[32][8], float *samples_out,
                            float scale)
{
    const float *prCoeff;
    int i;

    int sb_act = s->subband_activity[chans];
    int subindex;

    scale *= sqrt(1/8.0);

    /
    if (!s->multirate_inter)    /
        prCoeff = fir_32bands_nonperfect;
    else                        /
        prCoeff = fir_32bands_perfect;

    for (i = sb_act; i < 32; i++)
        s->raXin[i] = 0.0;

    /
    for (subindex = 0; subindex < 8; subindex++) {
        /
        for (i = 0; i < sb_act; i++){
            uint32_t v = AV_RN32A(&samples_in[i][subindex]) ^ ((i-1)&2)<<30;
            AV_WN32A(&s->raXin[i], v);
        }

        s->synth.synth_filter_float(&s->imdct,
                              s->subband_fir_hist[chans], &s->hist_index[chans],
                              s->subband_fir_noidea[chans], prCoeff,
                              samples_out, s->raXin, scale);
        samples_out+= 32;

    }
}
