static av_cold int aac_decode_init(AVCodecContext *avctx)
{
    AACContext *ac = avctx->priv_data;
    int ret;

    ac->avctx = avctx;
    ac->oc[1].m4ac.sample_rate = avctx->sample_rate;

    aacdec_init(ac);
#if USE_FIXED
    avctx->sample_fmt = AV_SAMPLE_FMT_S32P;
#else
    avctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
#endif /

    if (avctx->extradata_size > 0) {
        if ((ret = decode_audio_specific_config(ac, ac->avctx, &ac->oc[1].m4ac,
                                                avctx->extradata,
                                                avctx->extradata_size * 8,
                                                1)) < 0)
            return ret;
    } else {
        int sr, i;
        uint8_t layout_map[MAX_ELEM_ID*4][3];
        int layout_map_tags;

        sr = sample_rate_idx(avctx->sample_rate);
        ac->oc[1].m4ac.sampling_index = sr;
        ac->oc[1].m4ac.channels = avctx->channels;
        ac->oc[1].m4ac.sbr = -1;
        ac->oc[1].m4ac.ps = -1;

        for (i = 0; i < FF_ARRAY_ELEMS(ff_mpeg4audio_channels); i++)
            if (ff_mpeg4audio_channels[i] == avctx->channels)
                break;
        if (i == FF_ARRAY_ELEMS(ff_mpeg4audio_channels)) {
            i = 0;
        }
        ac->oc[1].m4ac.chan_config = i;

        if (ac->oc[1].m4ac.chan_config) {
            int ret = set_default_channel_config(avctx, layout_map,
                &layout_map_tags, ac->oc[1].m4ac.chan_config);
            if (!ret)
                output_configure(ac, layout_map, layout_map_tags,
                                 OC_GLOBAL_HDR, 0);
            else if (avctx->err_recognition & AV_EF_EXPLODE)
                return AVERROR_INVALIDDATA;
        }
    }

    if (avctx->channels > MAX_CHANNELS) {
        av_log(avctx, AV_LOG_ERROR, "Too many channels\n");
        return AVERROR_INVALIDDATA;
    }

    AAC_INIT_VLC_STATIC( 0, 304);
    AAC_INIT_VLC_STATIC( 1, 270);
    AAC_INIT_VLC_STATIC( 2, 550);
    AAC_INIT_VLC_STATIC( 3, 300);
    AAC_INIT_VLC_STATIC( 4, 328);
    AAC_INIT_VLC_STATIC( 5, 294);
    AAC_INIT_VLC_STATIC( 6, 306);
    AAC_INIT_VLC_STATIC( 7, 268);
    AAC_INIT_VLC_STATIC( 8, 510);
    AAC_INIT_VLC_STATIC( 9, 366);
    AAC_INIT_VLC_STATIC(10, 462);

    AAC_RENAME(ff_aac_sbr_init)();

#if USE_FIXED
    ac->fdsp = avpriv_alloc_fixed_dsp(avctx->flags & AV_CODEC_FLAG_BITEXACT);
#else
    ac->fdsp = avpriv_float_dsp_alloc(avctx->flags & AV_CODEC_FLAG_BITEXACT);
#endif /
    if (!ac->fdsp) {
        return AVERROR(ENOMEM);
    }

    ac->random_state = 0x1f2e3d4c;

    ff_aac_tableinit();

    INIT_VLC_STATIC(&vlc_scalefactors, 7,
                    FF_ARRAY_ELEMS(ff_aac_scalefactor_code),
                    ff_aac_scalefactor_bits,
                    sizeof(ff_aac_scalefactor_bits[0]),
                    sizeof(ff_aac_scalefactor_bits[0]),
                    ff_aac_scalefactor_code,
                    sizeof(ff_aac_scalefactor_code[0]),
                    sizeof(ff_aac_scalefactor_code[0]),
                    352);

    AAC_RENAME_32(ff_mdct_init)(&ac->mdct,       11, 1, 1.0 / RANGE15(1024.0));
    AAC_RENAME_32(ff_mdct_init)(&ac->mdct_ld,    10, 1, 1.0 / RANGE15(512.0));
    AAC_RENAME_32(ff_mdct_init)(&ac->mdct_small,  8, 1, 1.0 / RANGE15(128.0));
    AAC_RENAME_32(ff_mdct_init)(&ac->mdct_ltp,   11, 0, RANGE15(-2.0));
#if !USE_FIXED
    ret = ff_imdct15_init(&ac->mdct480, 5);
    if (ret < 0)
        return ret;
#endif
    // window initialization
    AAC_RENAME(ff_kbd_window_init)(AAC_RENAME(ff_aac_kbd_long_1024), 4.0, 1024);
    AAC_RENAME(ff_kbd_window_init)(AAC_RENAME(ff_aac_kbd_short_128), 6.0, 128);
    AAC_RENAME(ff_init_ff_sine_windows)(10);
    AAC_RENAME(ff_init_ff_sine_windows)( 9);
    AAC_RENAME(ff_init_ff_sine_windows)( 7);

    AAC_RENAME(cbrt_tableinit)();

    return 0;
}
