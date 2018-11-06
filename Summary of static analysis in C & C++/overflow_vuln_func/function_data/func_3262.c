static int mp_decode_layer3(MPADecodeContext *s)
{
    int nb_granules, main_data_begin, private_bits;
    int gr, ch, blocksplit_flag, i, j, k, n, bits_pos, bits_left;
    GranuleDef granules[2][2], *g;
    INT16 exponents[576];

    /
    if (s->lsf) {
        main_data_begin = get_bits(&s->gb, 8);
        if (s->nb_channels == 2)
            private_bits = get_bits(&s->gb, 2);
        else
            private_bits = get_bits(&s->gb, 1);
        nb_granules = 1;
    } else {
        main_data_begin = get_bits(&s->gb, 9);
        if (s->nb_channels == 2)
            private_bits = get_bits(&s->gb, 3);
        else
            private_bits = get_bits(&s->gb, 5);
        nb_granules = 2;
        for(ch=0;ch<s->nb_channels;ch++) {
            granules[ch][0].scfsi = 0; /
            granules[ch][1].scfsi = get_bits(&s->gb, 4);
        }
    }
    
    for(gr=0;gr<nb_granules;gr++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            dprintf("gr=%d ch=%d: side_info\n", gr, ch);
            g = &granules[ch][gr];
            g->part2_3_length = get_bits(&s->gb, 12);
            g->big_values = get_bits(&s->gb, 9);
            g->global_gain = get_bits(&s->gb, 8);
            /
            if ((s->mode_ext & (MODE_EXT_MS_STEREO | MODE_EXT_I_STEREO)) == 
                MODE_EXT_MS_STEREO)
                g->global_gain -= 2;
            if (s->lsf)
                g->scalefac_compress = get_bits(&s->gb, 9);
            else
                g->scalefac_compress = get_bits(&s->gb, 4);
            blocksplit_flag = get_bits(&s->gb, 1);
            if (blocksplit_flag) {
                g->block_type = get_bits(&s->gb, 2);
                if (g->block_type == 0)
                    return -1;
                g->switch_point = get_bits(&s->gb, 1);
                for(i=0;i<2;i++)
                    g->table_select[i] = get_bits(&s->gb, 5);
                for(i=0;i<3;i++) 
                    g->subblock_gain[i] = get_bits(&s->gb, 3);
                /
                if (g->block_type == 2)
                    g->region_size[0] = (36 / 2);
                else {
                    if (s->sample_rate_index <= 2) 
                        g->region_size[0] = (36 / 2);
                    else if (s->sample_rate_index != 8) 
                        g->region_size[0] = (54 / 2);
                    else
                        g->region_size[0] = (108 / 2);
                }
                g->region_size[1] = (576 / 2);
            } else {
                int region_address1, region_address2, l;
                g->block_type = 0;
                g->switch_point = 0;
                for(i=0;i<3;i++)
                    g->table_select[i] = get_bits(&s->gb, 5);
                /
                region_address1 = get_bits(&s->gb, 4);
                region_address2 = get_bits(&s->gb, 3);
                dprintf("region1=%d region2=%d\n", 
                        region_address1, region_address2);
                g->region_size[0] = 
                    band_index_long[s->sample_rate_index][region_address1 + 1] >> 1;
                l = region_address1 + region_address2 + 2;
                /
                if (l > 22)
                    l = 22;
                g->region_size[1] = 
                    band_index_long[s->sample_rate_index][l] >> 1;
            }
            /
            g->region_size[2] = (576 / 2);
            j = 0;
            for(i=0;i<3;i++) {
                k = g->region_size[i];
                if (k > g->big_values)
                    k = g->big_values;
                g->region_size[i] = k - j;
                j = k;
            }

            /
            if (g->block_type == 2) {
                if (g->switch_point) {
                    /
                    if (s->sample_rate_index <= 2)
                        g->long_end = 8;
                    else if (s->sample_rate_index != 8)
                        g->long_end = 6;
                    else
                        g->long_end = 4; /
                    
                    if (s->sample_rate_index != 8)
                        g->short_start = 3;
                    else
                        g->short_start = 2; 
                } else {
                    g->long_end = 0;
                    g->short_start = 0;
                }
            } else {
                g->short_start = 13;
                g->long_end = 22;
            }
            
            g->preflag = 0;
            if (!s->lsf)
                g->preflag = get_bits(&s->gb, 1);
            g->scalefac_scale = get_bits(&s->gb, 1);
            g->count1table_select = get_bits(&s->gb, 1);
            dprintf("block_type=%d switch_point=%d\n",
                    g->block_type, g->switch_point);
        }
    }

    /
    dprintf("seekback: %d\n", main_data_begin);
    seek_to_maindata(s, main_data_begin);

    for(gr=0;gr<nb_granules;gr++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            g = &granules[ch][gr];
            
            bits_pos = get_bits_count(&s->gb);
            
            if (!s->lsf) {
                UINT8 *sc;
                int slen, slen1, slen2;

                /
                slen1 = slen_table[0][g->scalefac_compress];
                slen2 = slen_table[1][g->scalefac_compress];
                dprintf("slen1=%d slen2=%d\n", slen1, slen2);
                if (g->block_type == 2) {
                    n = g->switch_point ? 17 : 18;
                    j = 0;
                    for(i=0;i<n;i++)
                        g->scale_factors[j++] = get_bitsz(&s->gb, slen1);
                    for(i=0;i<18;i++)
                        g->scale_factors[j++] = get_bitsz(&s->gb, slen2);
                    for(i=0;i<3;i++)
                        g->scale_factors[j++] = 0;
                } else {
                    sc = granules[ch][0].scale_factors;
                    j = 0;
                    for(k=0;k<4;k++) {
                        n = (k == 0 ? 6 : 5);
                        if ((g->scfsi & (0x8 >> k)) == 0) {
                            slen = (k < 2) ? slen1 : slen2;
                            for(i=0;i<n;i++)
                                g->scale_factors[j++] = get_bitsz(&s->gb, slen);
                        } else {
                            /
                            for(i=0;i<n;i++) {
                                g->scale_factors[j] = sc[j];
                                j++;
                            }
                        }
                    }
                    g->scale_factors[j++] = 0;
                }
#if defined(DEBUG)
                {
                    printf("scfsi=%x gr=%d ch=%d scale_factors:\n", 
                           g->scfsi, gr, ch);
                    for(i=0;i<j;i++)
                        printf(" %d", g->scale_factors[i]);
                    printf("\n");
                }
#endif
            } else {
                int tindex, tindex2, slen[4], sl, sf;

                /
                if (g->block_type == 2) {
                    tindex = g->switch_point ? 2 : 1;
                } else {
                    tindex = 0;
                }
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
                for(k=0;k<4;k++) {
                    n = lsf_nsf_table[tindex2][tindex][k];
                    sl = slen[k];
                    for(i=0;i<n;i++)
                        g->scale_factors[j++] = get_bitsz(&s->gb, sl);
                }
                /
                for(;j<40;j++)
                    g->scale_factors[j] = 0;
#if defined(DEBUG)
                {
                    printf("gr=%d ch=%d scale_factors:\n", 
                           gr, ch);
                    for(i=0;i<40;i++)
                        printf(" %d", g->scale_factors[i]);
                    printf("\n");
                }
#endif
            }

            exponents_from_scale_factors(s, g, exponents);

            /
            if (huffman_decode(s, g, exponents,
                               bits_pos + g->part2_3_length) < 0)
                return -1;
#if defined(DEBUG)
            sample_dump(0, g->sb_hybrid, 576);
#endif

            /
            bits_left = g->part2_3_length - (get_bits_count(&s->gb) - bits_pos);
            if (bits_left < 0) {
                dprintf("bits_left=%d\n", bits_left);
                return -1;
            }
            while (bits_left >= 16) {
                skip_bits(&s->gb, 16);
                bits_left -= 16;
            }
            if (bits_left > 0)
                skip_bits(&s->gb, bits_left);
        } /

        if (s->nb_channels == 2)
            compute_stereo(s, &granules[0][gr], &granules[1][gr]);

        for(ch=0;ch<s->nb_channels;ch++) {
            g = &granules[ch][gr];

            reorder_block(s, g);
#if defined(DEBUG)
            sample_dump(0, g->sb_hybrid, 576);
#endif
            compute_antialias(s, g);
#ifdef DEBUG
            sample_dump(1, g->sb_hybrid, 576);
#endif
            compute_imdct(s, g, &s->sb_samples[ch][18 * gr][0], s->mdct_buf[ch]); 
#ifdef DEBUG
            sample_dump(2, &s->sb_samples[ch][18 * gr][0], 576);
#endif
        }
    } /
    return nb_granules * 18;
}
