static int mxf_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    KLVPacket klv;

    while (!s->pb->eof_reached) {
        int ret;
        if (klv_read_packet(&klv, s->pb) < 0)
            return -1;
        PRINT_KEY(s, "read packet", klv.key);
        av_dlog(s, "size %"PRIu64" offset %#"PRIx64"\n", klv.length, klv.offset);
        if (IS_KLV_KEY(klv.key, mxf_encrypted_triplet_key)) {
            ret = mxf_decrypt_triplet(s, pkt, &klv);
            if (ret < 0) {
                av_log(s, AV_LOG_ERROR, "invalid encoded triplet\n");
                return AVERROR_INVALIDDATA;
            }
            return 0;
        }
        if (IS_KLV_KEY(klv.key, mxf_essence_element_key) ||
            IS_KLV_KEY(klv.key, mxf_avid_essence_element_key)) {
            int index = mxf_get_stream_index(s, &klv);
            if (index < 0) {
                av_log(s, AV_LOG_ERROR, "error getting stream index %d\n", AV_RB32(klv.key+12));
                goto skip;
            }
            if (s->streams[index]->discard == AVDISCARD_ALL)
                goto skip;
            /
            if (klv.key[12] == 0x06 && klv.key[13] == 0x01 && klv.key[14] == 0x10) {
                if (mxf_get_d10_aes3_packet(s->pb, s->streams[index], pkt, klv.length) < 0) {
                    av_log(s, AV_LOG_ERROR, "error reading D-10 aes3 frame\n");
                    return AVERROR_INVALIDDATA;
                }
            } else {
                ret = av_get_packet(s->pb, pkt, klv.length);
                if (ret < 0)
                    return ret;
            }
            pkt->stream_index = index;
            pkt->pos = klv.offset;
            return 0;
        } else
        skip:
            avio_skip(s->pb, klv.length);
    }
    return AVERROR_EOF;
}
