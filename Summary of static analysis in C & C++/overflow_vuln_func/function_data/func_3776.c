static int encode_audio_frame(AVFormatContext *s, OutputStream *ost,
                              const uint8_t *buf, int buf_size)
{
    AVCodecContext *enc = ost->st->codec;
    AVFrame *frame = NULL;
    AVPacket pkt;
    int ret, got_packet;

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    if (buf) {
        if (!ost->output_frame) {
            ost->output_frame = avcodec_alloc_frame();
            if (!ost->output_frame) {
                av_log(NULL, AV_LOG_FATAL, "out-of-memory in encode_audio_frame()\n");
                exit_program(1);
            }
        }
        frame = ost->output_frame;
        if (frame->extended_data != frame->data)
            av_freep(&frame->extended_data);
        avcodec_get_frame_defaults(frame);

        frame->nb_samples  = buf_size /
                             (enc->channels * av_get_bytes_per_sample(enc->sample_fmt));
        if ((ret = avcodec_fill_audio_frame(frame, enc->channels, enc->sample_fmt,
                                            buf, buf_size, 1)) < 0) {
            av_log(NULL, AV_LOG_FATAL, "Audio encoding failed\n");
            exit_program(1);
        }

        if (!check_recording_time(ost))
            return 0;

        frame->pts = ost->sync_opts;
        ost->sync_opts += frame->nb_samples;
    }

    got_packet = 0;
    if (avcodec_encode_audio2(enc, &pkt, frame, &got_packet) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Audio encoding failed\n");
        exit_program(1);
    }

    if (got_packet) {
        if (pkt.pts != AV_NOPTS_VALUE)
            pkt.pts      = av_rescale_q(pkt.pts,      enc->time_base, ost->st->time_base);
        if (pkt.dts != AV_NOPTS_VALUE)
            pkt.dts      = av_rescale_q(pkt.dts,      enc->time_base, ost->st->time_base);
        if (pkt.duration > 0)
            pkt.duration = av_rescale_q(pkt.duration, enc->time_base, ost->st->time_base);

        write_frame(s, &pkt, ost);

        audio_size += pkt.size;
    }

    return pkt.size;
}
