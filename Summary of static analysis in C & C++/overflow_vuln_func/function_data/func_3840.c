static int decode_audio_block(AC3DecodeContext *s, int blk)
{
    int fbw_channels = s->fbw_channels;
    int channel_mode = s->channel_mode;
    int i, bnd, seg, ch;
    int different_transforms;
    int downmix_output;
    int cpl_in_use;
    GetBitContext *gbc = &s->gbc;
    uint8_t bit_alloc_stages[AC3_MAX_CHANNELS] = { 0 };

    /
    different_transforms = 0;
    if (s->block_switch_syntax) {
        for (ch = 1; ch <= fbw_channels; ch++) {
            s->block_switch[ch] = get_bits1(gbc);
            if (ch > 1 && s->block_switch[ch] != s->block_switch[1])
                different_transforms = 1;
        }
    }

    /
    if (s->dither_flag_syntax) {
        for (ch = 1; ch <= fbw_channels; ch++) {
            s->dither_flag[ch] = get_bits1(gbc);
        }
    }

    /
    i = !s->channel_mode;
    do {
        if (get_bits1(gbc)) {
            /
            int range_bits = get_bits(gbc, 8);
            INTFLOAT range = AC3_RANGE(range_bits);
            if (range_bits <= 127 || s->drc_scale <= 1.0)
                s->dynamic_range[i] = AC3_DYNAMIC_RANGE(range);
            else
                s->dynamic_range[i] = range;
        } else if (blk == 0) {
            s->dynamic_range[i] = AC3_DYNAMIC_RANGE1;
        }
    } while (i--);

    /
    if (s->eac3 && (!blk || get_bits1(gbc))) {
        s->spx_in_use = get_bits1(gbc);
        if (s->spx_in_use) {
            int dst_start_freq, dst_end_freq, src_start_freq,
                start_subband, end_subband;

            /
            if (s->channel_mode == AC3_CHMODE_MONO) {
                s->channel_uses_spx[1] = 1;
            } else {
                for (ch = 1; ch <= fbw_channels; ch++)
                    s->channel_uses_spx[ch] = get_bits1(gbc);
            }

            /
            dst_start_freq = get_bits(gbc, 2);
            start_subband  = get_bits(gbc, 3) + 2;
            if (start_subband > 7)
                start_subband += start_subband - 7;
            end_subband    = get_bits(gbc, 3) + 5;
#if USE_FIXED
            s->spx_dst_end_freq = end_freq_inv_tab[end_subband];
#endif
            if (end_subband   > 7)
                end_subband   += end_subband   - 7;
            dst_start_freq = dst_start_freq * 12 + 25;
            src_start_freq = start_subband  * 12 + 25;
            dst_end_freq   = end_subband    * 12 + 25;

            /
            if (start_subband >= end_subband) {
                av_log(s->avctx, AV_LOG_ERROR, "invalid spectral extension "
                       "range (%d >= %d)\n", start_subband, end_subband);
                return AVERROR_INVALIDDATA;
            }
            if (dst_start_freq >= src_start_freq) {
                av_log(s->avctx, AV_LOG_ERROR, "invalid spectral extension "
                       "copy start bin (%d >= %d)\n", dst_start_freq, src_start_freq);
                return AVERROR_INVALIDDATA;
            }

            s->spx_dst_start_freq = dst_start_freq;
            s->spx_src_start_freq = src_start_freq;
            if (!USE_FIXED)
                s->spx_dst_end_freq   = dst_end_freq;

            decode_band_structure(gbc, blk, s->eac3, 0,
                                  start_subband, end_subband,
                                  ff_eac3_default_spx_band_struct,
                                  &s->num_spx_bands,
                                  s->spx_band_sizes);
        } else {
            for (ch = 1; ch <= fbw_channels; ch++) {
                s->channel_uses_spx[ch] = 0;
                s->first_spx_coords[ch] = 1;
            }
        }
    }

    /
    if (s->spx_in_use) {
        for (ch = 1; ch <= fbw_channels; ch++) {
            if (s->channel_uses_spx[ch]) {
                if (s->first_spx_coords[ch] || get_bits1(gbc)) {
                    INTFLOAT spx_blend;
                    int bin, master_spx_coord;

                    s->first_spx_coords[ch] = 0;
                    spx_blend = AC3_SPX_BLEND(get_bits(gbc, 5));
                    master_spx_coord = get_bits(gbc, 2) * 3;

                    bin = s->spx_src_start_freq;
                    for (bnd = 0; bnd < s->num_spx_bands; bnd++) {
                        int bandsize;
                        int spx_coord_exp, spx_coord_mant;
                        INTFLOAT nratio, sblend, nblend;
#if USE_FIXED
                        int64_t accu;
                        /
                        bandsize = s->spx_band_sizes[bnd];
                        accu = (int64_t)((bin << 23) + (bandsize << 22)) * s->spx_dst_end_freq;
                        nratio = (int)(accu >> 32);
                        nratio -= spx_blend << 18;

                        if (nratio < 0) {
                            nblend = 0;
                            sblend = 0x800000;
                        } else if (nratio > 0x7fffff) {
                            nblend = 0x800000;
                            sblend = 0;
                        } else {
                            nblend = fixed_sqrt(nratio, 23);
                            accu = (int64_t)nblend * 1859775393;
                            nblend = (int)((accu + (1<<29)) >> 30);
                            sblend = fixed_sqrt(0x800000 - nratio, 23);
                        }
#else
                        float spx_coord;

                        /
                        bandsize = s->spx_band_sizes[bnd];
                        nratio = ((float)((bin + (bandsize >> 1))) / s->spx_dst_end_freq) - spx_blend;
                        nratio = av_clipf(nratio, 0.0f, 1.0f);
                        nblend = sqrtf(3.0f * nratio); // noise is scaled by sqrt(3)
                                                       // to give unity variance
                        sblend = sqrtf(1.0f - nratio);
#endif
                        bin += bandsize;

                        /
                        spx_coord_exp  = get_bits(gbc, 4);
                        spx_coord_mant = get_bits(gbc, 2);
                        if (spx_coord_exp == 15) spx_coord_mant <<= 1;
                        else                     spx_coord_mant += 4;
                        spx_coord_mant <<= (25 - spx_coord_exp - master_spx_coord);

                        /
#if USE_FIXED
                        accu = (int64_t)nblend * spx_coord_mant;
                        s->spx_noise_blend[ch][bnd]  = (int)((accu + (1<<22)) >> 23);
                        accu = (int64_t)sblend * spx_coord_mant;
                        s->spx_signal_blend[ch][bnd] = (int)((accu + (1<<22)) >> 23);
#else
                        spx_coord = spx_coord_mant * (1.0f / (1 << 23));
                        s->spx_noise_blend [ch][bnd] = nblend * spx_coord;
                        s->spx_signal_blend[ch][bnd] = sblend * spx_coord;
#endif
                    }
                }
            } else {
                s->first_spx_coords[ch] = 1;
            }
        }
    }

    /
    if (s->eac3 ? s->cpl_strategy_exists[blk] : get_bits1(gbc)) {
        memset(bit_alloc_stages, 3, AC3_MAX_CHANNELS);
        if (!s->eac3)
            s->cpl_in_use[blk] = get_bits1(gbc);
        if (s->cpl_in_use[blk]) {
            /
            int cpl_start_subband, cpl_end_subband;

            if (channel_mode < AC3_CHMODE_STEREO) {
                av_log(s->avctx, AV_LOG_ERROR, "coupling not allowed in mono or dual-mono\n");
                return AVERROR_INVALIDDATA;
            }

            /
            if (s->eac3 && get_bits1(gbc)) {
                /
                avpriv_request_sample(s->avctx, "Enhanced coupling");
                return AVERROR_PATCHWELCOME;
            }

            /
            if (s->eac3 && s->channel_mode == AC3_CHMODE_STEREO) {
                s->channel_in_cpl[1] = 1;
                s->channel_in_cpl[2] = 1;
            } else {
                for (ch = 1; ch <= fbw_channels; ch++)
                    s->channel_in_cpl[ch] = get_bits1(gbc);
            }

            /
            if (channel_mode == AC3_CHMODE_STEREO)
                s->phase_flags_in_use = get_bits1(gbc);

            /
            cpl_start_subband = get_bits(gbc, 4);
            cpl_end_subband = s->spx_in_use ? (s->spx_src_start_freq - 37) / 12 :
                                              get_bits(gbc, 4) + 3;
            if (cpl_start_subband >= cpl_end_subband) {
                av_log(s->avctx, AV_LOG_ERROR, "invalid coupling range (%d >= %d)\n",
                       cpl_start_subband, cpl_end_subband);
                return AVERROR_INVALIDDATA;
            }
            s->start_freq[CPL_CH] = cpl_start_subband * 12 + 37;
            s->end_freq[CPL_CH]   = cpl_end_subband   * 12 + 37;

            decode_band_structure(gbc, blk, s->eac3, 0, cpl_start_subband,
                                  cpl_end_subband,
                                  ff_eac3_default_cpl_band_struct,
                                  &s->num_cpl_bands, s->cpl_band_sizes);
        } else {
            /
            for (ch = 1; ch <= fbw_channels; ch++) {
                s->channel_in_cpl[ch] = 0;
                s->first_cpl_coords[ch] = 1;
            }
            s->first_cpl_leak = s->eac3;
            s->phase_flags_in_use = 0;
        }
    } else if (!s->eac3) {
        if (!blk) {
            av_log(s->avctx, AV_LOG_ERROR, "new coupling strategy must "
                   "be present in block 0\n");
            return AVERROR_INVALIDDATA;
        } else {
            s->cpl_in_use[blk] = s->cpl_in_use[blk-1];
        }
    }
    cpl_in_use = s->cpl_in_use[blk];

    /
    if (cpl_in_use) {
        int cpl_coords_exist = 0;

        for (ch = 1; ch <= fbw_channels; ch++) {
            if (s->channel_in_cpl[ch]) {
                if ((s->eac3 && s->first_cpl_coords[ch]) || get_bits1(gbc)) {
                    int master_cpl_coord, cpl_coord_exp, cpl_coord_mant;
                    s->first_cpl_coords[ch] = 0;
                    cpl_coords_exist = 1;
                    master_cpl_coord = 3 * get_bits(gbc, 2);
                    for (bnd = 0; bnd < s->num_cpl_bands; bnd++) {
                        cpl_coord_exp = get_bits(gbc, 4);
                        cpl_coord_mant = get_bits(gbc, 4);
                        if (cpl_coord_exp == 15)
                            s->cpl_coords[ch][bnd] = cpl_coord_mant << 22;
                        else
                            s->cpl_coords[ch][bnd] = (cpl_coord_mant + 16) << 21;
                        s->cpl_coords[ch][bnd] >>= (cpl_coord_exp + master_cpl_coord);
                    }
                } else if (!blk) {
                    av_log(s->avctx, AV_LOG_ERROR, "new coupling coordinates must "
                           "be present in block 0\n");
                    return AVERROR_INVALIDDATA;
                }
            } else {
                /
                s->first_cpl_coords[ch] = 1;
            }
        }
        /
        if (channel_mode == AC3_CHMODE_STEREO && cpl_coords_exist) {
            for (bnd = 0; bnd < s->num_cpl_bands; bnd++) {
                s->phase_flags[bnd] = s->phase_flags_in_use? get_bits1(gbc) : 0;
            }
        }
    }

    /
    if (channel_mode == AC3_CHMODE_STEREO) {
        if ((s->eac3 && !blk) || get_bits1(gbc)) {
            s->num_rematrixing_bands = 4;
            if (cpl_in_use && s->start_freq[CPL_CH] <= 61) {
                s->num_rematrixing_bands -= 1 + (s->start_freq[CPL_CH] == 37);
            } else if (s->spx_in_use && s->spx_src_start_freq <= 61) {
                s->num_rematrixing_bands--;
            }
            for (bnd = 0; bnd < s->num_rematrixing_bands; bnd++)
                s->rematrixing_flags[bnd] = get_bits1(gbc);
        } else if (!blk) {
            av_log(s->avctx, AV_LOG_WARNING, "Warning: "
                   "new rematrixing strategy not present in block 0\n");
            s->num_rematrixing_bands = 0;
        }
    }

    /
    for (ch = !cpl_in_use; ch <= s->channels; ch++) {
        if (!s->eac3)
            s->exp_strategy[blk][ch] = get_bits(gbc, 2 - (ch == s->lfe_ch));
        if (s->exp_strategy[blk][ch] != EXP_REUSE)
            bit_alloc_stages[ch] = 3;
    }

    /
    for (ch = 1; ch <= fbw_channels; ch++) {
        s->start_freq[ch] = 0;
        if (s->exp_strategy[blk][ch] != EXP_REUSE) {
            int group_size;
            int prev = s->end_freq[ch];
            if (s->channel_in_cpl[ch])
                s->end_freq[ch] = s->start_freq[CPL_CH];
            else if (s->channel_uses_spx[ch])
                s->end_freq[ch] = s->spx_src_start_freq;
            else {
                int bandwidth_code = get_bits(gbc, 6);
                if (bandwidth_code > 60) {
                    av_log(s->avctx, AV_LOG_ERROR, "bandwidth code = %d > 60\n", bandwidth_code);
                    return AVERROR_INVALIDDATA;
                }
                s->end_freq[ch] = bandwidth_code * 3 + 73;
            }
            group_size = 3 << (s->exp_strategy[blk][ch] - 1);
            s->num_exp_groups[ch] = (s->end_freq[ch] + group_size-4) / group_size;
            if (blk > 0 && s->end_freq[ch] != prev)
                memset(bit_alloc_stages, 3, AC3_MAX_CHANNELS);
        }
    }
    if (cpl_in_use && s->exp_strategy[blk][CPL_CH] != EXP_REUSE) {
        s->num_exp_groups[CPL_CH] = (s->end_freq[CPL_CH] - s->start_freq[CPL_CH]) /
                                    (3 << (s->exp_strategy[blk][CPL_CH] - 1));
    }

    /
    for (ch = !cpl_in_use; ch <= s->channels; ch++) {
        if (s->exp_strategy[blk][ch] != EXP_REUSE) {
            s->dexps[ch][0] = get_bits(gbc, 4) << !ch;
            if (decode_exponents(gbc, s->exp_strategy[blk][ch],
                                 s->num_exp_groups[ch], s->dexps[ch][0],
                                 &s->dexps[ch][s->start_freq[ch]+!!ch])) {
                av_log(s->avctx, AV_LOG_ERROR, "exponent out-of-range\n");
                return AVERROR_INVALIDDATA;
            }
            if (ch != CPL_CH && ch != s->lfe_ch)
                skip_bits(gbc, 2); /
        }
    }

    /
    if (s->bit_allocation_syntax) {
        if (get_bits1(gbc)) {
            s->bit_alloc_params.slow_decay = ff_ac3_slow_decay_tab[get_bits(gbc, 2)] >> s->bit_alloc_params.sr_shift;
            s->bit_alloc_params.fast_decay = ff_ac3_fast_decay_tab[get_bits(gbc, 2)] >> s->bit_alloc_params.sr_shift;
            s->bit_alloc_params.slow_gain  = ff_ac3_slow_gain_tab[get_bits(gbc, 2)];
            s->bit_alloc_params.db_per_bit = ff_ac3_db_per_bit_tab[get_bits(gbc, 2)];
            s->bit_alloc_params.floor  = ff_ac3_floor_tab[get_bits(gbc, 3)];
            for (ch = !cpl_in_use; ch <= s->channels; ch++)
                bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 2);
        } else if (!blk) {
            av_log(s->avctx, AV_LOG_ERROR, "new bit allocation info must "
                   "be present in block 0\n");
            return AVERROR_INVALIDDATA;
        }
    }

    /
    if (!s->eac3 || !blk) {
        if (s->snr_offset_strategy && get_bits1(gbc)) {
            int snr = 0;
            int csnr;
            csnr = (get_bits(gbc, 6) - 15) << 4;
            for (i = ch = !cpl_in_use; ch <= s->channels; ch++) {
                /
                if (ch == i || s->snr_offset_strategy == 2)
                    snr = (csnr + get_bits(gbc, 4)) << 2;
                /
                if (blk && s->snr_offset[ch] != snr) {
                    bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 1);
                }
                s->snr_offset[ch] = snr;

                /
                if (!s->eac3) {
                    int prev = s->fast_gain[ch];
                    s->fast_gain[ch] = ff_ac3_fast_gain_tab[get_bits(gbc, 3)];
                    /
                    if (blk && prev != s->fast_gain[ch])
                        bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 2);
                }
            }
        } else if (!s->eac3 && !blk) {
            av_log(s->avctx, AV_LOG_ERROR, "new snr offsets must be present in block 0\n");
            return AVERROR_INVALIDDATA;
        }
    }

    /
    if (s->fast_gain_syntax && get_bits1(gbc)) {
        for (ch = !cpl_in_use; ch <= s->channels; ch++) {
            int prev = s->fast_gain[ch];
            s->fast_gain[ch] = ff_ac3_fast_gain_tab[get_bits(gbc, 3)];
            /
            if (blk && prev != s->fast_gain[ch])
                bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 2);
        }
    } else if (s->eac3 && !blk) {
        for (ch = !cpl_in_use; ch <= s->channels; ch++)
            s->fast_gain[ch] = ff_ac3_fast_gain_tab[4];
    }

    /
    if (s->frame_type == EAC3_FRAME_TYPE_INDEPENDENT && get_bits1(gbc)) {
        skip_bits(gbc, 10); // skip converter snr offset
    }

    /
    if (cpl_in_use) {
        if (s->first_cpl_leak || get_bits1(gbc)) {
            int fl = get_bits(gbc, 3);
            int sl = get_bits(gbc, 3);
            /
            if (blk && (fl != s->bit_alloc_params.cpl_fast_leak ||
                sl != s->bit_alloc_params.cpl_slow_leak)) {
                bit_alloc_stages[CPL_CH] = FFMAX(bit_alloc_stages[CPL_CH], 2);
            }
            s->bit_alloc_params.cpl_fast_leak = fl;
            s->bit_alloc_params.cpl_slow_leak = sl;
        } else if (!s->eac3 && !blk) {
            av_log(s->avctx, AV_LOG_ERROR, "new coupling leak info must "
                   "be present in block 0\n");
            return AVERROR_INVALIDDATA;
        }
        s->first_cpl_leak = 0;
    }

    /
    if (s->dba_syntax && get_bits1(gbc)) {
        /
        for (ch = !cpl_in_use; ch <= fbw_channels; ch++) {
            s->dba_mode[ch] = get_bits(gbc, 2);
            if (s->dba_mode[ch] == DBA_RESERVED) {
                av_log(s->avctx, AV_LOG_ERROR, "delta bit allocation strategy reserved\n");
                return AVERROR_INVALIDDATA;
            }
            bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 2);
        }
        /
        for (ch = !cpl_in_use; ch <= fbw_channels; ch++) {
            if (s->dba_mode[ch] == DBA_NEW) {
                s->dba_nsegs[ch] = get_bits(gbc, 3) + 1;
                for (seg = 0; seg < s->dba_nsegs[ch]; seg++) {
                    s->dba_offsets[ch][seg] = get_bits(gbc, 5);
                    s->dba_lengths[ch][seg] = get_bits(gbc, 4);
                    s->dba_values[ch][seg]  = get_bits(gbc, 3);
                }
                /
                bit_alloc_stages[ch] = FFMAX(bit_alloc_stages[ch], 2);
            }
        }
    } else if (blk == 0) {
        for (ch = 0; ch <= s->channels; ch++) {
            s->dba_mode[ch] = DBA_NONE;
        }
    }

    /
    for (ch = !cpl_in_use; ch <= s->channels; ch++) {
        if (bit_alloc_stages[ch] > 2) {
            /
            ff_ac3_bit_alloc_calc_psd(s->dexps[ch],
                                      s->start_freq[ch], s->end_freq[ch],
                                      s->psd[ch], s->band_psd[ch]);
        }
        if (bit_alloc_stages[ch] > 1) {
            /
            if (ff_ac3_bit_alloc_calc_mask(&s->bit_alloc_params, s->band_psd[ch],
                                           s->start_freq[ch],  s->end_freq[ch],
                                           s->fast_gain[ch],   (ch == s->lfe_ch),
                                           s->dba_mode[ch],    s->dba_nsegs[ch],
                                           s->dba_offsets[ch], s->dba_lengths[ch],
                                           s->dba_values[ch],  s->mask[ch])) {
                av_log(s->avctx, AV_LOG_ERROR, "error in bit allocation\n");
                return AVERROR_INVALIDDATA;
            }
        }
        if (bit_alloc_stages[ch] > 0) {
            /
            const uint8_t *bap_tab = s->channel_uses_aht[ch] ?
                                     ff_eac3_hebap_tab : ff_ac3_bap_tab;
            s->ac3dsp.bit_alloc_calc_bap(s->mask[ch], s->psd[ch],
                                      s->start_freq[ch], s->end_freq[ch],
                                      s->snr_offset[ch],
                                      s->bit_alloc_params.floor,
                                      bap_tab, s->bap[ch]);
        }
    }

    /
    if (s->skip_syntax && get_bits1(gbc)) {
        int skipl = get_bits(gbc, 9);
        while (skipl--)
            skip_bits(gbc, 8);
    }

    /
    decode_transform_coeffs(s, blk);

    /

    /
    if (s->channel_mode == AC3_CHMODE_STEREO)
        do_rematrixing(s);

    /
    for (ch = 1; ch <= s->channels; ch++) {
        int audio_channel = 0;
        INTFLOAT gain;
        if (s->channel_mode == AC3_CHMODE_DUALMONO)
            audio_channel = 2-ch;
        if (s->heavy_compression && s->compression_exists[audio_channel])
            gain = s->heavy_dynamic_range[audio_channel];
        else
            gain = s->dynamic_range[audio_channel];

#if USE_FIXED
        scale_coefs(s->transform_coeffs[ch], s->fixed_coeffs[ch], gain, 256);
#else
        if (s->target_level != 0)
          gain = gain * s->level_gain[audio_channel];
        gain *= 1.0 / 4194304.0f;
        s->fmt_conv.int32_to_float_fmul_scalar(s->transform_coeffs[ch],
                                               s->fixed_coeffs[ch], gain, 256);
#endif
    }

    /
    if (s->spx_in_use && CONFIG_EAC3_DECODER) {
        ff_eac3_apply_spectral_extension(s);
    }

    /
    downmix_output = s->channels != s->out_channels &&
                     !((s->output_mode & AC3_OUTPUT_LFEON) &&
                     s->fbw_channels == s->out_channels);
    if (different_transforms) {
        /
        if (s->downmixed) {
            s->downmixed = 0;
            ac3_upmix_delay(s);
        }

        do_imdct(s, s->channels);

        if (downmix_output) {
#if USE_FIXED
            ac3_downmix_c_fixed16(s->outptr, s->downmix_coeffs,
                              s->out_channels, s->fbw_channels, 256);
#else
            s->ac3dsp.downmix(s->outptr, s->downmix_coeffs,
                              s->out_channels, s->fbw_channels, 256);
#endif
        }
    } else {
        if (downmix_output) {
            s->ac3dsp.AC3_RENAME(downmix)(s->xcfptr + 1, s->downmix_coeffs,
                                          s->out_channels, s->fbw_channels, 256);
        }

        if (downmix_output && !s->downmixed) {
            s->downmixed = 1;
            s->ac3dsp.AC3_RENAME(downmix)(s->dlyptr, s->downmix_coeffs,
                                          s->out_channels, s->fbw_channels, 128);
        }

        do_imdct(s, s->out_channels);
    }

    return 0;
}
