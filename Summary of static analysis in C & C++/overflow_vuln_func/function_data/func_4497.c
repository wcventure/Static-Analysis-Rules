static av_cold int twin_decode_init(AVCodecContext *avctx)
{
    int ret;
    TwinContext *tctx = avctx->priv_data;
    int isampf, ibps;

    tctx->avctx       = avctx;
    avctx->sample_fmt = AV_SAMPLE_FMT_FLTP;

    if (!avctx->extradata || avctx->extradata_size < 12) {
        av_log(avctx, AV_LOG_ERROR, "Missing or incomplete extradata\n");
        return AVERROR_INVALIDDATA;
    }
    avctx->channels = AV_RB32(avctx->extradata    ) + 1;
    avctx->bit_rate = AV_RB32(avctx->extradata + 4) * 1000;
    isampf          = AV_RB32(avctx->extradata + 8);
    switch (isampf) {
    case 44: avctx->sample_rate = 44100;         break;
    case 22: avctx->sample_rate = 22050;         break;
    case 11: avctx->sample_rate = 11025;         break;
    default: avctx->sample_rate = isampf * 1000; break;
    }

    if (avctx->channels > CHANNELS_MAX) {
        av_log(avctx, AV_LOG_ERROR, "Unsupported number of channels: %i\n",
               avctx->channels);
        return -1;
    }
    ibps = avctx->bit_rate / (1000 * avctx->channels);

    switch ((isampf << 8) +  ibps) {
    case (8 <<8) +  8: tctx->mtab = &mode_08_08; break;
    case (11<<8) +  8: tctx->mtab = &mode_11_08; break;
    case (11<<8) + 10: tctx->mtab = &mode_11_10; break;
    case (16<<8) + 16: tctx->mtab = &mode_16_16; break;
    case (22<<8) + 20: tctx->mtab = &mode_22_20; break;
    case (22<<8) + 24: tctx->mtab = &mode_22_24; break;
    case (22<<8) + 32: tctx->mtab = &mode_22_32; break;
    case (44<<8) + 40: tctx->mtab = &mode_44_40; break;
    case (44<<8) + 48: tctx->mtab = &mode_44_48; break;
    default:
        av_log(avctx, AV_LOG_ERROR, "This version does not support %d kHz - %d kbit/s/ch mode.\n", isampf, isampf);
        return -1;
    }

    ff_dsputil_init(&tctx->dsp, avctx);
    avpriv_float_dsp_init(&tctx->fdsp, avctx->flags & CODEC_FLAG_BITEXACT);
    if ((ret = init_mdct_win(tctx))) {
        av_log(avctx, AV_LOG_ERROR, "Error initializing MDCT\n");
        twin_decode_close(avctx);
        return ret;
    }
    init_bitstream_params(tctx);

    memset_float(tctx->bark_hist[0][0], 0.1, FF_ARRAY_ELEMS(tctx->bark_hist));

    avcodec_get_frame_defaults(&tctx->frame);
    avctx->coded_frame = &tctx->frame;

    return 0;
}
