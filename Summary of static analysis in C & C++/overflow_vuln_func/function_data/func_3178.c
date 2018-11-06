static int vqf_read_seek(AVFormatContext *s,
                         int stream_index, int64_t timestamp, int flags)
{
    VqfContext *c = s->priv_data;
    AVStream *st;
    int ret;
    int64_t pos;

    st = s->streams[stream_index];
    pos = av_rescale_rnd(timestamp * st->codec->bit_rate,
                         st->time_base.num,
                         st->time_base.den * (int64_t)c->frame_bit_len,
                         (flags & AVSEEK_FLAG_BACKWARD) ?
                                                   AV_ROUND_DOWN : AV_ROUND_UP);
    pos *= c->frame_bit_len;

    st->cur_dts = av_rescale(pos, st->time_base.den,
                             st->codec->bit_rate * (int64_t)st->time_base.num);

    if ((ret = avio_seek(s->pb, ((pos-7) >> 3) + s->internal->data_offset, SEEK_SET)) < 0)
        return ret;

    c->remaining_bits = -7 - ((pos-7)&7);
    return 0;
}
