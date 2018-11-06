static int latm_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    AVIOContext *pb = s->pb;
    PutBitContext bs;
    int i, len;
    uint8_t loas_header[] = "\x56\xe0\x00";
    uint8_t *buf = NULL;

    if (s->streams[0]->codec->codec_id == CODEC_ID_AAC_LATM)
        return ff_raw_write_packet(s, pkt);

    if (pkt->size > 2 && pkt->data[0] == 0xff && (pkt->data[1] >> 4) == 0xf) {
        av_log(s, AV_LOG_ERROR, "ADTS header detected - ADTS will not be incorrectly muxed into LATM\n");
        return AVERROR_INVALIDDATA;
    }
    if (pkt->size > 0x1fff)
        goto too_large;

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
    if (pkt->size && (pkt->data[0] & 0xe1) == 0x81) {
        // Convert byte-aligned DSE to non-aligned.
        // Due to the input format encoding we know that
        // it is naturally byte-aligned in the input stream,
        // so there are no padding bits to account for.
        // To avoid having to add padding bits and rearrange
        // the whole stream we just remove the byte-align flag.
        // This allows us to remux our FATE AAC samples into latm
        // files that are still playable with minimal effort.
        put_bits(&bs, 8, pkt->data[0] & 0xfe);
        avpriv_copy_bits(&bs, pkt->data + 1, 8*pkt->size - 8);
    } else
        avpriv_copy_bits(&bs, pkt->data, 8*pkt->size);

    avpriv_align_put_bits(&bs);
    flush_put_bits(&bs);

    len = put_bits_count(&bs) >> 3;

    if (len > 0x1fff)
        goto too_large;

    loas_header[1] |= (len >> 8) & 0x1f;
    loas_header[2] |= len & 0xff;

    avio_write(pb, loas_header, 3);
    avio_write(pb, buf, len);

    av_free(buf);

    return 0;

too_large:
    av_log(s, AV_LOG_ERROR, "LATM packet size larger than maximum size 0x1fff\n");
    av_free(buf);
    return AVERROR_INVALIDDATA;
}
