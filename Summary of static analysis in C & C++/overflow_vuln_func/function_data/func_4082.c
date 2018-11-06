static void search_for_quantizers_anmr(AVCodecContext *avctx, AACEncContext *s,
                                       SingleChannelElement *sce,
                                       const float lambda)
{
    int q, w, w2, g, start = 0;
    int i, j;
    int idx;
    TrellisPath paths[TRELLIS_STAGES][TRELLIS_STATES];
    int bandaddr[TRELLIS_STAGES];
    int minq;
    float mincost;
    float q0f = FLT_MAX, q1f = 0.0f, qnrgf = 0.0f;
    int q0, q1, qcnt = 0;

    for (i = 0; i < 1024; i++) {
        float t = fabsf(sce->coeffs[i]);
        if (t > 0.0f) {
            q0f = FFMIN(q0f, t);
            q1f = FFMAX(q1f, t);
            qnrgf += t*t;
            qcnt++;
        }
    }

    if (!qcnt) {
        memset(sce->sf_idx, 0, sizeof(sce->sf_idx));
        memset(sce->zeroes, 1, sizeof(sce->zeroes));
        return;
    }

    //minimum scalefactor index is when minimum nonzero coefficient after quantizing is not clipped
    q0 = coef2minsf(q0f);
    //maximum scalefactor index is when maximum coefficient after quantizing is still not zero
    q1 = coef2maxsf(q1f);
    if (q1 - q0 > 60) {
        int q0low  = q0;
        int q1high = q1;
        //minimum scalefactor index is when maximum nonzero coefficient after quantizing is not clipped
        int qnrg = av_clip_uint8(log2f(sqrtf(qnrgf/qcnt))*4 - 31 + SCALE_ONE_POS - SCALE_DIV_512);
        q1 = qnrg + 30;
        q0 = qnrg - 30;
        if (q0 < q0low) {
            q1 += q0low - q0;
            q0  = q0low;
        } else if (q1 > q1high) {
            q0 -= q1 - q1high;
            q1  = q1high;
        }
    }

    for (i = 0; i < TRELLIS_STATES; i++) {
        paths[0][i].cost    = 0.0f;
        paths[0][i].prev    = -1;
    }
    for (j = 1; j < TRELLIS_STAGES; j++) {
        for (i = 0; i < TRELLIS_STATES; i++) {
            paths[j][i].cost    = INFINITY;
            paths[j][i].prev    = -2;
        }
    }
    idx = 1;
    abs_pow34_v(s->scoefs, sce->coeffs, 1024);
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w]) {
        start = w*128;
        for (g = 0; g < sce->ics.num_swb; g++) {
            const float *coefs = sce->coeffs + start;
            float qmin, qmax;
            int nz = 0;

            bandaddr[idx] = w * 16 + g;
            qmin = INT_MAX;
            qmax = 0.0f;
            for (w2 = 0; w2 < sce->ics.group_len[w]; w2++) {
                FFPsyBand *band = &s->psy.ch[s->cur_channel].psy_bands[(w+w2)*16+g];
                if (band->energy <= band->threshold || band->threshold == 0.0f) {
                    sce->zeroes[(w+w2)*16+g] = 1;
                    continue;
                }
                sce->zeroes[(w+w2)*16+g] = 0;
                nz = 1;
                for (i = 0; i < sce->ics.swb_sizes[g]; i++) {
                    float t = fabsf(coefs[w2*128+i]);
                    if (t > 0.0f)
                        qmin = FFMIN(qmin, t);
                    qmax = FFMAX(qmax, t);
                }
            }
            if (nz) {
                int minscale, maxscale;
                float minrd = INFINITY;
                float maxval;
                //minimum scalefactor index is when minimum nonzero coefficient after quantizing is not clipped
                minscale = coef2minsf(qmin);
                //maximum scalefactor index is when maximum coefficient after quantizing is still not zero
                maxscale = coef2maxsf(qmax);
                minscale = av_clip(minscale - q0, 0, TRELLIS_STATES - 1);
                maxscale = av_clip(maxscale - q0, 0, TRELLIS_STATES);
                maxval = find_max_val(sce->ics.group_len[w], sce->ics.swb_sizes[g], s->scoefs+start);
                for (q = minscale; q < maxscale; q++) {
                    float dist = 0;
                    int cb = find_min_book(maxval, sce->sf_idx[w*16+g]);
                    for (w2 = 0; w2 < sce->ics.group_len[w]; w2++) {
                        FFPsyBand *band = &s->psy.ch[s->cur_channel].psy_bands[(w+w2)*16+g];
                        dist += quantize_band_cost(s, coefs + w2*128, s->scoefs + start + w2*128, sce->ics.swb_sizes[g],
                                                   q + q0, cb, lambda / band->threshold, INFINITY, NULL, 0);
                    }
                    minrd = FFMIN(minrd, dist);

                    for (i = 0; i < q1 - q0; i++) {
                        float cost;
                        cost = paths[idx - 1][i].cost + dist
                               + ff_aac_scalefactor_bits[q - i + SCALE_DIFF_ZERO];
                        if (cost < paths[idx][q].cost) {
                            paths[idx][q].cost    = cost;
                            paths[idx][q].prev    = i;
                        }
                    }
                }
            } else {
                for (q = 0; q < q1 - q0; q++) {
                    paths[idx][q].cost = paths[idx - 1][q].cost + 1;
                    paths[idx][q].prev = q;
                }
            }
            sce->zeroes[w*16+g] = !nz;
            start += sce->ics.swb_sizes[g];
            idx++;
        }
    }
    idx--;
    mincost = paths[idx][0].cost;
    minq    = 0;
    for (i = 1; i < TRELLIS_STATES; i++) {
        if (paths[idx][i].cost < mincost) {
            mincost = paths[idx][i].cost;
            minq = i;
        }
    }
    while (idx) {
        sce->sf_idx[bandaddr[idx]] = minq + q0;
        minq = paths[idx][minq].prev;
        idx--;
    }
    //set the same quantizers inside window groups
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w])
        for (g = 0;  g < sce->ics.num_swb; g++)
            for (w2 = 1; w2 < sce->ics.group_len[w]; w2++)
                sce->sf_idx[(w+w2)*16+g] = sce->sf_idx[w*16+g];
}
