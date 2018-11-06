static int decode_scalefactors(AACContext *ac, float sf[120], GetBitContext *gb,
                               unsigned int global_gain,
                               IndividualChannelStream *ics,
                               enum BandType band_type[120],
                               int band_type_run_end[120])
{
    int g, i, idx = 0;
    int offset[3] = { global_gain, global_gain - 90, 0 };
    int clipped_offset;
    int noise_flag = 1;
    static const char *const sf_str[3] = { "Global gain", "Noise gain", "Intensity stereo position" };
    for (g = 0; g < ics->num_window_groups; g++) {
        for (i = 0; i < ics->max_sfb;) {
            int run_end = band_type_run_end[idx];
            if (band_type[idx] == ZERO_BT) {
                for (; i < run_end; i++, idx++)
                    sf[idx] = 0.;
            } else if ((band_type[idx] == INTENSITY_BT) || (band_type[idx] == INTENSITY_BT2)) {
                for (; i < run_end; i++, idx++) {
                    offset[2] += get_vlc2(gb, vlc_scalefactors.table, 7, 3) - 60;
                    clipped_offset = av_clip(offset[2], -155, 100);
                    if (offset[2] != clipped_offset) {
                        av_log_ask_for_sample(ac->avctx, "Intensity stereo "
                                "position clipped (%d -> %d).\nIf you heard an "
                                "audible artifact, there may be a bug in the "
                                "decoder. ", offset[2], clipped_offset);
                    }
                    sf[idx] = ff_aac_pow2sf_tab[-clipped_offset + POW_SF2_ZERO];
                }
            } else if (band_type[idx] == NOISE_BT) {
                for (; i < run_end; i++, idx++) {
                    if (noise_flag-- > 0)
                        offset[1] += get_bits(gb, 9) - 256;
                    else
                        offset[1] += get_vlc2(gb, vlc_scalefactors.table, 7, 3) - 60;
                    clipped_offset = av_clip(offset[1], -100, 155);
                    if (offset[1] != clipped_offset) {
                        av_log_ask_for_sample(ac->avctx, "Noise gain clipped "
                                "(%d -> %d).\nIf you heard an audible "
                                "artifact, there may be a bug in the decoder. ",
                                offset[1], clipped_offset);
                    }
                    sf[idx] = -ff_aac_pow2sf_tab[clipped_offset + POW_SF2_ZERO];
                }
            } else {
                for (; i < run_end; i++, idx++) {
                    offset[0] += get_vlc2(gb, vlc_scalefactors.table, 7, 3) - 60;
                    if (offset[0] > 255U) {
                        av_log(ac->avctx, AV_LOG_ERROR,
                               "%s (%d) out of range.\n", sf_str[0], offset[0]);
                        return -1;
                    }
                    sf[idx] = -ff_aac_pow2sf_tab[offset[0] - 100 + POW_SF2_ZERO];
                }
            }
        }
    }
    return 0;
}
