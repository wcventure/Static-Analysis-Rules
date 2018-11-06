static AVStream * init_stream(AVFormatContext *s)
{
    BinDemuxContext *bin = s->priv_data;
    AVStream *st = avformat_new_stream(s, NULL);
    if (!st)
        return NULL;
    st->codec->codec_tag   = 0;
    st->codec->codec_type  = AVMEDIA_TYPE_VIDEO;

    if (!bin->width) {
        st->codec->width  = (80<<3);
        st->codec->height = (25<<4);
    }

    avpriv_set_pts_info(st, 60, bin->framerate.den, bin->framerate.num);

    /
    bin->chars_per_frame = FFMAX(av_q2d(st->time_base) * bin->chars_per_frame, 1);

    return st;
}
