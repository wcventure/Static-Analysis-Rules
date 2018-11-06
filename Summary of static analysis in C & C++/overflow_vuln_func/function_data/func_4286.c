static int oma_read_seek(struct AVFormatContext *s,
                         int stream_index, int64_t timestamp, int flags)
{
    OMAContext *oc = s->priv_data;
    int err = ff_pcm_read_seek(s, stream_index, timestamp, flags);

    if (!oc->encrypted)
        return err;

    /
    if (err || avio_tell(s->pb) < oc->content_start)
        goto wipe;
    if ((err = avio_seek(s->pb, -8, SEEK_CUR)) < 0)
        goto wipe;
    if ((err = avio_read(s->pb, oc->iv, 8)) < 8) {
        if (err >= 0)
            err = AVERROR_EOF;
        goto wipe;
    }

    return 0;
wipe:
    memset(oc->iv, 0, 8);
    return err;
}
