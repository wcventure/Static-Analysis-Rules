static int pmp_header(AVFormatContext *s)
{
    PMPContext *pmp = s->priv_data;
    AVIOContext *pb = s->pb;
    int tb_num, tb_den;
    uint32_t index_cnt;
    int audio_codec_id = AV_CODEC_ID_NONE;
    int srate, channels;
    int i;
    uint64_t pos;
    int64_t fsize = avio_size(pb);

    AVStream *vst = avformat_new_stream(s, NULL);
    if (!vst)
        return AVERROR(ENOMEM);
    vst->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    avio_skip(pb, 8);
    switch (avio_rl32(pb)) {
    case 0:
        vst->codec->codec_id = AV_CODEC_ID_MPEG4;
        break;
    case 1:
        vst->codec->codec_id = AV_CODEC_ID_H264;
        break;
    default:
        av_log(s, AV_LOG_ERROR, "Unsupported video format\n");
        break;
    }
    index_cnt = avio_rl32(pb);
    vst->codec->width  = avio_rl32(pb);
    vst->codec->height = avio_rl32(pb);

    tb_num = avio_rl32(pb);
    tb_den = avio_rl32(pb);
    avpriv_set_pts_info(vst, 32, tb_num, tb_den);
    vst->nb_frames = index_cnt;
    vst->duration = index_cnt;

    switch (avio_rl32(pb)) {
    case 0:
        audio_codec_id = AV_CODEC_ID_MP3;
        break;
    case 1:
        av_log(s, AV_LOG_ERROR, "AAC not yet correctly supported\n");
        audio_codec_id = AV_CODEC_ID_AAC;
        break;
    default:
        av_log(s, AV_LOG_ERROR, "Unsupported audio format\n");
        break;
    }
    pmp->num_streams = avio_rl16(pb) + 1;
    avio_skip(pb, 10);
    srate = avio_rl32(pb);
    channels = avio_rl32(pb) + 1;
    pos = avio_tell(pb) + 4*index_cnt;
    for (i = 0; i < index_cnt; i++) {
        uint32_t size = avio_rl32(pb);
        int flags = size & 1 ? AVINDEX_KEYFRAME : 0;
        if (url_feof(pb)) {
            av_log(s, AV_LOG_FATAL, "Encountered EOF while reading index.\n");
            return AVERROR_INVALIDDATA;
        }
        size >>= 1;
        if (size < 9 + 4*pmp->num_streams) {
            av_log(s, AV_LOG_ERROR, "Packet too small\n");
            return AVERROR_INVALIDDATA;
        }
        av_add_index_entry(vst, pos, i, size, 0, flags);
        pos += size;
        if (fsize > 0 && i == 0 && pos > fsize) {
            av_log(s, AV_LOG_ERROR, "File ends before first packet\n");
            return AVERROR_INVALIDDATA;
        }
    }
    for (i = 1; i < pmp->num_streams; i++) {
        AVStream *ast = avformat_new_stream(s, NULL);
        if (!ast)
            return AVERROR(ENOMEM);
        ast->codec->codec_type = AVMEDIA_TYPE_AUDIO;
        ast->codec->codec_id = audio_codec_id;
        ast->codec->channels = channels;
        ast->codec->sample_rate = srate;
        avpriv_set_pts_info(ast, 32, 1, srate);
    }
    return 0;
}
