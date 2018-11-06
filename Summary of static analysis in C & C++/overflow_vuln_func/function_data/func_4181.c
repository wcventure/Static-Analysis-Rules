static int xmv_read_header(AVFormatContext *s)
{
    XMVDemuxContext *xmv = s->priv_data;
    AVIOContext     *pb  = s->pb;
    AVStream        *vst = NULL;

    uint32_t file_version;
    uint32_t this_packet_size;
    uint16_t audio_track;
    int ret;

    avio_skip(pb, 4); /

    this_packet_size = avio_rl32(pb);

    avio_skip(pb, 4); /
    avio_skip(pb, 4); /

    file_version = avio_rl32(pb);
    if ((file_version != 4) && (file_version != 2))
        avpriv_request_sample(s, "Uncommon version %d", file_version);


    /

    vst = avformat_new_stream(s, NULL);
    if (!vst)
        return AVERROR(ENOMEM);

    avpriv_set_pts_info(vst, 32, 1, 1000);

    vst->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    vst->codec->codec_id   = AV_CODEC_ID_WMV2;
    vst->codec->codec_tag  = MKBETAG('W', 'M', 'V', '2');
    vst->codec->width      = avio_rl32(pb);
    vst->codec->height     = avio_rl32(pb);

    vst->duration          = avio_rl32(pb);

    xmv->video.stream_index = vst->index;

    /

    xmv->audio_track_count = avio_rl16(pb);

    avio_skip(pb, 2); /

    xmv->audio_tracks = av_malloc(xmv->audio_track_count * sizeof(XMVAudioTrack));
    if (!xmv->audio_tracks)
        return AVERROR(ENOMEM);

    xmv->audio = av_malloc(xmv->audio_track_count * sizeof(XMVAudioPacket));
    if (!xmv->audio) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    for (audio_track = 0; audio_track < xmv->audio_track_count; audio_track++) {
        XMVAudioTrack  *track  = &xmv->audio_tracks[audio_track];
        XMVAudioPacket *packet = &xmv->audio       [audio_track];
        AVStream *ast = NULL;

        track->compression     = avio_rl16(pb);
        track->channels        = avio_rl16(pb);
        track->sample_rate     = avio_rl32(pb);
        track->bits_per_sample = avio_rl16(pb);
        track->flags           = avio_rl16(pb);

        track->bit_rate      = track->bits_per_sample *
                               track->sample_rate *
                               track->channels;
        track->block_align   = 36 * track->channels;
        track->block_samples = 64;
        track->codec_id      = ff_wav_codec_get_id(track->compression,
                                                   track->bits_per_sample);

        packet->track        = track;
        packet->stream_index = -1;

        packet->frame_size  = 0;
        packet->block_count = 0;

        /
        if (track->flags & XMV_AUDIO_ADPCM51)
            av_log(s, AV_LOG_WARNING, "Unsupported 5.1 ADPCM audio stream "
                                      "(0x%04X)\n", track->flags);

        if (!track->channels || !track->sample_rate) {
            av_log(s, AV_LOG_ERROR, "Invalid parameters for audio track %d.\n",
                   audio_track);
            ret = AVERROR_INVALIDDATA;
            goto fail;
        }

        ast = avformat_new_stream(s, NULL);
        if (!ast) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }

        ast->codec->codec_type            = AVMEDIA_TYPE_AUDIO;
        ast->codec->codec_id              = track->codec_id;
        ast->codec->codec_tag             = track->compression;
        ast->codec->channels              = track->channels;
        ast->codec->sample_rate           = track->sample_rate;
        ast->codec->bits_per_coded_sample = track->bits_per_sample;
        ast->codec->bit_rate              = track->bit_rate;
        ast->codec->block_align           = 36 * track->channels;

        avpriv_set_pts_info(ast, 32, track->block_samples, track->sample_rate);

        packet->stream_index = ast->index;

        ast->duration = vst->duration;
    }


    /

    xmv->next_packet_offset = avio_tell(pb);
    xmv->next_packet_size   = this_packet_size - xmv->next_packet_offset;
    xmv->stream_count       = xmv->audio_track_count + 1;

    return 0;

fail:
    xmv_read_close(s);
    return ret;
}
