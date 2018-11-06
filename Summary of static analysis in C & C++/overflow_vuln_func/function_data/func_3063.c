static int apng_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    APNGDemuxContext *ctx = s->priv_data;
    int ret;
    int64_t size;
    AVIOContext *pb = s->pb;
    uint32_t len, tag;

    /
    /
    if (ctx->num_play == 1 && (ret = ffio_ensure_seekback(pb, 46)) < 0)
        return ret;

    len = avio_rb32(pb);
    tag = avio_rl32(pb);
    switch (tag) {
    case MKTAG('f', 'c', 'T', 'L'):
        if (len != 26)
            return AVERROR_INVALIDDATA;

        if ((ret = decode_fctl_chunk(s, ctx, pkt)) < 0)
            return ret;

        /
        len = avio_rb32(pb);
        tag = avio_rl32(pb);
        if (len > 0x7fffffff ||
            tag != MKTAG('f', 'd', 'A', 'T') &&
            tag != MKTAG('I', 'D', 'A', 'T'))
            return AVERROR_INVALIDDATA;

        size = 38 / + 8 / + len + 4 /;
        if (size > INT_MAX)
            return AVERROR(EINVAL);

        if ((ret = avio_seek(pb, -46, SEEK_CUR)) < 0 ||
            (ret = av_append_packet(pb, pkt, size)) < 0)
            return ret;

        if (ctx->num_play == 1 && (ret = ffio_ensure_seekback(pb, 8)) < 0)
            return ret;

        len = avio_rb32(pb);
        tag = avio_rl32(pb);
        while (tag &&
               tag != MKTAG('f', 'c', 'T', 'L') &&
               tag != MKTAG('I', 'E', 'N', 'D')) {
            if (len > 0x7fffffff)
                return AVERROR_INVALIDDATA;
            if ((ret = avio_seek(pb, -8, SEEK_CUR)) < 0 ||
                (ret = av_append_packet(pb, pkt, len + 12)) < 0)
                return ret;
            if (ctx->num_play == 1 && (ret = ffio_ensure_seekback(pb, 8)) < 0)
                return ret;
            len = avio_rb32(pb);
            tag = avio_rl32(pb);
        }
        if ((ret = avio_seek(pb, -8, SEEK_CUR)) < 0)
            return ret;

        if (ctx->is_key_frame)
            pkt->flags |= AV_PKT_FLAG_KEY;
        pkt->pts = ctx->pkt_pts;
        pkt->duration = ctx->pkt_duration;
        ctx->pkt_pts += ctx->pkt_duration;
        return ret;
    case MKTAG('I', 'E', 'N', 'D'):
        ctx->cur_loop++;
        if (ctx->ignore_loop || ctx->num_play >= 1 && ctx->cur_loop == ctx->num_play) {
            avio_seek(pb, -8, SEEK_CUR);
            return AVERROR_EOF;
        }
        if ((ret = avio_seek(pb, s->streams[0]->codec->extradata_size + 8, SEEK_SET)) < 0)
            return ret;
        return 0;
    default:
        {
        char tag_buf[32];

        av_get_codec_tag_string(tag_buf, sizeof(tag_buf), tag);
        avpriv_request_sample(s, "In-stream tag=%s (0x%08X) len=%"PRIu32, tag_buf, tag, len);
        avio_skip(pb, len + 4);
        }
    }

    /
    return AVERROR_PATCHWELCOME;
}
