static void search_for_quantizers_faac(AVCodecContext *avctx, AACEncContext *s,
                                       SingleChannelElement *sce,
                                       const float lambda)
{
    int start = 0, i, w, w2, g;
    float uplim[128], maxq[128];
    int minq, maxsf;
    float distfact = ((sce->ics.num_windows > 1) ? 85.80 : 147.84) / lambda;
    int last = 0, lastband = 0, curband = 0;
    float avg_energy = 0.0;
    if (sce->ics.num_windows == 1) {
        start = 0;
        for (i = 0; i < 1024; i++) {
            if (i - start >= sce->ics.swb_sizes[curband]) {
                start += sce->ics.swb_sizes[curband];
                curband++;
            }
            if (sce->coeffs[i]) {
                avg_energy += sce->coeffs[i] * sce->coeffs[i];
                last = i;
                lastband = curband;
            }
        }
    } else {
        for (w = 0; w < 8; w++) {
            const float *coeffs = sce->coeffs + w*128;
            curband = start = 0;
            for (i = 0; i < 128; i++) {
                if (i - start >= sce->ics.swb_sizes[curband]) {
                    start += sce->ics.swb_sizes[curband];
                    curband++;
                }
                if (coeffs[i]) {
                    avg_energy += coeffs[i] * coeffs[i];
                    last = FFMAX(last, i);
                    lastband = FFMAX(lastband, curband);
                }
            }
        }
    }
    last++;
    avg_energy /= last;
    if (avg_energy == 0.0f) {
        for (i = 0; i < FF_ARRAY_ELEMS(sce->sf_idx); i++)
            sce->sf_idx[i] = SCALE_ONE_POS;
        return;
    }
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w]) {
        start = w*128;
        for (g = 0; g < sce->ics.num_swb; g++) {
            float *coefs   = sce->coeffs + start;
            const int size = sce->ics.swb_sizes[g];
            int start2 = start, end2 = start + size, peakpos = start;
            float maxval = -1, thr = 0.0f, t;
            maxq[w*16+g] = 0.0f;
            if (g > lastband) {
                maxq[w*16+g] = 0.0f;
                start += size;
                for (w2 = 0; w2 < sce->ics.group_len[w]; w2++)
                    memset(coefs + w2*128, 0, sizeof(coefs[0])*size);
                continue;
            }
            for (w2 = 0; w2 < sce->ics.group_len[w]; w2++) {
                for (i = 0; i < size; i++) {
                    float t = coefs[w2*128+i]*coefs[w2*128+i];
                    maxq[w*16+g] = FFMAX(maxq[w*16+g], fabsf(coefs[w2*128 + i]));
                    thr += t;
                    if (sce->ics.num_windows == 1 && maxval < t) {
                        maxval  = t;
                        peakpos = start+i;
                    }
                }
            }
            if (sce->ics.num_windows == 1) {
                start2 = FFMAX(peakpos - 2, start2);
                end2   = FFMIN(peakpos + 3, end2);
            } else {
                start2 -= start;
                end2   -= start;
            }
            start += size;
            thr = pow(thr / (avg_energy * (end2 - start2)), 0.3 + 0.1*(lastband - g) / lastband);
            t   = 1.0 - (1.0 * start2 / last);
            uplim[w*16+g] = distfact / (1.4 * thr + t*t*t + 0.075);
        }
    }
    memset(sce->sf_idx, 0, sizeof(sce->sf_idx));
    abs_pow34_v(s->scoefs, sce->coeffs, 1024);
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w]) {
        start = w*128;
        for (g = 0;  g < sce->ics.num_swb; g++) {
            const float *coefs  = sce->coeffs + start;
            const float *scaled = s->scoefs   + start;
            const int size      = sce->ics.swb_sizes[g];
            int scf, prev_scf, step;
            int min_scf = -1, max_scf = 256;
            float curdiff;
            if (maxq[w*16+g] < 21.544) {
                sce->zeroes[w*16+g] = 1;
                start += size;
                continue;
            }
            sce->zeroes[w*16+g] = 0;
            scf  = prev_scf = av_clip(SCALE_ONE_POS - SCALE_DIV_512 - log2f(1/maxq[w*16+g])*16/3, 60, 218);
            for (;;) {
                float dist = 0.0f;
                int quant_max;

                for (w2 = 0; w2 < sce->ics.group_len[w]; w2++) {
                    int b;
                    dist += quantize_band_cost(s, coefs + w2*128,
                                               scaled + w2*128,
                                               sce->ics.swb_sizes[g],
                                               scf,
                                               ESC_BT,
                                               lambda,
                                               INFINITY,
                                               &b,
                                               0);
                    dist -= b;
                }
                dist *= 1.0f / 512.0f / lambda;
                quant_max = quant(maxq[w*16+g], ff_aac_pow2sf_tab[POW_SF2_ZERO - scf + SCALE_ONE_POS - SCALE_DIV_512], ROUND_STANDARD);
                if (quant_max >= 8191) { // too much, return to the previous quantizer
                    sce->sf_idx[w*16+g] = prev_scf;
                    break;
                }
                prev_scf = scf;
                curdiff = fabsf(dist - uplim[w*16+g]);
                if (curdiff <= 1.0f)
                    step = 0;
                else
                    step = log2f(curdiff);
                if (dist > uplim[w*16+g])
                    step = -step;
                scf += step;
                scf = av_clip_uint8(scf);
                step = scf - prev_scf;
                if (FFABS(step) <= 1 || (step > 0 && scf >= max_scf) || (step < 0 && scf <= min_scf)) {
                    sce->sf_idx[w*16+g] = av_clip(scf, min_scf, max_scf);
                    break;
                }
                if (step > 0)
                    min_scf = prev_scf;
                else
                    max_scf = prev_scf;
            }
            start += size;
        }
    }
    minq = sce->sf_idx[0] ? sce->sf_idx[0] : INT_MAX;
    for (i = 1; i < 128; i++) {
        if (!sce->sf_idx[i])
            sce->sf_idx[i] = sce->sf_idx[i-1];
        else
            minq = FFMIN(minq, sce->sf_idx[i]);
    }
    if (minq == INT_MAX)
        minq = 0;
    minq = FFMIN(minq, SCALE_MAX_POS);
    maxsf = FFMIN(minq + SCALE_MAX_DIFF, SCALE_MAX_POS);
    for (i = 126; i >= 0; i--) {
        if (!sce->sf_idx[i])
            sce->sf_idx[i] = sce->sf_idx[i+1];
        sce->sf_idx[i] = av_clip(sce->sf_idx[i], minq, maxsf);
    }
}
