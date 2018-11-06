static void search_for_pns(AACEncContext *s, AVCodecContext *avctx, SingleChannelElement *sce)
{
    FFPsyBand *band;
    int w, g, w2, i;
    float *PNS = &s->scoefs[0*128], *PNS34 = &s->scoefs[1*128];
    float *NOR34 = &s->scoefs[3*128];
    const float lambda = s->lambda;
    const float freq_mult = avctx->sample_rate/(1024.0f/sce->ics.num_windows)/2.0f;
    const float thr_mult = NOISE_LAMBDA_REPLACE*(100.0f/lambda);
    const float spread_threshold = NOISE_SPREAD_THRESHOLD*FFMAX(0.5f, lambda/100.f);

    memcpy(sce->band_alt, sce->band_type, sizeof(sce->band_type));
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w]) {
        int wstart = sce->ics.swb_offset[w*16];
        for (g = 0;  g < sce->ics.num_swb; g++) {
            int noise_sfi;
            float dist1 = 0.0f, dist2 = 0.0f, noise_amp;
            float pns_energy = 0.0f, pns_tgt_energy, energy_ratio, dist_thresh;
            float sfb_energy = 0.0f, threshold = 0.0f, spread = 0.0f;
            const int start = sce->ics.swb_offset[w*16+g];
            const float freq = (start-wstart)*freq_mult;
            const float freq_boost = FFMAX(0.88f*freq/NOISE_LOW_LIMIT, 1.0f);
            if (freq < NOISE_LOW_LIMIT || avctx->cutoff && freq >= avctx->cutoff)
                continue;
            for (w2 = 0; w2 < sce->ics.group_len[w]; w2++) {
                band = &s->psy.ch[s->cur_channel].psy_bands[(w+w2)*16+g];
                sfb_energy += band->energy;
                spread     += band->spread;
                threshold  += band->threshold;
            }

            /
            dist_thresh = FFMIN(2.5f*NOISE_LOW_LIMIT/freq, 2.5f);

            /
            if (((sce->zeroes[w*16+g] || !sce->band_alt[w*16+g]) && sfb_energy < threshold*sqrtf(1.5f/freq_boost)) || spread < spread_threshold ||
                (sce->band_alt[w*16+g] && sfb_energy > threshold*thr_mult*freq_boost)) {
                sce->pns_ener[w*16+g] = sfb_energy;
                continue;
            }

            pns_tgt_energy = sfb_energy*spread*spread/sce->ics.group_len[w];
            noise_sfi = av_clip(roundf(log2f(pns_tgt_energy)*2), -100, 155); /
            noise_amp = -ff_aac_pow2sf_tab[noise_sfi + POW_SF2_ZERO];    /
            for (w2 = 0; w2 < sce->ics.group_len[w]; w2++) {
                float band_energy, scale, pns_senergy;
                const int start_c = sce->ics.swb_offset[(w+w2)*16+g];
                band = &s->psy.ch[s->cur_channel].psy_bands[(w+w2)*16+g];
                for (i = 0; i < sce->ics.swb_sizes[g]; i++)
                    PNS[i] = s->random_state = lcg_random(s->random_state);
                band_energy = s->fdsp->scalarproduct_float(PNS, PNS, sce->ics.swb_sizes[g]);
                scale = noise_amp/sqrtf(band_energy);
                s->fdsp->vector_fmul_scalar(PNS, PNS, scale, sce->ics.swb_sizes[g]);
                pns_senergy = s->fdsp->scalarproduct_float(PNS, PNS, sce->ics.swb_sizes[g]);
                pns_energy += pns_senergy;
                abs_pow34_v(NOR34, &sce->coeffs[start_c], sce->ics.swb_sizes[g]);
                abs_pow34_v(PNS34, PNS, sce->ics.swb_sizes[g]);
                dist1 += quantize_band_cost(s, &sce->coeffs[start_c],
                                            NOR34,
                                            sce->ics.swb_sizes[g],
                                            sce->sf_idx[(w+w2)*16+g],
                                            sce->band_alt[(w+w2)*16+g],
                                            lambda/band->threshold, INFINITY, NULL, 0);
                /
                dist2 += 9+band->energy/(band->spread*band->spread)*lambda/band->threshold;
            }
            energy_ratio = pns_tgt_energy/pns_energy; /
            sce->pns_ener[w*16+g] = energy_ratio*pns_tgt_energy;
            if (energy_ratio > 0.85f && energy_ratio < 1.25f && (sce->zeroes[w*16+g] || !sce->band_alt[w*16+g] || dist2*dist_thresh < dist1)) {
                sce->band_type[w*16+g] = NOISE_BT;
                sce->zeroes[w*16+g] = 0;
            }
        }
    }
}
