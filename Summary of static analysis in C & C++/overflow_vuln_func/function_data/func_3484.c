static int calculate_bitrate(AVFormatContext *s)
{
    AVIContext *avi = s->priv_data;
    int i, j;
    int64_t lensum = 0;
    int64_t maxpos = 0;

    for (i = 0; i<s->nb_streams; i++) {
        int64_t len = 0;
        AVStream *st = s->streams[i];

        if (!st->nb_index_entries)
            continue;

        for (j = 0; j < st->nb_index_entries; j++)
            len += st->index_entries[j].size;
        maxpos = FFMAX(maxpos, st->index_entries[j-1].pos);
        lensum += len;
    }
    if (maxpos < avi->io_fsize*9/10) // index does not cover the whole file
        return 0;
    if (lensum*9/10 > maxpos || lensum < maxpos*9/10) // frame sum and filesize mismatch
        return 0;

    for (i = 0; i<s->nb_streams; i++) {
        int64_t len = 0;
        AVStream *st = s->streams[i];
        int64_t duration;

        for (j = 0; j < st->nb_index_entries; j++)
            len += st->index_entries[j].size;

        if (st->nb_index_entries < 2 || st->codec->bit_rate > 0)
            continue;
        duration = st->index_entries[j-1].timestamp - st->index_entries[0].timestamp;
        st->codec->bit_rate = av_rescale(8*len, st->time_base.den, duration * st->time_base.num);
    }
    return 1;
}
