static void flush_encoders(void)
{
    int i, ret;

    for (i = 0; i < nb_output_streams; i++) {
        OutputStream   *ost = output_streams[i];
        AVCodecContext *enc = ost->st->codec;
        AVFormatContext *os = output_files[ost->file_index]->ctx;
        int stop_encoding = 0;

        if (!ost->encoding_needed)
            continue;

        if (ost->st->codec->codec_type == AVMEDIA_TYPE_AUDIO && enc->frame_size <= 1)
            continue;
        if (ost->st->codec->codec_type == AVMEDIA_TYPE_VIDEO && (os->oformat->flags & AVFMT_RAWPICTURE) && enc->codec->id == CODEC_ID_RAWVIDEO)
            continue;

        for (;;) {
            AVPacket pkt;
            int fifo_bytes, got_packet;
            av_init_packet(&pkt);
            pkt.data = NULL;
            pkt.size = 0;

            switch (ost->st->codec->codec_type) {
            case AVMEDIA_TYPE_AUDIO:
                fifo_bytes = av_fifo_size(ost->fifo);
                if (fifo_bytes > 0) {
                    /
                    int frame_bytes = fifo_bytes;

                    av_fifo_generic_read(ost->fifo, audio_buf, fifo_bytes, NULL);

                    encode_audio_frame(os, ost, audio_buf, frame_bytes);
                } else {
                    /
                    if (encode_audio_frame(os, ost, NULL, 0) == 0) {
                        stop_encoding = 1;
                        break;
                    }
                }
                break;
            case AVMEDIA_TYPE_VIDEO:
                update_benchmark(NULL);
                ret = avcodec_encode_video2(enc, &pkt, NULL, &got_packet);
                update_benchmark("encode_video %d.%d", ost->file_index, ost->index);
                if (ret < 0) {
                    av_log(NULL, AV_LOG_FATAL, "Video encoding failed\n");
                    exit_program(1);
                }
                video_size += pkt.size;
                if (ost->logfile && enc->stats_out) {
                    fprintf(ost->logfile, "%s", enc->stats_out);
                }
                if (!got_packet) {
                    stop_encoding = 1;
                    break;
                }
                if (pkt.pts != AV_NOPTS_VALUE)
                    pkt.pts = av_rescale_q(pkt.pts, enc->time_base, ost->st->time_base);
                if (pkt.dts != AV_NOPTS_VALUE)
                    pkt.dts = av_rescale_q(pkt.dts, enc->time_base, ost->st->time_base);
                write_frame(os, &pkt, ost);
                break;
            default:
                stop_encoding = 1;
            }
            if (stop_encoding)
                break;
        }
    }
}
