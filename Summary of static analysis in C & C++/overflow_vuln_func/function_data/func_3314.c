static int latm_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    AVIOContext *pb = s->pb;
    PutBitContext bs;
    int i, len;
    uint8_t loas_header[] = "\x56\xe0\x00";
    uint8_t *buf;

    if (s->streams[0]->codec->codec_id == CODEC_ID_AAC_LATM)
        return ff_raw_write_packet(s, pkt);

    if (pkt->size > 2 && pkt->data[0] == 0xff && (pkt->data[1] >> 4) == 0xf) {
        av_log(s, AV_LOG_ERROR, "ADTS header detected - ADTS will not be incorrectly muxed into LATM\n");
        return AVERROR_INVALIDDATA;
    }

    buf = av_malloc(pkt->size+1024);
    if (!buf)
        return AVERROR(ENOMEM);

    init_put_bits(&bs, buf, pkt->size+1024);

    latm_write_frame_header(s, &bs);

    /
    for (i = 0; i <= pkt->size-255; i+=255)
        put_bits(&bs, 8, 255);

    put_bits(&bs, 8, pkt->size-i);

    /

    /
    for (i = 0; i < pkt->size; i++)
        put_bits(&bs, 8, pkt->data[i]);

    avpriv_align_put_bits(&bs);
    flush_put_bits(&bs);

    len = put_bits_count(&bs) >> 3;

    loas_header[1] |= (len >> 8) & 0x1f;
    loas_header[2] |= len & 0xff;

    avio_write(pb, loas_header, 3);
    avio_write(pb, buf, len);

    av_free(buf);

    return 0;
}
