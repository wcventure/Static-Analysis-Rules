static int applehttp_read_seek(AVFormatContext *s, int stream_index,
                               int64_t timestamp, int flags)
{
    AppleHTTPContext *c = s->priv_data;
    int pos = 0, i;
    struct variant *var = c->variants[0];

    if ((flags & AVSEEK_FLAG_BYTE) || !c->finished)
        return AVERROR(ENOSYS);

    /
    c->last_packet_dts = AV_NOPTS_VALUE;
    for (i = 0; i < c->n_variants; i++) {
        struct variant *var = c->variants[i];
        if (var->pb) {
            avio_close(var->pb);
            var->pb = NULL;
        }
        av_free_packet(&var->pkt);
        reset_packet(&var->pkt);
    }

    timestamp = av_rescale_rnd(timestamp, 1, stream_index >= 0 ?
                               s->streams[stream_index]->time_base.den :
                               AV_TIME_BASE, flags & AVSEEK_FLAG_BACKWARD ?
                               AV_ROUND_DOWN : AV_ROUND_UP);
    /
    for (i = 0; i < var->n_segments; i++) {
        if (timestamp >= pos && timestamp < pos + var->segments[i]->duration) {
            c->cur_seq_no = var->start_seq_no + i;
            return 0;
        }
        pos += var->segments[i]->duration;
    }
    return AVERROR(EIO);
}
