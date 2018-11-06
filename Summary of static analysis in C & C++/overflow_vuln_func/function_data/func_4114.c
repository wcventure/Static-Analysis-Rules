static int mp_decode_layer3(MPADecodeContext *s)
{
    int nb_granules, main_data_begin;
    int gr, ch, blocksplit_flag, i, j, k, n, bits_pos;
    GranuleDef *g;
    int16_t exponents[576]; //FIXME try INTFLOAT

    /
    if (s->lsf) {
        main_data_begin = get_bits(&s->gb, 8);
        skip_bits(&s->gb, s->nb_channels);
        nb_granules = 1;
    } else {
        main_data_begin = get_bits(&s->gb, 9);
        if (s->nb_channels == 2)
            skip_bits(&s->gb, 3);
        else
            skip_bits(&s->gb, 5);
        nb_granules = 2;
        for (ch = 0; ch < s->nb_channels; ch++) {
            s->granules[ch][0].scfsi = 0;/
            s->granules[ch][1].scfsi = get_bits(&s->gb, 4);
        }
    }

    for (gr = 0; gr < nb_granules; gr++) {
        for (ch = 0; ch < s->nb_channels; ch++) {
            av_dlog(s->avctx, "gr=%d ch=%d: side_info\n", gr, ch);
            g = &s->granules[ch][gr];
            g->part2_3_length = get_bits(&s->gb, 12);
            g->big_values     = get_bits(&s->gb,  9);
            if (g->big_values > 288) {
                av_log(s->avctx, AV_LOG_ERROR, "big_values too big\n");
                return AVERROR_INVALIDDATA;
            }

            g->global_gain = get_bits(&s->gb, 8);
            /
            if ((s->mode_ext & (MODE_EXT_MS_STEREO | MODE_EXT_I_STEREO)) ==
                MODE_EXT_MS_STEREO)
                g->global_gain -= 2;
            if (s->lsf)
                g->scalefac_compress = get_bits(&s->gb, 9);
            else
                g->scalefac_compress = get_bits(&s->gb, 4);
            blocksplit_flag = get_bits1(&s->gb);
            if (blocksplit_flag) {
                g->block_type = get_bits(&s->gb, 2);
                if (g->block_type == 0) {
                    av_log(s->avctx, AV_LOG_ERROR, "invalid block type\n");
                    return AVERROR_INVALIDDATA;
                }
                g->switch_point = get_bits1(&s->gb);
                for (i = 0; i < 2; i++)
                    g->table_select[i] = get_bits(&s->gb, 5);
                for (i = 0; i < 3; i++)
                    g->subblock_gain[i] = get_bits(&s->gb, 3);
                ff_init_short_region(s, g);
            } else {
                int region_address1, region_address2;
                g->block_type = 0;
                g->switch_point = 0;
                for (i = 0; i < 3; i++)
                    g->table_select[i] = get_bits(&s->gb, 5);
                /
                region_address1 = get_bits(&s->gb, 4);
                region_address2 = get_bits(&s->gb, 3);
                av_dlog(s->avctx, "region1=%d region2=%d\n",
                        region_address1, region_address2);
                ff_init_long_region(s, g, region_address1, region_address2);
            }
            ff_region_offset2size(g);
            ff_compute_band_indexes(s, g);

            g->preflag = 0;
            if (!s->lsf)
                g->preflag = get_bits1(&s->gb);
            g->scalefac_scale     = get_bits1(&s->gb);
            g->count1table_select = get_bits1(&s->gb);
            av_dlog(s->avctx, "block_type=%d switch_point=%d\n",
                    g->block_type, g->switch_point);
        }
    }

    if (!s->adu_mode) {
        int skip;
        const uint8_t *ptr = s->gb.buffer + (get_bits_count(&s->gb)>>3);
        assert((get_bits_count(&s->gb) & 7) == 0);
        /
        av_dlog(s->avctx, "seekback: %d\n", main_data_begin);
    //av_log(NULL, AV_LOG_ERROR, "backstep:%d, lastbuf:%d\n", main_data_begin, s->last_buf_size);

        memcpy(s->last_buf + s->last_buf_size, ptr, EXTRABYTES);
        s->in_gb = s->gb;
        init_get_bits(&s->gb, s->last_buf, s->last_buf_size*8);
#if !UNCHECKED_BITSTREAM_READER
        s->gb.size_in_bits_plus8 += EXTRABYTES * 8;
#endif
        s->last_buf_size <<= 3;
        for (gr = 0; gr < nb_granules && (s->last_buf_size >> 3) < main_data_begin; gr++) {
            for (ch = 0; ch < s->nb_channels; ch++) {
                g = &s->granules[ch][gr];
                s->last_buf_size += g->part2_3_length;
                memset(g->sb_hybrid, 0, sizeof(g->sb_hybrid));
            }
        }
        skip = s->last_buf_size - 8 * main_data_begin;
        if (skip >= s->gb.size_in_bits && s->in_gb.buffer) {
            skip_bits_long(&s->in_gb, skip - s->gb.size_in_bits);
            s->gb           = s->in_gb;
            s->in_gb.buffer = NULL;
        } else {
            skip_bits_long(&s->gb, skip);
        }
    } else {
        gr = 0;
    }

    for (; gr < nb_granules; gr++) {
        for (ch = 0; ch < s->nb_channels; ch++) {
            g = &s->granules[ch][gr];
            bits_pos = get_bits_count(&s->gb);

            if (!s->lsf) {
                uint8_t *sc;
                int slen, slen1, slen2;

                /
                slen1 = slen_table[0][g->scalefac_compress];
                slen2 = slen_table[1][g->scalefac_compress];
                av_dlog(s->avctx, "slen1=%d slen2=%d\n", slen1, slen2);
                if (g->block_type == 2) {
                    n = g->switch_point ? 17 : 18;
                    j = 0;
                    if (slen1) {
                        for (i = 0; i < n; i++)
                            g->scale_factors[j++] = get_bits(&s->gb, slen1);
                    } else {
                        for (i = 0; i < n; i++)
                            g->scale_factors[j++] = 0;
                    }
                    if (slen2) {
                        for (i = 0; i < 18; i++)
                            g->scale_factors[j++] = get_bits(&s->gb, slen2);
                        for (i = 0; i < 3; i++)
                            g->scale_factors[j++] = 0;
                    } else {
                        for (i = 0; i < 21; i++)
                            g->scale_factors[j++] = 0;
                    }
                } else {
                    sc = s->granules[ch][0].scale_factors;
                    j = 0;
                    for (k = 0; k < 4; k++) {
                        n = k == 0 ? 6 : 5;
                        if ((g->scfsi & (0x8 >> k)) == 0) {
                            slen = (k < 2) ? slen1 : slen2;
                            if (slen) {
                                for (i = 0; i < n; i++)
                                    g->scale_factors[j++] = get_bits(&s->gb, slen);
                            } else {
                                for (i = 0; i < n; i++)
                                    g->scale_factors[j++] = 0;
                            }
                        } else {
                            /
                            for (i = 0; i < n; i++) {
                                g->scale_factors[j] = sc[j];
                                j++;
                            }
                        }
                    }
                    g->scale_factors[j++] = 0;
                }
            } else {
                int tindex, tindex2, slen[4], sl, sf;

                /
                if (g->block_type == 2)
                    tindex = g->switch_point ? 2 : 1;
                else
                    tindex = 0;

                sf = g->scalefac_compress;
                if ((s->mode_ext & MODE_EXT_I_STEREO) && ch == 1) {
                    /
                    sf >>= 1;
                    if (sf < 180) {
                        lsf_sf_expand(slen, sf, 6, 6, 0);
                        tindex2 = 3;
                    } else if (sf < 244) {
                        lsf_sf_expand(slen, sf - 180, 4, 4, 0);
                        tindex2 = 4;
                    } else {
                        lsf_sf_expand(slen, sf - 244, 3, 0, 0);
                        tindex2 = 5;
                    }
                } else {
                    /
                    if (sf < 400) {
                        lsf_sf_expand(slen, sf, 5, 4, 4);
                        tindex2 = 0;
                    } else if (sf < 500) {
                        lsf_sf_expand(slen, sf - 400, 5, 4, 0);
                        tindex2 = 1;
                    } else {
                        lsf_sf_expand(slen, sf - 500, 3, 0, 0);
                        tindex2 = 2;
                        g->preflag = 1;
                    }
                }

                j = 0;
                for (k = 0; k < 4; k++) {
                    n  = lsf_nsf_table[tindex2][tindex][k];
                    sl = slen[k];
                    if (sl) {
                        for (i = 0; i < n; i++)
                            g->scale_factors[j++] = get_bits(&s->gb, sl);
                    } else {
                        for (i = 0; i < n; i++)
                            g->scale_factors[j++] = 0;
                    }
                }
                /
                for (; j < 40; j++)
                    g->scale_factors[j] = 0;
            }

            exponents_from_scale_factors(s, g, exponents);

            /
            huffman_decode(s, g, exponents, bits_pos + g->part2_3_length);
        } /

        if (s->nb_channels == 2)
            compute_stereo(s, &s->granules[0][gr], &s->granules[1][gr]);

        for (ch = 0; ch < s->nb_channels; ch++) {
            g = &s->granules[ch][gr];

            reorder_block(s, g);
            compute_antialias(s, g);
            compute_imdct(s, g, &s->sb_samples[ch][18 * gr][0], s->mdct_buf[ch]);
        }
    } /
    if (get_bits_count(&s->gb) < 0)
        skip_bits_long(&s->gb, -get_bits_count(&s->gb));
    return nb_granules * 18;
}
