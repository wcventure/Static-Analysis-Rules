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

    if (buf && buf_size) {
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
            av_log(NULL, AV_LOG_FATAL, "Audio encoding failed (avcodec_fill_audio_frame)\n");
            exit_program(1);
        }

        frame->pts = ost->sync_opts;
        ost->sync_opts += frame->nb_samples;
    }

    got_packet = 0;
    update_benchmark(NULL);
    if (avcodec_encode_audio2(enc, &pkt, frame, &got_packet) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Audio encoding failed (avcodec_encode_audio2)\n");
        exit_program(1);
    }
    update_benchmark("encode_audio %d.%d", ost->file_index, ost->index);

    ret = pkt.size;

    if (got_packet) {
        if (pkt.pts != AV_NOPTS_VALUE)
            pkt.pts      = av_rescale_q(pkt.pts,      enc->time_base, ost->st->time_base);
        if (pkt.dts != AV_NOPTS_VALUE) {
            int64_t max = ost->st->cur_dts + !(s->oformat->flags & AVFMT_TS_NONSTRICT);
            pkt.dts      = av_rescale_q(pkt.dts,      enc->time_base, ost->st->time_base);
            if (ost->st->cur_dts && ost->st->cur_dts != AV_NOPTS_VALUE &&  max > pkt.dts) {
                av_log(s, max - pkt.dts > 2 ? AV_LOG_WARNING : AV_LOG_DEBUG, "Audio timestamp %"PRId64" < %"PRId64" invalid, cliping\n", pkt.dts, max);
                pkt.pts = pkt.dts = max;
            }
        }
        if (pkt.duration > 0)
            pkt.duration = av_rescale_q(pkt.duration, enc->time_base, ost->st->time_base);

        write_frame(s, &pkt, ost);

        audio_size += pkt.size;

        av_free_packet(&pkt);
    }

    if (debug_ts) {
        av_log(NULL, AV_LOG_INFO, "encoder -> type:audio "
               "pkt_pts:%s pkt_pts_time:%s pkt_dts:%s pkt_dts_time:%s\n",
               av_ts2str(pkt.pts), av_ts2timestr(pkt.pts, &ost->st->time_base),
               av_ts2str(pkt.dts), av_ts2timestr(pkt.dts, &ost->st->time_base));
    }

    return ret;
}
