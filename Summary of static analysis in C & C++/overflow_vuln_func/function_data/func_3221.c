static av_cold int tta_decode_init(AVCodecContext * avctx)
{
    TTAContext *s = avctx->priv_data;

    s->avctx = avctx;

    // 30bytes includes a seektable with one frame
    if (avctx->extradata_size < 30)
        return -1;

    init_get_bits(&s->gb, avctx->extradata, avctx->extradata_size * 8);
    if (show_bits_long(&s->gb, 32) == AV_RL32("TTA1"))
    {
        if (avctx->err_recognition & AV_EF_CRCCHECK) {
            s->crc_table = av_crc_get_table(AV_CRC_32_IEEE_LE);
            tta_check_crc(s, avctx->extradata, 18);
        }

        /
        skip_bits_long(&s->gb, 32);

        s->format = get_bits(&s->gb, 16);
        if (s->format > 2) {
            av_log(s->avctx, AV_LOG_ERROR, "Invalid format\n");
            return -1;
        }
        if (s->format == FORMAT_ENCRYPTED) {
            av_log_missing_feature(s->avctx, "Encrypted TTA", 0);
            return AVERROR(EINVAL);
        }
        avctx->channels = s->channels = get_bits(&s->gb, 16);
        avctx->bits_per_coded_sample = get_bits(&s->gb, 16);
        s->bps = (avctx->bits_per_coded_sample + 7) / 8;
        avctx->sample_rate = get_bits_long(&s->gb, 32);
        s->data_length = get_bits_long(&s->gb, 32);
        skip_bits_long(&s->gb, 32); // CRC32 of header

        if (s->channels == 0) {
            av_log(s->avctx, AV_LOG_ERROR, "Invalid number of channels\n");
            return AVERROR_INVALIDDATA;
        } else if (avctx->sample_rate == 0) {
            av_log(s->avctx, AV_LOG_ERROR, "Invalid samplerate\n");
            return AVERROR_INVALIDDATA;
        }

        switch(s->bps) {
        case 2:
            avctx->sample_fmt = AV_SAMPLE_FMT_S16;
            avctx->bits_per_raw_sample = 16;
            break;
        case 3:
            avctx->sample_fmt = AV_SAMPLE_FMT_S32;
            avctx->bits_per_raw_sample = 24;
            break;
        default:
            av_log(avctx, AV_LOG_ERROR, "Invalid/unsupported sample format.\n");
            return AVERROR_INVALIDDATA;
        }

        // prevent overflow
        if (avctx->sample_rate > 0x7FFFFF) {
            av_log(avctx, AV_LOG_ERROR, "sample_rate too large\n");
            return AVERROR(EINVAL);
        }
        s->frame_length = 256 * avctx->sample_rate / 245;

        s->last_frame_length = s->data_length % s->frame_length;
        s->total_frames = s->data_length / s->frame_length +
                        (s->last_frame_length ? 1 : 0);

        av_log(s->avctx, AV_LOG_DEBUG, "format: %d chans: %d bps: %d rate: %d block: %d\n",
            s->format, avctx->channels, avctx->bits_per_coded_sample, avctx->sample_rate,
            avctx->block_align);
        av_log(s->avctx, AV_LOG_DEBUG, "data_length: %d frame_length: %d last: %d total: %d\n",
            s->data_length, s->frame_length, s->last_frame_length, s->total_frames);

        // FIXME: seek table
        if (get_bits_left(&s->gb) < 32 * s->total_frames + 32)
            av_log(avctx, AV_LOG_WARNING, "Seek table missing or too small\n");
        else if (avctx->err_recognition & AV_EF_CRCCHECK) {
            if (tta_check_crc(s, avctx->extradata + 22, s->total_frames * 4))
                return AVERROR_INVALIDDATA;
        }
        skip_bits_long(&s->gb, 32 * s->total_frames);
        skip_bits_long(&s->gb, 32); // CRC32 of seektable

        if(s->frame_length >= UINT_MAX / (s->channels * sizeof(int32_t))){
            av_log(avctx, AV_LOG_ERROR, "frame_length too large\n");
            return -1;
        }

        if (s->bps == 2) {
            s->decode_buffer = av_mallocz(sizeof(int32_t)*s->frame_length*s->channels);
            if (!s->decode_buffer)
                return AVERROR(ENOMEM);
        }
        s->ch_ctx = av_malloc(avctx->channels * sizeof(*s->ch_ctx));
        if (!s->ch_ctx) {
            av_freep(&s->decode_buffer);
            return AVERROR(ENOMEM);
        }
    } else {
        av_log(avctx, AV_LOG_ERROR, "Wrong extradata present\n");
        return -1;
    }

    avcodec_get_frame_defaults(&s->frame);
    avctx->coded_frame = &s->frame;

    return 0;
}
