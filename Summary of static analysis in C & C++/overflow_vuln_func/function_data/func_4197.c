static int read_packet(AVFormatContext *s, AVPacket *pkt)
{
    PAFDemuxContext *p = s->priv_data;
    AVIOContext     *pb = s->pb;
    uint32_t        count, offset;
    int             size, i;

    if (p->current_frame >= p->nb_frames)
        return AVERROR_EOF;

    if (url_feof(pb))
        return AVERROR_EOF;

    if (p->got_audio) {
        if (av_new_packet(pkt, p->audio_size) < 0)
            return AVERROR(ENOMEM);

        memcpy(pkt->data, p->temp_audio_frame, p->audio_size);
        pkt->duration     = PAF_SOUND_SAMPLES * (p->audio_size / PAF_SOUND_FRAME_SIZE);
        pkt->flags       |= AV_PKT_FLAG_KEY;
        pkt->stream_index = 1;
        p->got_audio      = 0;
        return pkt->size;
    }

    count = (p->current_frame == 0) ? p->preload_count : p->blocks_count_table[p->current_frame - 1];
    for (i = 0; i < count; i++) {
        if (p->current_frame_block >= p->frame_blks)
            return AVERROR_INVALIDDATA;

        offset = p->blocks_offset_table[p->current_frame_block] & ~(1 << 31);
        if (p->blocks_offset_table[p->current_frame_block] & (1 << 31)) {
            if (offset > p->audio_size - p->buffer_size)
                return AVERROR_INVALIDDATA;

            avio_read(pb, p->audio_frame + offset, p->buffer_size);
            if (offset == (p->max_audio_blks - 2) * p->buffer_size) {
                memcpy(p->temp_audio_frame, p->audio_frame, p->audio_size);
                p->got_audio = 1;
            }
        } else {
            if (offset > p->video_size - p->buffer_size)
                return AVERROR_INVALIDDATA;

            avio_read(pb, p->video_frame + offset, p->buffer_size);
        }
        p->current_frame_block++;
    }

    size = p->video_size - p->frames_offset_table[p->current_frame];
    if (size < 1)
        return AVERROR_INVALIDDATA;

    if (av_new_packet(pkt, size) < 0)
        return AVERROR(ENOMEM);

    pkt->stream_index = 0;
    pkt->duration     = 1;
    memcpy(pkt->data, p->video_frame + p->frames_offset_table[p->current_frame], size);
    if (pkt->data[0] & 0x20)
        pkt->flags   |= AV_PKT_FLAG_KEY;
    p->current_frame++;

    return pkt->size;
}
