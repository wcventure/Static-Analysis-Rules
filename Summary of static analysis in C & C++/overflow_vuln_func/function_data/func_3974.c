static inline int spx_strategy(AC3DecodeContext *s, int blk)
{
    GetBitContext *bc = &s->gbc;
    int fbw_channels = s->fbw_channels;
    int dst_start_freq, dst_end_freq, src_start_freq,
        start_subband, end_subband, ch;

    /
    if (s->channel_mode == AC3_CHMODE_MONO) {
        s->channel_uses_spx[1] = 1;
    } else {
        for (ch = 1; ch <= fbw_channels; ch++)
            s->channel_uses_spx[ch] = get_bits1(bc);
    }

    /
    dst_start_freq = get_bits(bc, 2);
    start_subband  = get_bits(bc, 3) + 2;
    if (start_subband > 7)
        start_subband += start_subband - 7;
    end_subband    = get_bits(bc, 3) + 5;
#if USE_FIXED
    s->spx_dst_end_freq = end_freq_inv_tab[end_subband-5];
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

    decode_band_structure(bc, blk, s->eac3, 0,
                          start_subband, end_subband,
                          ff_eac3_default_spx_band_struct,
                          &s->num_spx_bands,
                          s->spx_band_sizes);
    return 0;
}
