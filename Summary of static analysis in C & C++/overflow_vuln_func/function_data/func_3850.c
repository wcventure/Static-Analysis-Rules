static int ac3_decode_frame(AVCodecContext * avctx, void *data,
                            int *got_frame_ptr, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    AC3DecodeContext *s = avctx->priv_data;
    int blk, ch, err, ret;
    const uint8_t *channel_map;
    const float *output[AC3_MAX_CHANNELS];

    /
    if (buf_size >= 2 && AV_RB16(buf) == 0x770B) {
        // seems to be byte-swapped AC-3
        int cnt = FFMIN(buf_size, AC3_FRAME_BUFFER_SIZE) >> 1;
        s->dsp.bswap16_buf((uint16_t *)s->input_buffer, (const uint16_t *)buf, cnt);
    } else
        memcpy(s->input_buffer, buf, FFMIN(buf_size, AC3_FRAME_BUFFER_SIZE));
    buf = s->input_buffer;
    /
    init_get_bits(&s->gbc, buf, buf_size * 8);

    /
    err = parse_frame_header(s);

    if (err) {
        switch (err) {
        case AAC_AC3_PARSE_ERROR_SYNC:
            av_log(avctx, AV_LOG_ERROR, "frame sync error\n");
            return -1;
        case AAC_AC3_PARSE_ERROR_BSID:
            av_log(avctx, AV_LOG_ERROR, "invalid bitstream id\n");
            break;
        case AAC_AC3_PARSE_ERROR_SAMPLE_RATE:
            av_log(avctx, AV_LOG_ERROR, "invalid sample rate\n");
            break;
        case AAC_AC3_PARSE_ERROR_FRAME_SIZE:
            av_log(avctx, AV_LOG_ERROR, "invalid frame size\n");
            break;
        case AAC_AC3_PARSE_ERROR_FRAME_TYPE:
            /
            /
            if (s->frame_type == EAC3_FRAME_TYPE_DEPENDENT || s->substreamid) {
                av_log(avctx, AV_LOG_ERROR, "unsupported frame type : "
                       "skipping frame\n");
                *got_frame_ptr = 0;
                return s->frame_size;
            } else {
                av_log(avctx, AV_LOG_ERROR, "invalid frame type\n");
            }
            break;
        default:
            av_log(avctx, AV_LOG_ERROR, "invalid header\n");
            break;
        }
    } else {
        /
        if (s->frame_size > buf_size) {
            av_log(avctx, AV_LOG_ERROR, "incomplete frame\n");
            err = AAC_AC3_PARSE_ERROR_FRAME_SIZE;
        } else if (avctx->err_recognition & (AV_EF_CRCCHECK|AV_EF_CAREFUL)) {
            /
            if (av_crc(av_crc_get_table(AV_CRC_16_ANSI), 0, &buf[2],
                       s->frame_size - 2)) {
                av_log(avctx, AV_LOG_ERROR, "frame CRC mismatch\n");
                err = AAC_AC3_PARSE_ERROR_CRC;
            }
        }
    }

    /
    if (!err) {
        avctx->sample_rate = s->sample_rate;
        avctx->bit_rate    = s->bit_rate;

        /
        s->out_channels = s->channels;
        s->output_mode  = s->channel_mode;
        if (s->lfe_on)
            s->output_mode |= AC3_OUTPUT_LFEON;
        if (avctx->request_channels > 0 && avctx->request_channels <= 2 &&
                avctx->request_channels < s->channels) {
            s->out_channels = avctx->request_channels;
            s->output_mode  = avctx->request_channels == 1 ? AC3_CHMODE_MONO : AC3_CHMODE_STEREO;
            s->channel_layout = avpriv_ac3_channel_layout_tab[s->output_mode];
        }
        avctx->channels       = s->out_channels;
        avctx->channel_layout = s->channel_layout;

        s->loro_center_mix_level   = gain_levels[s->  center_mix_level];
        s->loro_surround_mix_level = gain_levels[s->surround_mix_level];
        s->ltrt_center_mix_level   = LEVEL_MINUS_3DB;
        s->ltrt_surround_mix_level = LEVEL_MINUS_3DB;
        /
        if (s->channels != s->out_channels && !((s->output_mode & AC3_OUTPUT_LFEON) &&
                s->fbw_channels == s->out_channels)) {
            set_downmix_coeffs(s);
        }
    } else if (!s->out_channels) {
        s->out_channels = avctx->channels;
        if (s->out_channels < s->channels)
            s->output_mode  = s->out_channels == 1 ? AC3_CHMODE_MONO : AC3_CHMODE_STEREO;
    }
    if (avctx->channels != s->out_channels) {
        av_log(avctx, AV_LOG_ERROR, "channel number mismatching on damaged frame\n");
        return AVERROR_INVALIDDATA;
    }
    /
    avctx->audio_service_type = s->bitstream_mode;
    if (s->bitstream_mode == 0x7 && s->channels > 1)
        avctx->audio_service_type = AV_AUDIO_SERVICE_TYPE_KARAOKE;

    /
    avctx->channels = s->out_channels;
    s->frame.nb_samples = s->num_blocks * 256;
    if ((ret = ff_get_buffer(avctx, &s->frame)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }

    /
    channel_map = ff_ac3_dec_channel_map[s->output_mode & ~AC3_OUTPUT_LFEON][s->lfe_on];
    for (ch = 0; ch < s->channels; ch++) {
        if (ch < s->out_channels)
            s->outptr[channel_map[ch]] = (float *)s->frame.data[ch];
        else
            s->outptr[ch] = s->output[ch];
        output[ch] = s->output[ch];
    }
    for (blk = 0; blk < s->num_blocks; blk++) {
        if (!err && decode_audio_block(s, blk)) {
            av_log(avctx, AV_LOG_ERROR, "error decoding the audio block\n");
            err = 1;
        }
        if (err)
            for (ch = 0; ch < s->out_channels; ch++)
                memcpy(s->outptr[channel_map[ch]], output[ch], 1024);
        for (ch = 0; ch < s->out_channels; ch++) {
            output[ch] = s->outptr[channel_map[ch]];
            s->outptr[channel_map[ch]] += AC3_BLOCK_SIZE;
        }
    }

    s->frame.decode_error_flags = err ? FF_DECODE_ERROR_INVALID_BITSTREAM : 0;

    /
    for (ch = 0; ch < s->out_channels; ch++)
        memcpy(s->output[ch], output[ch], 1024);

    *got_frame_ptr   = 1;
    *(AVFrame *)data = s->frame;

    return FFMIN(buf_size, s->frame_size);
}
