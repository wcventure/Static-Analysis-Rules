static int parse_tonal(DCALbrDecoder *s, int group)
{
    unsigned int amp[DCA_LBR_CHANNELS_TOTAL];
    unsigned int phs[DCA_LBR_CHANNELS_TOTAL];
    unsigned int diff, main_amp, shift;
    int sf, sf_idx, ch, main_ch, freq;
    int ch_nbits = av_ceil_log2(s->nchannels_total);

    // Parse subframes for this group
    for (sf = 0; sf < 1 << group; sf += diff ? 8 : 1) {
        sf_idx = ((s->framenum << group) + sf) & 31;
        s->tonal_bounds[group][sf_idx][0] = s->ntones;

        // Parse tones for this subframe
        for (freq = 1;; freq++) {
            if (get_bits_left(&s->gb) < 1) {
                av_log(s->avctx, AV_LOG_ERROR, "Tonal group chunk too short\n");
                return -1;
            }

            diff = parse_vlc(&s->gb, &ff_dca_vlc_tnl_grp[group], 2);
            if (diff >= FF_ARRAY_ELEMS(ff_dca_fst_amp)) {
                av_log(s->avctx, AV_LOG_ERROR, "Invalid tonal frequency diff\n");
                return -1;
            }

            diff = get_bitsz(&s->gb, diff >> 2) + ff_dca_fst_amp[diff];
            if (diff <= 1)
                break;  // End of subframe

            freq += diff - 2;
            if (freq >> (5 - group) > s->nsubbands * 4 - 5) {
                av_log(s->avctx, AV_LOG_ERROR, "Invalid spectral line offset\n");
                return -1;
            }

            // Main channel
            main_ch = get_bitsz(&s->gb, ch_nbits);
            main_amp = parse_vlc(&s->gb, &ff_dca_vlc_tnl_scf, 2)
                + s->tonal_scf[ff_dca_freq_to_sb[freq >> (7 - group)]]
                + s->limited_range - 2;
            amp[main_ch] = main_amp < AMP_MAX ? main_amp : 0;
            phs[main_ch] = get_bits(&s->gb, 3);

            // Secondary channels
            for (ch = 0; ch < s->nchannels_total; ch++) {
                if (ch == main_ch)
                    continue;
                if (get_bits1(&s->gb)) {
                    amp[ch] = amp[main_ch] - parse_vlc(&s->gb, &ff_dca_vlc_damp, 1);
                    phs[ch] = phs[main_ch] - parse_vlc(&s->gb, &ff_dca_vlc_dph,  1);
                } else {
                    amp[ch] = 0;
                    phs[ch] = 0;
                }
            }

            if (amp[main_ch]) {
                // Allocate new tone
                DCALbrTone *t = &s->tones[s->ntones];
                s->ntones = (s->ntones + 1) & (DCA_LBR_TONES - 1);

                t->x_freq = freq >> (5 - group);
                t->f_delt = (freq & ((1 << (5 - group)) - 1)) << group;
                t->ph_rot = 256 - (t->x_freq & 1) * 128 - t->f_delt * 4;

                shift = ff_dca_ph0_shift[(t->x_freq & 3) * 2 + (freq & 1)]
                    - ((t->ph_rot << (5 - group)) - t->ph_rot);

                for (ch = 0; ch < s->nchannels; ch++) {
                    t->amp[ch] = amp[ch] < AMP_MAX ? amp[ch] : 0;
                    t->phs[ch] = 128 - phs[ch] * 32 + shift;
                }
            }
        }

        s->tonal_bounds[group][sf_idx][1] = s->ntones;
    }

    return 0;
}
