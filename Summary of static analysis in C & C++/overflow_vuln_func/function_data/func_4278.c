void ff_aac_search_for_ltp(AACEncContext *s, SingleChannelElement *sce,
                           int common_window)
{
    int w, g, w2, i, start = 0, count = 0;
    int saved_bits = -(15 + FFMIN(sce->ics.max_sfb, MAX_LTP_LONG_SFB));
    float *C34 = &s->scoefs[128*0], *PCD = &s->scoefs[128*1];
    float *PCD34 = &s->scoefs[128*2];
    const int max_ltp = FFMIN(sce->ics.max_sfb, MAX_LTP_LONG_SFB);

    if (sce->ics.window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        if (sce->ics.ltp.lag) {
            memset(&sce->lcoeffs[0], 0.0f, 3072*sizeof(sce->lcoeffs[0]));
            memset(&sce->ics.ltp, 0, sizeof(LongTermPrediction));
        }
        return;
    }

    if (!sce->ics.ltp.lag)
        return;

    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w]) {
        start = 0;
        for (g = 0;  g < sce->ics.num_swb; g++) {
            int bits1 = 0, bits2 = 0;
            float dist1 = 0.0f, dist2 = 0.0f;
            if (w*16+g > max_ltp) {
                start += sce->ics.swb_sizes[g];
                continue;
            }
            for (w2 = 0; w2 < sce->ics.group_len[w]; w2++) {
                int bits_tmp1, bits_tmp2;
                FFPsyBand *band = &s->psy.ch[s->cur_channel].psy_bands[(w+w2)*16+g];
                for (i = 0; i < sce->ics.swb_sizes[g]; i++)
                    PCD[i] = sce->coeffs[start+(w+w2)*128+i] - sce->lcoeffs[start+(w+w2)*128+i];
                abs_pow34_v(C34,  &sce->coeffs[start+(w+w2)*128],  sce->ics.swb_sizes[g]);
                abs_pow34_v(PCD34, PCD, sce->ics.swb_sizes[g]);
                dist1 += quantize_band_cost(s, &sce->coeffs[start+(w+w2)*128], C34, sce->ics.swb_sizes[g],
                                            sce->sf_idx[(w+w2)*16+g], sce->band_type[(w+w2)*16+g],
                                            s->lambda/band->threshold, INFINITY, &bits_tmp1, NULL, 0);
                dist2 += quantize_band_cost(s, PCD, PCD34, sce->ics.swb_sizes[g],
                                            sce->sf_idx[(w+w2)*16+g],
                                            sce->band_type[(w+w2)*16+g],
                                            s->lambda/band->threshold, INFINITY, &bits_tmp2, NULL, 0);
                bits1 += bits_tmp1;
                bits2 += bits_tmp2;
            }
            if (dist2 < dist1 && bits2 < bits1) {
                for (w2 = 0; w2 < sce->ics.group_len[w]; w2++)
                    for (i = 0; i < sce->ics.swb_sizes[g]; i++)
                        sce->coeffs[start+(w+w2)*128+i] -= sce->lcoeffs[start+(w+w2)*128+i];
                sce->ics.ltp.used[w*16+g] = 1;
                saved_bits += bits1 - bits2;
                count++;
            }
            start += sce->ics.swb_sizes[g];
        }
    }

    sce->ics.ltp.present = !!count && (saved_bits >= 0);
    sce->ics.predictor_present = !!sce->ics.ltp.present;

    /
    if (!sce->ics.ltp.present && !!count) {
        for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w]) {
            start = 0;
            for (g = 0;  g < sce->ics.num_swb; g++) {
                if (sce->ics.ltp.used[w*16+g]) {
                    for (w2 = 0; w2 < sce->ics.group_len[w]; w2++) {
                        for (i = 0; i < sce->ics.swb_sizes[g]; i++) {
                            sce->coeffs[start+(w+w2)*128+i] += sce->lcoeffs[start+(w+w2)*128+i];
                        }
                    }
                }
                start += sce->ics.swb_sizes[g];
            }
        }
    }
}
