static int transcode_audio(InputStream *ist, AVPacket *pkt, int *got_output)
{
    AVFrame *decoded_frame;
    AVCodecContext *avctx = ist->st->codec;
    int bps = av_get_bytes_per_sample(ist->st->codec->sample_fmt);
    int i, ret;

    if (!ist->decoded_frame && !(ist->decoded_frame = avcodec_alloc_frame()))
        return AVERROR(ENOMEM);
    else
        avcodec_get_frame_defaults(ist->decoded_frame);
    decoded_frame = ist->decoded_frame;

    ret = avcodec_decode_audio4(avctx, decoded_frame, got_output, pkt);
    if (ret < 0) {
        return ret;
    }

    if (!*got_output) {
        /
        return ret;
    }

    /
    if (decoded_frame->pts != AV_NOPTS_VALUE)
        ist->next_dts = decoded_frame->pts;

    /
    ist->next_dts += ((int64_t)AV_TIME_BASE * decoded_frame->nb_samples) /
                     avctx->sample_rate;

    // preprocess audio (volume)
    if (audio_volume != 256) {
        int decoded_data_size = decoded_frame->nb_samples * avctx->channels * bps;
        void *samples = decoded_frame->data[0];
        switch (avctx->sample_fmt) {
        case AV_SAMPLE_FMT_U8:
        {
            uint8_t *volp = samples;
            for (i = 0; i < (decoded_data_size / sizeof(*volp)); i++) {
                int v = (((*volp - 128) * audio_volume + 128) >> 8) + 128;
                *volp++ = av_clip_uint8(v);
            }
            break;
        }
        case AV_SAMPLE_FMT_S16:
        {
            int16_t *volp = samples;
            for (i = 0; i < (decoded_data_size / sizeof(*volp)); i++) {
                int v = ((*volp) * audio_volume + 128) >> 8;
                *volp++ = av_clip_int16(v);
            }
            break;
        }
        case AV_SAMPLE_FMT_S32:
        {
            int32_t *volp = samples;
            for (i = 0; i < (decoded_data_size / sizeof(*volp)); i++) {
                int64_t v = (((int64_t)*volp * audio_volume + 128) >> 8);
                *volp++ = av_clipl_int32(v);
            }
            break;
        }
        case AV_SAMPLE_FMT_FLT:
        {
            float *volp = samples;
            float scale = audio_volume / 256.f;
            for (i = 0; i < (decoded_data_size / sizeof(*volp)); i++) {
                *volp++ *= scale;
            }
            break;
        }
        case AV_SAMPLE_FMT_DBL:
        {
            double *volp = samples;
            double scale = audio_volume / 256.;
            for (i = 0; i < (decoded_data_size / sizeof(*volp)); i++) {
                *volp++ *= scale;
            }
            break;
        }
        default:
            av_log(NULL, AV_LOG_FATAL,
                   "Audio volume adjustment on sample format %s is not supported.\n",
                   av_get_sample_fmt_name(ist->st->codec->sample_fmt));
            exit_program(1);
        }
    }

    rate_emu_sleep(ist);

    for (i = 0; i < nb_output_streams; i++) {
        OutputStream *ost = output_streams[i];

        if (!check_output_constraints(ist, ost) || !ost->encoding_needed)
            continue;
        do_audio_out(output_files[ost->file_index]->ctx, ost, ist, decoded_frame);
    }

    return ret;
}
