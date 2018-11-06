static void search_for_ms(AACEncContext *s, ChannelElement *cpe)
{
    int start = 0, i, w, w2, g;
    float M[128], S[128];
    float *L34 = s->scoefs, *R34 = s->scoefs + 128, *M34 = s->scoefs + 128*2, *S34 = s->scoefs + 128*3;
    const float lambda = s->lambda;
    SingleChannelElement *sce0 = &cpe->ch[0];
    SingleChannelElement *sce1 = &cpe->ch[1];
    if (!cpe->common_window)
        return;
    for (w = 0; w < sce0->ics.num_windows; w += sce0->ics.group_len[w]) {
        start = 0;
        for (g = 0;  g < sce0->ics.num_swb; g++) {
            if (!cpe->ch[0].zeroes[w*16+g] && !cpe->ch[1].zeroes[w*16+g] && !cpe->is_mask[w*16+g]) {
                float dist1 = 0.0f, dist2 = 0.0f;
                for (w2 = 0; w2 < sce0->ics.group_len[w]; w2++) {
                    FFPsyBand *band0 = &s->psy.ch[s->cur_channel+0].psy_bands[(w+w2)*16+g];
                    FFPsyBand *band1 = &s->psy.ch[s->cur_channel+1].psy_bands[(w+w2)*16+g];
                    float minthr = FFMIN(band0->threshold, band1->threshold);
                    float maxthr = FFMAX(band0->threshold, band1->threshold);
                    for (i = 0; i < sce0->ics.swb_sizes[g]; i++) {
                        M[i] = (sce0->pcoeffs[start+(w+w2)*128+i]
                              + sce1->pcoeffs[start+(w+w2)*128+i]) * 0.5;
                        S[i] =  M[i]
                              - sce1->pcoeffs[start+(w+w2)*128+i];
                    }
                    abs_pow34_v(L34, sce0->coeffs+start+(w+w2)*128, sce0->ics.swb_sizes[g]);
                    abs_pow34_v(R34, sce1->coeffs+start+(w+w2)*128, sce0->ics.swb_sizes[g]);
                    abs_pow34_v(M34, M,                         sce0->ics.swb_sizes[g]);
                    abs_pow34_v(S34, S,                         sce0->ics.swb_sizes[g]);
                    dist1 += quantize_band_cost(s, sce0->coeffs + start + (w+w2)*128,
                                                L34,
                                                sce0->ics.swb_sizes[g],
                                                sce0->sf_idx[(w+w2)*16+g],
                                                sce0->band_type[(w+w2)*16+g],
                                                lambda / band0->threshold, INFINITY, NULL, 0);
                    dist1 += quantize_band_cost(s, sce1->coeffs + start + (w+w2)*128,
                                                R34,
                                                sce1->ics.swb_sizes[g],
                                                sce1->sf_idx[(w+w2)*16+g],
                                                sce1->band_type[(w+w2)*16+g],
                                                lambda / band1->threshold, INFINITY, NULL, 0);
                    dist2 += quantize_band_cost(s, M,
                                                M34,
                                                sce0->ics.swb_sizes[g],
                                                sce0->sf_idx[(w+w2)*16+g],
                                                sce0->band_type[(w+w2)*16+g],
                                                lambda / maxthr, INFINITY, NULL, 0);
                    dist2 += quantize_band_cost(s, S,
                                                S34,
                                                sce1->ics.swb_sizes[g],
                                                sce1->sf_idx[(w+w2)*16+g],
                                                sce1->band_type[(w+w2)*16+g],
                                                lambda / minthr, INFINITY, NULL, 0);
                }
                cpe->ms_mask[w*16+g] = dist2 < dist1;
            }
            start += sce0->ics.swb_sizes[g];
        }
    }
}
