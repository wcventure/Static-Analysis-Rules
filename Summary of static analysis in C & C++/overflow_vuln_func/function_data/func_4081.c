static void codebook_trellis_rate(AACEncContext *s, SingleChannelElement *sce,
                                  int win, int group_len, const float lambda)
{
    BandCodingPath path[120][CB_TOT_ALL];
    int w, swb, cb, start, size;
    int i, j;
    const int max_sfb  = sce->ics.max_sfb;
    const int run_bits = sce->ics.num_windows == 1 ? 5 : 3;
    const int run_esc  = (1 << run_bits) - 1;
    int idx, ppos, count;
    int stackrun[120], stackcb[120], stack_len;
    float next_minbits = INFINITY;
    int next_mincb = 0;

    abs_pow34_v(s->scoefs, sce->coeffs, 1024);
    start = win*128;
    for (cb = 0; cb < CB_TOT_ALL; cb++) {
        path[0][cb].cost     = run_bits+4;
        path[0][cb].prev_idx = -1;
        path[0][cb].run      = 0;
    }
    for (swb = 0; swb < max_sfb; swb++) {
        size = sce->ics.swb_sizes[swb];
        if (sce->zeroes[win*16 + swb]) {
            float cost_stay_here = path[swb][0].cost;
            float cost_get_here  = next_minbits + run_bits + 4;
            if (   run_value_bits[sce->ics.num_windows == 8][path[swb][0].run]
                != run_value_bits[sce->ics.num_windows == 8][path[swb][0].run+1])
                cost_stay_here += run_bits;
            if (cost_get_here < cost_stay_here) {
                path[swb+1][0].prev_idx = next_mincb;
                path[swb+1][0].cost     = cost_get_here;
                path[swb+1][0].run      = 1;
            } else {
                path[swb+1][0].prev_idx = 0;
                path[swb+1][0].cost     = cost_stay_here;
                path[swb+1][0].run      = path[swb][0].run + 1;
            }
            next_minbits = path[swb+1][0].cost;
            next_mincb = 0;
            for (cb = 1; cb < CB_TOT_ALL; cb++) {
                path[swb+1][cb].cost = 61450;
                path[swb+1][cb].prev_idx = -1;
                path[swb+1][cb].run = 0;
            }
        } else {
            float minbits = next_minbits;
            int mincb = next_mincb;
            int startcb = sce->band_type[win*16+swb];
            startcb = aac_cb_in_map[startcb];
            next_minbits = INFINITY;
            next_mincb = 0;
            for (cb = 0; cb < startcb; cb++) {
                path[swb+1][cb].cost = 61450;
                path[swb+1][cb].prev_idx = -1;
                path[swb+1][cb].run = 0;
            }
            for (cb = startcb; cb < CB_TOT_ALL; cb++) {
                float cost_stay_here, cost_get_here;
                float bits = 0.0f;
                if (cb >= 12 && sce->band_type[win*16+swb] != aac_cb_out_map[cb]) {
                    path[swb+1][cb].cost = 61450;
                    path[swb+1][cb].prev_idx = -1;
                    path[swb+1][cb].run = 0;
                    continue;
                }
                for (w = 0; w < group_len; w++) {
                    bits += quantize_band_cost(s, sce->coeffs + start + w*128,
                                               s->scoefs + start + w*128, size,
                                               sce->sf_idx[win*16+swb],
                                               aac_cb_out_map[cb],
                                               0, INFINITY, NULL, 0);
                }
                cost_stay_here = path[swb][cb].cost + bits;
                cost_get_here  = minbits            + bits + run_bits + 4;
                if (   run_value_bits[sce->ics.num_windows == 8][path[swb][cb].run]
                    != run_value_bits[sce->ics.num_windows == 8][path[swb][cb].run+1])
                    cost_stay_here += run_bits;
                if (cost_get_here < cost_stay_here) {
                    path[swb+1][cb].prev_idx = mincb;
                    path[swb+1][cb].cost     = cost_get_here;
                    path[swb+1][cb].run      = 1;
                } else {
                    path[swb+1][cb].prev_idx = cb;
                    path[swb+1][cb].cost     = cost_stay_here;
                    path[swb+1][cb].run      = path[swb][cb].run + 1;
                }
                if (path[swb+1][cb].cost < next_minbits) {
                    next_minbits = path[swb+1][cb].cost;
                    next_mincb = cb;
                }
            }
        }
        start += sce->ics.swb_sizes[swb];
    }

    //convert resulting path from backward-linked list
    stack_len = 0;
    idx       = 0;
    for (cb = 1; cb < CB_TOT_ALL; cb++)
        if (path[max_sfb][cb].cost < path[max_sfb][idx].cost)
            idx = cb;
    ppos = max_sfb;
    while (ppos > 0) {
        av_assert1(idx >= 0);
        cb = idx;
        stackrun[stack_len] = path[ppos][cb].run;
        stackcb [stack_len] = cb;
        idx = path[ppos-path[ppos][cb].run+1][cb].prev_idx;
        ppos -= path[ppos][cb].run;
        stack_len++;
    }
    //perform actual band info encoding
    start = 0;
    for (i = stack_len - 1; i >= 0; i--) {
        cb = aac_cb_out_map[stackcb[i]];
        put_bits(&s->pb, 4, cb);
        count = stackrun[i];
        memset(sce->zeroes + win*16 + start, !cb, count);
        //XXX: memset when band_type is also uint8_t
        for (j = 0; j < count; j++) {
            sce->band_type[win*16 + start] = cb;
            start++;
        }
        while (count >= run_esc) {
            put_bits(&s->pb, run_bits, run_esc);
            count -= run_esc;
        }
        put_bits(&s->pb, run_bits, count);
    }
}
