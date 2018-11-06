static int bfi_read_header(AVFormatContext * s)
{
    BFIContext *bfi = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *vstream;
    AVStream *astream;
    int fps, chunk_header;

    /
    vstream = avformat_new_stream(s, NULL);
    if (!vstream)
        return AVERROR(ENOMEM);

    /
    astream = avformat_new_stream(s, NULL);
    if (!astream)
        return AVERROR(ENOMEM);

    /
    avio_skip(pb, 8);
    chunk_header           = avio_rl32(pb);
    bfi->nframes           = avio_rl32(pb);
    avio_rl32(pb);
    avio_rl32(pb);
    avio_rl32(pb);
    fps                    = avio_rl32(pb);
    avio_skip(pb, 12);
    vstream->codecpar->width  = avio_rl32(pb);
    vstream->codecpar->height = avio_rl32(pb);

    /
    avio_skip(pb, 8);
    vstream->codecpar->extradata      = av_malloc(768);
    if (!vstream->codecpar->extradata)
        return AVERROR(ENOMEM);
    vstream->codecpar->extradata_size = 768;
    avio_read(pb, vstream->codecpar->extradata,
               vstream->codecpar->extradata_size);

    astream->codecpar->sample_rate = avio_rl32(pb);
    if (astream->codecpar->sample_rate <= 0) {
        av_log(s, AV_LOG_ERROR, "Invalid sample rate %d\n", astream->codecpar->sample_rate);
        return AVERROR_INVALIDDATA;
    }

    /
    avpriv_set_pts_info(vstream, 32, 1, fps);
    vstream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    vstream->codecpar->codec_id   = AV_CODEC_ID_BFI;
    vstream->codecpar->format     = AV_PIX_FMT_PAL8;
    vstream->nb_frames            =
    vstream->duration             = bfi->nframes;

    /
    astream->codecpar->codec_type      = AVMEDIA_TYPE_AUDIO;
    astream->codecpar->codec_id        = AV_CODEC_ID_PCM_U8;
    astream->codecpar->channels        = 1;
    astream->codecpar->channel_layout  = AV_CH_LAYOUT_MONO;
    astream->codecpar->bits_per_coded_sample = 8;
    astream->codecpar->bit_rate        =
        astream->codecpar->sample_rate * astream->codecpar->bits_per_coded_sample;
    avio_seek(pb, chunk_header - 3, SEEK_SET);
    avpriv_set_pts_info(astream, 64, 1, astream->codecpar->sample_rate);
    return 0;
}
