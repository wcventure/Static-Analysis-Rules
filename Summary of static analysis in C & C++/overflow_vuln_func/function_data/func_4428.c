static int vmd_read_header(AVFormatContext *s,
                           AVFormatParameters *ap)
{
    VmdDemuxContext *vmd = (VmdDemuxContext *)s->priv_data;
    ByteIOContext *pb = &s->pb;
    AVStream *st;
    unsigned int toc_offset;
    unsigned char *raw_frame_table;
    int raw_frame_table_size;
    unsigned char *current_frame_record;
    offset_t current_offset;
    int i;
    unsigned int total_frames;
    int64_t video_pts_inc;
    int64_t current_video_pts = 0;

    /
    url_fseek(pb, 0, SEEK_SET);
    if (get_buffer(pb, vmd->vmd_header, VMD_HEADER_SIZE) != VMD_HEADER_SIZE)
        return AVERROR_IO;

    vmd->audio_sample_counter = 0;
    vmd->audio_frame_divisor = 1;
    vmd->audio_block_align = 1;

    /
    st = av_new_stream(s, 0);
    if (!st)
        return AVERROR_NOMEM;
    av_set_pts_info(st, 33, 1, 90000);
    vmd->video_stream_index = st->index;
    st->codec.codec_type = CODEC_TYPE_VIDEO;
    st->codec.codec_id = CODEC_ID_VMDVIDEO;
    st->codec.codec_tag = 0;  /
    st->codec.width = LE_16(&vmd->vmd_header[12]);
    st->codec.height = LE_16(&vmd->vmd_header[14]);
    st->codec.extradata_size = VMD_HEADER_SIZE;
    st->codec.extradata = av_malloc(VMD_HEADER_SIZE);
    memcpy(st->codec.extradata, vmd->vmd_header, VMD_HEADER_SIZE);

    /
    vmd->sample_rate = LE_16(&vmd->vmd_header[804]);
    if (vmd->sample_rate) {
        st = av_new_stream(s, 0);
        if (!st)
            return AVERROR_NOMEM;
        av_set_pts_info(st, 33, 1, 90000);
        vmd->audio_stream_index = st->index;
        st->codec.codec_type = CODEC_TYPE_AUDIO;
        st->codec.codec_id = CODEC_ID_VMDAUDIO;
        st->codec.codec_tag = 0;  /
        st->codec.channels = (vmd->vmd_header[811] & 0x80) ? 2 : 1;
        st->codec.sample_rate = vmd->sample_rate;
        st->codec.block_align = vmd->audio_block_align = 
            LE_16(&vmd->vmd_header[806]);
        if (st->codec.block_align & 0x8000) {
            st->codec.bits_per_sample = 16;
            st->codec.block_align = -(st->codec.block_align - 0x10000);
        } else
            st->codec.bits_per_sample = 16;
//            st->codec.bits_per_sample = 8;
        st->codec.bit_rate = st->codec.sample_rate * 
            st->codec.bits_per_sample * st->codec.channels;

        /
        vmd->audio_frame_divisor = st->codec.bits_per_sample / 8 / 
            st->codec.channels;

        video_pts_inc = 90000;
        video_pts_inc *= st->codec.block_align;
        video_pts_inc /= st->codec.sample_rate;
    } else {
        /
        video_pts_inc = 90000 / 10;
    }

    /
    toc_offset = LE_32(&vmd->vmd_header[812]);
    vmd->frame_count = LE_16(&vmd->vmd_header[6]);
    url_fseek(pb, toc_offset + vmd->frame_count * 6, SEEK_SET);

    /
    if(vmd->sample_rate)
	vmd->frame_count *= 2;
    raw_frame_table = NULL;
    vmd->frame_table = NULL;
    raw_frame_table_size = vmd->frame_count * BYTES_PER_FRAME_RECORD;
    raw_frame_table = av_malloc(raw_frame_table_size);
    vmd->frame_table = av_malloc(vmd->frame_count * sizeof(vmd_frame_t));
    if (!raw_frame_table || !vmd->frame_table) {
        av_free(raw_frame_table);
        av_free(vmd->frame_table);
        return AVERROR_NOMEM;
    }
    if (get_buffer(pb, raw_frame_table, raw_frame_table_size) != 
        raw_frame_table_size) {
        av_free(raw_frame_table);
        av_free(vmd->frame_table);
        return AVERROR_IO;
    }

    current_offset = LE_32(&vmd->vmd_header[20]);
    current_frame_record = raw_frame_table;
    total_frames = vmd->frame_count;
    i = 0;
    while (total_frames--) {

        /
        vmd->frame_table[i].frame_size = LE_32(&current_frame_record[2]);

        /
        if (!vmd->frame_table[i].frame_size) {
            vmd->frame_count--;  /
            current_frame_record += BYTES_PER_FRAME_RECORD;
            continue;
        }

        if (current_frame_record[0] == 0x02)
            vmd->frame_table[i].stream_index = vmd->video_stream_index;
        else
            vmd->frame_table[i].stream_index = vmd->audio_stream_index;
        vmd->frame_table[i].frame_offset = current_offset;
        current_offset += vmd->frame_table[i].frame_size;
        memcpy(vmd->frame_table[i].frame_record, current_frame_record,
            BYTES_PER_FRAME_RECORD);

        /
        if (current_frame_record[0] == 0x02) {
            vmd->frame_table[i].pts = current_video_pts;
            current_video_pts += video_pts_inc;
        } else if (current_frame_record[0] == 0x01) {
            /
            vmd->frame_table[i].pts = 0;
        }

        current_frame_record += BYTES_PER_FRAME_RECORD;
        i++;
    }

    av_free(raw_frame_table);

    vmd->current_frame = 0;

    return 0;
}
