static int asf_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    ASFContext *asf = s->priv_data;
    ASFStream *stream;
    int64_t duration;
    AVCodecContext *codec;

    codec = &s->streams[pkt->stream_index]->codec;
    stream = &asf->streams[pkt->stream_index];

    //XXX /FIXME use duration from AVPacket
    if (codec->codec_type == CODEC_TYPE_AUDIO) {
        duration = (codec->frame_number * codec->frame_size * int64_t_C(10000000)) /
            codec->sample_rate;
    } else {
        duration = av_rescale(codec->frame_number * codec->frame_rate_base, 10000000, codec->frame_rate);
    }
    if (duration > asf->duration)
        asf->duration = duration;

    put_frame(s, stream, pkt->pts, pkt->data, pkt->size, pkt->flags);
    return 0;
}
