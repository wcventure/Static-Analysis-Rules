static int ffm_read_data(AVFormatContext *s,
                         uint8_t *buf, int size, int header)
{
    FFMContext *ffm = s->priv_data;
    AVIOContext *pb = s->pb;
    int len, fill_size, size1, frame_offset, id;

    size1 = size;
    while (size > 0) {
    redo:
        len = ffm->packet_end - ffm->packet_ptr;
        if (len < 0)
            return -1;
        if (len > size)
            len = size;
        if (len == 0) {
            if (avio_tell(pb) == ffm->file_size)
                avio_seek(pb, ffm->packet_size, SEEK_SET);
    retry_read:
            if (pb->buffer_size != ffm->packet_size) {
                int64_t tell = avio_tell(pb);
                ffio_set_buf_size(pb, ffm->packet_size);
                avio_seek(pb, tell, SEEK_SET);
            }
            id = avio_rb16(pb); /
            if (id != PACKET_ID)
                if (ffm_resync(s, id) < 0)
                    return -1;
            fill_size = avio_rb16(pb);
            ffm->dts = avio_rb64(pb);
            frame_offset = avio_rb16(pb);
            avio_read(pb, ffm->packet, ffm->packet_size - FFM_HEADER_SIZE);
            ffm->packet_end = ffm->packet + (ffm->packet_size - FFM_HEADER_SIZE - fill_size);
            if (ffm->packet_end < ffm->packet || frame_offset < 0)
                return -1;
            /
            if (ffm->first_packet || (frame_offset & 0x8000)) {
                if (!frame_offset) {
                    /
                    if (avio_tell(pb) >= ffm->packet_size * 3) {
                        avio_seek(pb, -ffm->packet_size * 2, SEEK_CUR);
                        goto retry_read;
                    }
                    /
                    return 0;
                }
                ffm->first_packet = 0;
                if ((frame_offset & 0x7fff) < FFM_HEADER_SIZE)
                    return -1;
                ffm->packet_ptr = ffm->packet + (frame_offset & 0x7fff) - FFM_HEADER_SIZE;
                if (!header)
                    break;
            } else {
                ffm->packet_ptr = ffm->packet;
            }
            goto redo;
        }
        memcpy(buf, ffm->packet_ptr, len);
        buf += len;
        ffm->packet_ptr += len;
        size -= len;
        header = 0;
    }
    return size1 - size;
}
