static int flac_encode_frame(AVCodecContext *avctx, AVPacket *avpkt,
                             const AVFrame *frame, int *got_packet_ptr)
{
    FlacEncodeContext *s;
    const int16_t *samples;
    int frame_bytes, out_bytes, ret;

    s = avctx->priv_data;

    /
    if (!frame) {
        s->max_framesize = s->max_encoded_framesize;
        av_md5_final(s->md5ctx, s->md5sum);
        write_streaminfo(s, avctx->extradata);
        return 0;
    }
    samples = (const int16_t *)frame->data[0];

    /
    if (frame->nb_samples < s->frame.blocksize) {
        s->max_framesize = ff_flac_get_max_frame_size(frame->nb_samples,
                                                      s->channels, 16);
    }

    init_frame(s, frame->nb_samples);

    copy_samples(s, samples);

    channel_decorrelation(s);

    remove_wasted_bits(s);

    frame_bytes = encode_frame(s);

    /
    if (frame_bytes > s->max_framesize) {
        s->frame.verbatim_only = 1;
        frame_bytes = encode_frame(s);
    }

    if ((ret = ff_alloc_packet(avpkt, frame_bytes))) {
        av_log(avctx, AV_LOG_ERROR, "Error getting output packet\n");
        return ret;
    }

    out_bytes = write_frame(s, avpkt);

    s->frame_count++;
    s->sample_count += frame->nb_samples;
    if ((ret = update_md5_sum(s, samples)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "Error updating MD5 checksum\n");
        return ret;
    }
    if (out_bytes > s->max_encoded_framesize)
        s->max_encoded_framesize = out_bytes;
    if (out_bytes < s->min_framesize)
        s->min_framesize = out_bytes;

    avpkt->pts      = frame->pts;
    avpkt->duration = ff_samples_to_time_base(avctx, frame->nb_samples);
    avpkt->size     = out_bytes;
    *got_packet_ptr = 1;
    return 0;
}
