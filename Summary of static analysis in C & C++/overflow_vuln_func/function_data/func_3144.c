static int adx_read_header(AVFormatContext *s)
{
    ADXDemuxerContext *c = s->priv_data;
    AVCodecParameters *par;

    AVStream *st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);
    par = s->streams[0]->codecpar;

    if (avio_rb16(s->pb) != 0x8000)
        return AVERROR_INVALIDDATA;
    c->header_size = avio_rb16(s->pb) + 4;
    avio_seek(s->pb, -4, SEEK_CUR);

    if (ff_get_extradata(s, par, s->pb, c->header_size) < 0)
        return AVERROR(ENOMEM);

    if (par->extradata_size < 12) {
        av_log(s, AV_LOG_ERROR, "Invalid extradata size.\n");
        return AVERROR_INVALIDDATA;
    }
    par->channels    = AV_RB8 (par->extradata + 7);
    par->sample_rate = AV_RB32(par->extradata + 8);

    if (par->channels <= 0) {
        av_log(s, AV_LOG_ERROR, "invalid number of channels %d\n", par->channels);
        return AVERROR_INVALIDDATA;
    }

    if (par->sample_rate <= 0) {
        av_log(s, AV_LOG_ERROR, "Invalid sample rate %d\n", par->sample_rate);
        return AVERROR_INVALIDDATA;
    }

    par->codec_type  = AVMEDIA_TYPE_AUDIO;
    par->codec_id    = s->iformat->raw_codec_id;
    par->bit_rate    = par->sample_rate * par->channels * BLOCK_SIZE * 8LL / BLOCK_SAMPLES;

    avpriv_set_pts_info(st, 64, BLOCK_SAMPLES, par->sample_rate);

    return 0;
}
