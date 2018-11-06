static int vmd_read_header(AVFormatContext *s,
                           AVFormatParameters *ap)
{
    VmdDemuxContext *vmd = (VmdDemuxContext *)s->priv_data;
    ByteIOContext *pb = &s->pb;
    AVStream *st, *vst;
    unsigned int toc_offset;
    unsigned char *raw_frame_table;
    int raw_frame_table_size;
    offset_t current_offset;
    int i, j;
    unsigned int total_frames;
    int64_t pts_inc = 1;
    int64_t current_video_pts = 0, current_audio_pts = 0;
    unsigned char chunk[BYTES_PER_FRAME_RECORD];
    int num, den;

    /
    url_fseek(pb, 0, SEEK_SET);
    if (get_buffer(pb, vmd->vmd_header, VMD_HEADER_SIZE) != VMD_HEADER_SIZE)
        return AVERROR_IO;

    /
    vst = av_new_stream(s, 0);
    if (!vst)
        return AVERROR_NOMEM;
    av_set_pts_info(vst, 33, 1, 10);
    vmd->video_stream_index = vst->index;
    vst->codec->codec_type = CODEC_TYPE_VIDEO;
    vst->codec->codec_id = CODEC_ID_VMDVIDEO;
    vst->codec->codec_tag = 0;  /
    vst->codec->width = LE_16(&vmd->vmd_header[12]);
    vst->codec->height = LE_16(&vmd->vmd_header[14]);
    vst->codec->extradata_size = VMD_HEADER_SIZE;
    vst->codec->extradata = av_mallocz(VMD_HEADER_SIZE + FF_INPUT_BUFFER_PADDING_SIZE);
    memcpy(vst->codec->extradata, vmd->vmd_header, VMD_HEADER_SIZE);

    /
    vmd->sample_rate = LE_16(&vmd->vmd_header[804]);
    if (vmd->sample_rate) {
        st = av_new_stream(s, 0);
        if (!st)
            return AVERROR_NOMEM;
        vmd->audio_stream_index = st->index;
        st->codec->codec_type = CODEC_TYPE_AUDIO;
        st->codec->codec_id = CODEC_ID_VMDAUDIO;
        st->codec->codec_tag = 0;  /
        st->codec->channels = (vmd->vmd_header[811] & 0x80) ? 2 : 1;
        st->codec->sample_rate = vmd->sample_rate;
        st->codec->block_align = LE_16(&vmd->vmd_header[806]);
        if (st->codec->block_align & 0x8000) {
            st->codec->bits_per_sample = 16;
            st->codec->block_align = -(st->codec->block_align - 0x10000);
        } else {
            st->codec->bits_per_sample = 8;
        }
        st->codec->bit_rate = st->codec->sample_rate *
            st->codec->bits_per_sample * st->codec->channels;

        /
        num = st->codec->block_align;
        den = st->codec->sample_rate * st->codec->channels;
        av_reduce(&den, &num, den, num, (1UL<<31)-1);
        av_set_pts_info(vst, 33, num, den);
        av_set_pts_info(st, 33, num, den);
        pts_inc = num;
    }

    toc_offset = LE_32(&vmd->vmd_header[812]);
    vmd->frame_count = LE_16(&vmd->vmd_header[6]);
    vmd->frames_per_block = LE_16(&vmd->vmd_header[18]);
    url_fseek(pb, toc_offset, SEEK_SET);

    raw_frame_table = NULL;
    vmd->frame_table = NULL;
    raw_frame_table_size = vmd->frame_count * 6;
    raw_frame_table = av_malloc(raw_frame_table_size);
    if(vmd->frame_count * vmd->frames_per_block  >= UINT_MAX / sizeof(vmd_frame_t)){
        av_log(s, AV_LOG_ERROR, "vmd->frame_count * vmd->frames_per_block too large\n");
        return -1;
    }
    vmd->frame_table = av_malloc(vmd->frame_count * vmd->frames_per_block * sizeof(vmd_frame_t));
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

    total_frames = 0;
    for (i = 0; i < vmd->frame_count; i++) {

        current_offset = LE_32(&raw_frame_table[6 * i + 2]);

        /
        for (j = 0; j < vmd->frames_per_block; j++) {
            int type;
            uint32_t size;

            get_buffer(pb, chunk, BYTES_PER_FRAME_RECORD);
            type = chunk[0];
            size = LE_32(&chunk[2]);
            if(!size)
                continue;
            switch(type) {
            case 1: /
                vmd->frame_table[total_frames].frame_offset = current_offset;
                vmd->frame_table[total_frames].stream_index = vmd->audio_stream_index;
                vmd->frame_table[total_frames].frame_size = size;
                memcpy(vmd->frame_table[total_frames].frame_record, chunk, BYTES_PER_FRAME_RECORD);
                vmd->frame_table[total_frames].pts = current_audio_pts;
                total_frames++;
                /
                current_audio_pts += (current_audio_pts == 0) ? LE_16(&vmd->vmd_header[808]) * pts_inc : pts_inc;
                break;
            case 2: /
                vmd->frame_table[total_frames].frame_offset = current_offset;
                vmd->frame_table[total_frames].stream_index = vmd->video_stream_index;
                vmd->frame_table[total_frames].frame_size = size;
                memcpy(vmd->frame_table[total_frames].frame_record, chunk, BYTES_PER_FRAME_RECORD);
                vmd->frame_table[total_frames].pts = current_video_pts;
                total_frames++;
                break;
            }
            current_offset += size;
        }
        current_video_pts += pts_inc;
    }

    av_free(raw_frame_table);

    vmd->current_frame = 0;
    vmd->frame_count = total_frames;

    return 0;
}
