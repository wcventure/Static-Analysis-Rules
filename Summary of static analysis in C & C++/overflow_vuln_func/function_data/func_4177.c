int ff_hevc_decode_short_term_rps(HEVCContext *s, ShortTermRPS *rps,
                                  const HEVCSPS *sps, int is_slice_header)
{
    HEVCLocalContext *lc = &s->HEVClc;
    uint8_t rps_predict = 0;
    int delta_poc;
    int k0 = 0;
    int k1 = 0;
    int k  = 0;
    int i;

    GetBitContext *gb = &lc->gb;

    if (rps != sps->st_rps && sps->nb_st_rps)
        rps_predict = get_bits1(gb);

    if (rps_predict) {
        const ShortTermRPS *rps_ridx;
        int delta_rps, abs_delta_rps;
        uint8_t use_delta_flag = 0;
        uint8_t delta_rps_sign;

        if (is_slice_header) {
            int delta_idx = get_ue_golomb_long(gb) + 1;
            if (delta_idx > sps->nb_st_rps) {
                av_log(s->avctx, AV_LOG_ERROR,
                       "Invalid value of delta_idx in slice header RPS: %d > %d.\n",
                       delta_idx, sps->nb_st_rps);
                return AVERROR_INVALIDDATA;
            }
            rps_ridx = &sps->st_rps[sps->nb_st_rps - delta_idx];
        } else
            rps_ridx = &sps->st_rps[rps - sps->st_rps - 1];

        delta_rps_sign = get_bits1(gb);
        abs_delta_rps  = get_ue_golomb_long(gb) + 1;
        delta_rps      = (1 - (delta_rps_sign << 1)) * abs_delta_rps;
        for (i = 0; i <= rps_ridx->num_delta_pocs; i++) {
            int used = rps->used[k] = get_bits1(gb);

            if (!used)
                use_delta_flag = get_bits1(gb);

            if (used || use_delta_flag) {
                if (i < rps_ridx->num_delta_pocs)
                    delta_poc = delta_rps + rps_ridx->delta_poc[i];
                else
                    delta_poc = delta_rps;
                rps->delta_poc[k] = delta_poc;
                if (delta_poc < 0)
                    k0++;
                else
                    k1++;
                k++;
            }
        }

        rps->num_delta_pocs    = k;
        rps->num_negative_pics = k0;
        // sort in increasing order (smallest first)
        if (rps->num_delta_pocs != 0) {
            int used, tmp;
            for (i = 1; i < rps->num_delta_pocs; i++) {
                delta_poc = rps->delta_poc[i];
                used      = rps->used[i];
                for (k = i - 1; k >= 0; k--) {
                    tmp = rps->delta_poc[k];
                    if (delta_poc < tmp) {
                        rps->delta_poc[k + 1] = tmp;
                        rps->used[k + 1]      = rps->used[k];
                        rps->delta_poc[k]     = delta_poc;
                        rps->used[k]          = used;
                    }
                }
            }
        }
        if ((rps->num_negative_pics >> 1) != 0) {
            int used;
            k = rps->num_negative_pics - 1;
            // flip the negative values to largest first
            for (i = 0; i < rps->num_negative_pics >> 1; i++) {
                delta_poc         = rps->delta_poc[i];
                used              = rps->used[i];
                rps->delta_poc[i] = rps->delta_poc[k];
                rps->used[i]      = rps->used[k];
                rps->delta_poc[k] = delta_poc;
                rps->used[k]      = used;
                k--;
            }
        }
    } else {
        unsigned int prev, nb_positive_pics;
        rps->num_negative_pics = get_ue_golomb_long(gb);
        nb_positive_pics       = get_ue_golomb_long(gb);

        if (rps->num_negative_pics >= MAX_REFS ||
            nb_positive_pics >= MAX_REFS) {
            av_log(s->avctx, AV_LOG_ERROR, "Too many refs in a short term RPS.\n");
            return AVERROR_INVALIDDATA;
        }

        rps->num_delta_pocs = rps->num_negative_pics + nb_positive_pics;
        if (rps->num_delta_pocs) {
            prev = 0;
            for (i = 0; i < rps->num_negative_pics; i++) {
                delta_poc = get_ue_golomb_long(gb) + 1;
                prev -= delta_poc;
                rps->delta_poc[i] = prev;
                rps->used[i]      = get_bits1(gb);
            }
            prev = 0;
            for (i = 0; i < nb_positive_pics; i++) {
                delta_poc = get_ue_golomb_long(gb) + 1;
                prev += delta_poc;
                rps->delta_poc[rps->num_negative_pics + i] = prev;
                rps->used[rps->num_negative_pics + i]      = get_bits1(gb);
            }
        }
    }
    return 0;
}
