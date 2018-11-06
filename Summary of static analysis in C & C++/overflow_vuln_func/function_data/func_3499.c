static int mxf_set_audio_pts(MXFContext *mxf, AVCodecContext *codec, AVPacket *pkt)
{
    MXFTrack *track = mxf->fc->streams[pkt->stream_index]->priv_data;
    pkt->pts = track->sample_count;
    if (codec->channels <= 0 || av_get_bits_per_sample(codec->codec_id) <= 0)
        return AVERROR(EINVAL);
    track->sample_count += pkt->size / (codec->channels * av_get_bits_per_sample(codec->codec_id) / 8);
    return 0;
}
