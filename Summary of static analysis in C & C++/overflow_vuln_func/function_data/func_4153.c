static void compute_stereo(MPADecodeContext *s, GranuleDef *g0, GranuleDef *g1)
{
    int i, j, k, l;
    int sf_max, sf, len, non_zero_found;
    INTFLOAT (*is_tab)[16], *tab0, *tab1, tmp0, tmp1, v1, v2;
    int non_zero_found_short[3];

    /
    if (s->mode_ext & MODE_EXT_I_STEREO) {
        if (!s->lsf) {
            is_tab = is_table;
            sf_max = 7;
        } else {
            is_tab = is_table_lsf[g1->scalefac_compress & 1];
            sf_max = 16;
        }

        tab0 = g0->sb_hybrid + 576;
        tab1 = g1->sb_hybrid + 576;

        non_zero_found_short[0] = 0;
        non_zero_found_short[1] = 0;
        non_zero_found_short[2] = 0;
        k = (13 - g1->short_start) * 3 + g1->long_end - 3;
        for (i = 12; i >= g1->short_start; i--) {
            /
            if (i != 11)
                k -= 3;
            len = band_size_short[s->sample_rate_index][i];
            for (l = 2; l >= 0; l--) {
                tab0 -= len;
                tab1 -= len;
                if (!non_zero_found_short[l]) {
                    /
                    for (j = 0; j < len; j++) {
                        if (tab1[j] != 0) {
                            non_zero_found_short[l] = 1;
                            goto found1;
                        }
                    }
                    sf = g1->scale_factors[k + l];
                    if (sf >= sf_max)
                        goto found1;

                    v1 = is_tab[0][sf];
                    v2 = is_tab[1][sf];
                    for (j = 0; j < len; j++) {
                        tmp0    = tab0[j];
                        tab0[j] = MULLx(tmp0, v1, FRAC_BITS);
                        tab1[j] = MULLx(tmp0, v2, FRAC_BITS);
                    }
                } else {
found1:
                    if (s->mode_ext & MODE_EXT_MS_STEREO) {
                        /
                        for (j = 0; j < len; j++) {
                            tmp0    = tab0[j];
                            tmp1    = tab1[j];
                            tab0[j] = MULLx(tmp0 + tmp1, ISQRT2, FRAC_BITS);
                            tab1[j] = MULLx(tmp0 - tmp1, ISQRT2, FRAC_BITS);
                        }
                    }
                }
            }
        }

        non_zero_found = non_zero_found_short[0] |
                         non_zero_found_short[1] |
                         non_zero_found_short[2];

        for (i = g1->long_end - 1;i >= 0;i--) {
            len   = band_size_long[s->sample_rate_index][i];
            tab0 -= len;
            tab1 -= len;
            /
            if (!non_zero_found) {
                for (j = 0; j < len; j++) {
                    if (tab1[j] != 0) {
                        non_zero_found = 1;
                        goto found2;
                    }
                }
                /
                k  = (i == 21) ? 20 : i;
                sf = g1->scale_factors[k];
                if (sf >= sf_max)
                    goto found2;
                v1 = is_tab[0][sf];
                v2 = is_tab[1][sf];
                for (j = 0; j < len; j++) {
                    tmp0    = tab0[j];
                    tab0[j] = MULLx(tmp0, v1, FRAC_BITS);
                    tab1[j] = MULLx(tmp0, v2, FRAC_BITS);
                }
            } else {
found2:
                if (s->mode_ext & MODE_EXT_MS_STEREO) {
                    /
                    for (j = 0; j < len; j++) {
                        tmp0    = tab0[j];
                        tmp1    = tab1[j];
                        tab0[j] = MULLx(tmp0 + tmp1, ISQRT2, FRAC_BITS);
                        tab1[j] = MULLx(tmp0 - tmp1, ISQRT2, FRAC_BITS);
                    }
                }
            }
        }
    } else if (s->mode_ext & MODE_EXT_MS_STEREO) {
        /
        /
#if USE_FLOATS
       s->fdsp->butterflies_float(g0->sb_hybrid, g1->sb_hybrid, 576);
#else
        tab0 = g0->sb_hybrid;
        tab1 = g1->sb_hybrid;
        for (i = 0; i < 576; i++) {
            tmp0    = tab0[i];
            tmp1    = tab1[i];
            tab0[i] = tmp0 + tmp1;
            tab1[i] = tmp0 - tmp1;
        }
#endif
    }
}
