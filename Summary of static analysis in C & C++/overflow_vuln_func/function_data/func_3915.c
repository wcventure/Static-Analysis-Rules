static int ffm_is_avail_data(AVFormatContext *s, int size)
{
    FFMContext *ffm = s->priv_data;
    int64_t pos, avail_size;
    int len;

    len = ffm->packet_end - ffm->packet_ptr;
    if (size <= len)
        return 1;
    pos = avio_tell(s->pb);
    if (!ffm->write_index) {
        if (pos == ffm->file_size)
            return AVERROR_EOF;
        avail_size = ffm->file_size - pos;
    } else {
    if (pos == ffm->write_index) {
        /
        if (ffm->server_attached)
            return AVERROR(EAGAIN);
        else
            return AVERROR_INVALIDDATA;
    } else if (pos < ffm->write_index) {
        avail_size = ffm->write_index - pos;
    } else {
        avail_size = (ffm->file_size - pos) + (ffm->write_index - FFM_PACKET_SIZE);
    }
    }
    avail_size = (avail_size / ffm->packet_size) * (ffm->packet_size - FFM_HEADER_SIZE) + len;
    if (size <= avail_size)
        return 1;
    else if (ffm->server_attached)
        return AVERROR(EAGAIN);
    else
        return AVERROR_INVALIDDATA;
}
