static int av_cold libopus_encode_init(AVCodecContext *avctx)
{
    LibopusEncContext *opus = avctx->priv_data;
    const uint8_t *channel_mapping;
    OpusMSEncoder *enc;
    int ret = OPUS_OK;
    int coupled_stream_count, header_size, frame_size;

    coupled_stream_count = opus_coupled_streams[avctx->channels - 1];
    opus->stream_count   = avctx->channels - coupled_stream_count;
    channel_mapping      = libav_libopus_channel_map[avctx->channels - 1];

    /
    if (avctx->channels > 8)
        av_log(avctx, AV_LOG_WARNING,
               "Channel layout undefined for %d channels.\n", avctx->channels);

    if (!avctx->bit_rate) {
        /
        avctx->bit_rate = 64000 * opus->stream_count +
                          32000 * coupled_stream_count;
        av_log(avctx, AV_LOG_WARNING,
               "No bit rate set. Defaulting to %d bps.\n", avctx->bit_rate);
    }

    if (avctx->bit_rate < 500 || avctx->bit_rate > 256000 * avctx->channels) {
        av_log(avctx, AV_LOG_ERROR, "The bit rate %d bps is unsupported. "
               "Please choose a value between 500 and %d.\n", avctx->bit_rate,
               256000 * avctx->channels);
        return AVERROR(EINVAL);
    }

    frame_size = opus->opts.frame_duration * 48000 / 1000;
    switch (frame_size) {
    case 120:
    case 240:
        if (opus->opts.application != OPUS_APPLICATION_RESTRICTED_LOWDELAY)
            av_log(avctx, AV_LOG_WARNING,
                   "LPC mode cannot be used with a frame duration of less "
                   "than 10ms. Enabling restricted low-delay mode.\n"
                   "Use a longer frame duration if this is not what you want.\n");
        /
        opus->opts.application = OPUS_APPLICATION_RESTRICTED_LOWDELAY;
    case 480:
    case 960:
    case 1920:
    case 2880:
        opus->opts.packet_size =
        avctx->frame_size      = frame_size * avctx->sample_rate / 48000;
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Invalid frame duration: %g.\n"
               "Frame duration must be exactly one of: 2.5, 5, 10, 20, 40 or 60.\n",
               opus->opts.frame_duration);
        return AVERROR(EINVAL);
    }

    if (avctx->compression_level < 0 || avctx->compression_level > 10) {
        av_log(avctx, AV_LOG_WARNING,
               "Compression level must be in the range 0 to 10. "
               "Defaulting to 10.\n");
        opus->opts.complexity = 10;
    } else {
        opus->opts.complexity = avctx->compression_level;
    }

    if (avctx->cutoff) {
        switch (avctx->cutoff) {
        case  4000:
            opus->opts.max_bandwidth = OPUS_BANDWIDTH_NARROWBAND;
            break;
        case  6000:
            opus->opts.max_bandwidth = OPUS_BANDWIDTH_MEDIUMBAND;
            break;
        case  8000:
            opus->opts.max_bandwidth = OPUS_BANDWIDTH_WIDEBAND;
            break;
        case 12000:
            opus->opts.max_bandwidth = OPUS_BANDWIDTH_SUPERWIDEBAND;
            break;
        case 20000:
            opus->opts.max_bandwidth = OPUS_BANDWIDTH_FULLBAND;
            break;
        default:
            av_log(avctx, AV_LOG_WARNING,
                   "Invalid frequency cutoff: %d. Using default maximum bandwidth.\n"
                   "Cutoff frequency must be exactly one of: 4000, 6000, 8000, 12000 or 20000.\n",
                   avctx->cutoff);
            avctx->cutoff = 0;
        }
    }

    enc = opus_multistream_encoder_create(avctx->sample_rate, avctx->channels,
                                          opus->stream_count,
                                          coupled_stream_count,
                                          channel_mapping,
                                          opus->opts.application, &ret);
    if (ret != OPUS_OK) {
        av_log(avctx, AV_LOG_ERROR,
               "Failed to create encoder: %s\n", opus_strerror(ret));
        return ff_opus_error_to_averror(ret);
    }

    ret = libopus_configure_encoder(avctx, enc, &opus->opts);
    if (ret != OPUS_OK) {
        ret = ff_opus_error_to_averror(ret);
        goto fail;
    }

    header_size = 19 + (avctx->channels > 2 ? 2 + avctx->channels : 0);
    avctx->extradata = av_malloc(header_size + FF_INPUT_BUFFER_PADDING_SIZE);
    if (!avctx->extradata) {
        av_log(avctx, AV_LOG_ERROR, "Failed to allocate extradata.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    avctx->extradata_size = header_size;

    opus->samples = av_mallocz(frame_size * avctx->channels *
                               av_get_bytes_per_sample(avctx->sample_fmt));
    if (!opus->samples) {
        av_log(avctx, AV_LOG_ERROR, "Failed to allocate samples buffer.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    ret = opus_multistream_encoder_ctl(enc, OPUS_GET_LOOKAHEAD(&avctx->initial_padding));
    if (ret != OPUS_OK)
        av_log(avctx, AV_LOG_WARNING,
               "Unable to get number of lookahead samples: %s\n",
               opus_strerror(ret));

    libopus_write_header(avctx, opus->stream_count, coupled_stream_count,
                         opus_vorbis_channel_map[avctx->channels - 1]);

    ff_af_queue_init(avctx, &opus->afq);

    opus->enc = enc;

    return 0;

fail:
    opus_multistream_encoder_destroy(enc);
    av_freep(&avctx->extradata);
    return ret;
}
