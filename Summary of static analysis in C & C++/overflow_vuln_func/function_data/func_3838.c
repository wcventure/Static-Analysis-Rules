static int decode_audio_specific_config(AACContext *ac,
                                        AVCodecContext *avctx,
                                        MPEG4AudioConfig *m4ac,
                                        const uint8_t *data, int bit_size,
                                        int sync_extension)
{
    GetBitContext gb;
    int i, ret;

    ff_dlog(avctx, "audio specific config size %d\n", bit_size >> 3);
    for (i = 0; i < bit_size >> 3; i++)
        ff_dlog(avctx, "%02x ", data[i]);
    ff_dlog(avctx, "\n");

    if ((ret = init_get_bits(&gb, data, bit_size)) < 0)
        return ret;

    if ((i = avpriv_mpeg4audio_get_config(m4ac, data, bit_size,
                                          sync_extension)) < 0)
        return AVERROR_INVALIDDATA;
    if (m4ac->sampling_index > 12) {
        av_log(avctx, AV_LOG_ERROR,
               "invalid sampling rate index %d\n",
               m4ac->sampling_index);
        return AVERROR_INVALIDDATA;
    }
    if (m4ac->object_type == AOT_ER_AAC_LD &&
        (m4ac->sampling_index < 3 || m4ac->sampling_index > 7)) {
        av_log(avctx, AV_LOG_ERROR,
               "invalid low delay sampling rate index %d\n",
               m4ac->sampling_index);
        return AVERROR_INVALIDDATA;
    }

    skip_bits_long(&gb, i);

    switch (m4ac->object_type) {
    case AOT_AAC_MAIN:
    case AOT_AAC_LC:
    case AOT_AAC_LTP:
    case AOT_ER_AAC_LC:
    case AOT_ER_AAC_LD:
        if ((ret = decode_ga_specific_config(ac, avctx, &gb,
                                            m4ac, m4ac->chan_config)) < 0)
            return ret;
        break;
    case AOT_ER_AAC_ELD:
        if ((ret = decode_eld_specific_config(ac, avctx, &gb,
                                              m4ac, m4ac->chan_config)) < 0)
            return ret;
        break;
    default:
        avpriv_report_missing_feature(avctx,
                                      "Audio object type %s%d",
                                      m4ac->sbr == 1 ? "SBR+" : "",
                                      m4ac->object_type);
        return AVERROR(ENOSYS);
    }

    ff_dlog(avctx,
            "AOT %d chan config %d sampling index %d (%d) SBR %d PS %d\n",
            m4ac->object_type, m4ac->chan_config, m4ac->sampling_index,
            m4ac->sample_rate, m4ac->sbr,
            m4ac->ps);

    return get_bits_count(&gb);
}
