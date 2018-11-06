static int vmd_read_header(AVFormatContext *s,
                           AVFormatParameters *ap)
{
    VmdDemuxContext *vmd = s->priv_data;
    ByteIOContext *pb = s->pb;
    AVStream *st = NULL, *vst;
    unsigned int toc_offset;
    unsigned char *raw_frame_table;
    int raw_frame_table_size;
    int64_t current_offset;
    int i, j;
    unsigned int total_frames;
    int64_t current_audio_pts = 0;
    unsigned char chunk[BYTES_PER_FRAME_RECORD];
    int num, den;
    int sound_buffers;

    /
    url_fseek(pb, 0, SEEK_SET);
    if (get_buffer(pb, vmd->vmd_header, VMD_HEADER_SIZE) != VMD_HEADER_SIZE)
        return AVERROR(EIO);

    if(vmd->vmd_header[16] == 'i' && vmd->vmd_header[17] == 'v' && vmd->vmd_header[18] == '3')
        vmd->is_indeo3 = 1;
    else
        vmd->is_indeo3 = 0;
    /
    vst = av_new_stream(s, 0);
    if (!vst)
        return AVERROR(ENOMEM);
    av_set_pts_info(vst, 33, 1, 10);
    vmd->video_stream_index = vst->index;
    vst->codec->codec_type = CODEC_TYPE_VIDEO;
    vst->codec->codec_id = vmd->is_indeo3 ? CODEC_ID_INDEO3 : CODEC_ID_VMDVIDEO;
    vst->codec->codec_tag = 0;  /
    vst->codec->width = AV_RL16(&vmd->vmd_header[12]);
    vst->codec->height = AV_RL16(&vmd->vmd_header[14]);
    if(vmd->is_indeo3 && vst->codec->width > 320){
        vst->codec->width >>= 1;
        vst->codec->height >>= 1;
    }
    vst->codec->extradata_size = VMD_HEADER_SIZE;
    vst->codec->extradata = av_mallocz(VMD_HEADER_SIZE + FF_INPUT_BUFFER_PADDING_SIZE);
    memcpy(vst->codec->extradata, vmd->vmd_header, VMD_HEADER_SIZE);

    /
    vmd->sample_rate = AV_RL16(&vmd->vmd_header[804]);
    if (vmd->sample_rate) {
        st = av_new_stream(s, 0);
        if (!st)
            return AVERROR(ENOMEM);
        vmd->audio_stream_index = st->index;
        st->codec->codec_type = CODEC_TYPE_AUDIO;
        st->codec->codec_id = CODEC_ID_VMDAUDIO;
        st->codec->codec_tag = 0;  /
        st->codec->channels = (vmd->vmd_header[811] & 0x80) ? 2 : 1;
        st->codec->sample_rate = vmd->sample_rate;
        st->codec->block_align = AV_RL16(&vmd->vmd_header[806]);
        if (st->codec->block_align & 0x8000) {
            st->codec->bits_per_coded_sample = 16;
            st->codec->block_align = -(st->codec->block_align - 0x10000);
        } else {
            st->codec->bits_per_coded_sample = 8;
        }
        st->codec->bit_rate = st->codec->sample_rate *
            st->codec->bits_per_coded_sample * st->codec->channels;

        /
        num = st->codec->block_align;
        den = st->codec->sample_rate * st->codec->channels;
        av_reduce(&den, &num, den, num, (1UL<<31)-1);
        av_set_pts_info(vst, 33, num, den);
        av_set_pts_info(st, 33, num, den);
    }

    toc_offset = AV_RL32(&vmd->vmd_header[812]);
    vmd->frame_count = AV_RL16(&vmd->vmd_header[6]);
    vmd->frames_per_block = AV_RL16(&vmd->vmd_header[18]);
    url_fseek(pb, toc_offset, SEEK_SET);

    raw_frame_table = NULL;
    vmd->frame_table = NULL;
    sound_buffers = AV_RL16(&vmd->vmd_header[808]);
    raw_frame_table_size = vmd->frame_count * 6;
    if(vmd->frame_count * vmd->frames_per_block  >= UINT_MAX / sizeof(vmd_frame)){
        av_log(s, AV_LOG_ERROR, "vmd->frame_count * vmd->frames_per_block too large\n");
        return -1;
    }
    raw_frame_table = av_malloc(raw_frame_table_size);
    vmd->frame_table = av_malloc((vmd->frame_count * vmd->frames_per_block + sound_buffers) * sizeof(vmd_frame));
    if (!raw_frame_table || !vmd->frame_table) {
        av_free(raw_frame_table);
        av_free(vmd->frame_table);
        return AVERROR(ENOMEM);
    }
    if (get_buffer(pb, raw_frame_table, raw_frame_table_size) !=
        raw_frame_table_size) {
        av_free(raw_frame_table);
        av_free(vmd->frame_table);
        return AVERROR(EIO);
    }

    total_frames = 0;
    for (i = 0; i < vmd->frame_count; i++) {

        current_offset = AV_RL32(&raw_frame_table[6 * i + 2]);

        /
        for (j = 0; j < vmd->frames_per_block; j++) {
            int type;
            uint32_t size;

            get_buffer(pb, chunk, BYTES_PER_FRAME_RECORD);
            type = chunk[0];
            size = AV_RL32(&chunk[2]);
            if(!size && type != 1)
                continue;
            switch(type) {
            case 1: /
                if (!st) break;
                /
                vmd->frame_table[total_frames].frame_offset = current_offset;
                vmd->frame_table[total_frames].stream_index = vmd->audio_stream_index;
                vmd->frame_table[total_frames].frame_size = size;
                memcpy(vmd->frame_table[total_frames].frame_record, chunk, BYTES_PER_FRAME_RECORD);
                vmd->frame_table[total_frames].pts = current_audio_pts;
                total_frames++;
                if(!current_audio_pts)
                    current_audio_pts += sound_buffers;
                else
                    current_audio_pts++;
                break;
            case 2: /
                vmd->frame_table[total_frames].frame_offset = current_offset;
                vmd->frame_table[total_frames].stream_index = vmd->video_stream_index;
                vmd->frame_table[total_frames].frame_size = size;
                memcpy(vmd->frame_table[total_frames].frame_record, chunk, BYTES_PER_FRAME_RECORD);
                vmd->frame_table[total_frames].pts = i;
                total_frames++;
                break;
            }
            current_offset += size;
        }
    }

    av_free(raw_frame_table);

    vmd->current_frame = 0;
    vmd->frame_count = total_frames;

    return 0;
}
