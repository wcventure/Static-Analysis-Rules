static int wav_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    int ret, size;
    int64_t left;
    AVStream *st;
    WAVDemuxContext *wav = s->priv_data;

    if (CONFIG_SPDIF_DEMUXER && wav->spdif == 0 &&
        s->streams[0]->codec->codec_tag == 1) {
        enum AVCodecID codec;
        ret = ff_spdif_probe(s->pb->buffer, s->pb->buf_end - s->pb->buffer,
                             &codec);
        if (ret > AVPROBE_SCORE_EXTENSION) {
            s->streams[0]->codec->codec_id = codec;
            wav->spdif = 1;
        } else {
            wav->spdif = -1;
        }
    }
    if (CONFIG_SPDIF_DEMUXER && wav->spdif == 1)
        return ff_spdif_read_packet(s, pkt);

    if (wav->smv_data_ofs > 0) {
        int64_t audio_dts, video_dts;
smv_retry:
        audio_dts = s->streams[0]->cur_dts;
        video_dts = s->streams[1]->cur_dts;

        if (audio_dts != AV_NOPTS_VALUE && video_dts != AV_NOPTS_VALUE) {
            audio_dts = av_rescale_q(audio_dts, s->streams[0]->time_base, AV_TIME_BASE_Q);
            video_dts = av_rescale_q(video_dts, s->streams[1]->time_base, AV_TIME_BASE_Q);
            /
            wav->smv_last_stream = wav->smv_given_first ? video_dts > audio_dts : 0;
            wav->smv_given_first = 1;
        }
        wav->smv_last_stream = !wav->smv_last_stream;
        wav->smv_last_stream |= wav->audio_eof;
        wav->smv_last_stream &= !wav->smv_eof;
        if (wav->smv_last_stream) {
            uint64_t old_pos = avio_tell(s->pb);
            uint64_t new_pos = wav->smv_data_ofs +
                wav->smv_block * wav->smv_block_size;
            if (avio_seek(s->pb, new_pos, SEEK_SET) < 0) {
                ret = AVERROR_EOF;
                goto smv_out;
            }
            size = avio_rl24(s->pb);
            ret  = av_get_packet(s->pb, pkt, size);
            if (ret < 0)
                goto smv_out;
            pkt->pos -= 3;
            pkt->pts = wav->smv_block * wav->smv_frames_per_jpeg + wav->smv_cur_pt;
            wav->smv_cur_pt++;
            if (wav->smv_frames_per_jpeg > 0)
                wav->smv_cur_pt %= wav->smv_frames_per_jpeg;
            if (!wav->smv_cur_pt)
                wav->smv_block++;

            pkt->stream_index = 1;
smv_out:
            avio_seek(s->pb, old_pos, SEEK_SET);
            if (ret == AVERROR_EOF) {
                wav->smv_eof = 1;
                goto smv_retry;
            }
            return ret;
        }
    }

    st = s->streams[0];

    left = wav->data_end - avio_tell(s->pb);
    if (wav->ignore_length)
        left = INT_MAX;
    if (left <= 0) {
        if (CONFIG_W64_DEMUXER && wav->w64)
            left = find_guid(s->pb, ff_w64_guid_data) - 24;
        else
            left = find_tag(s->pb, MKTAG('d', 'a', 't', 'a'));
        if (left < 0) {
            wav->audio_eof = 1;
            if (wav->smv_data_ofs > 0 && !wav->smv_eof)
                goto smv_retry;
            return AVERROR_EOF;
        }
        wav->data_end = avio_tell(s->pb) + left;
    }

    size = MAX_SIZE;
    if (st->codec->block_align > 1) {
        if (size < st->codec->block_align)
            size = st->codec->block_align;
        size = (size / st->codec->block_align) * st->codec->block_align;
    }
    size = FFMIN(size, left);
    ret  = av_get_packet(s->pb, pkt, size);
    if (ret < 0)
        return ret;
    pkt->stream_index = 0;

    return ret;
}
