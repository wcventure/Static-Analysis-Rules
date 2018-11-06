static void extract_mpeg4_header(AVFormatContext *infile)
{
    int mpeg4_count, i, size;
    AVPacket pkt;
    AVStream *st;
    const uint8_t *p;

    mpeg4_count = 0;
    for(i=0;i<infile->nb_streams;i++) {
        st = infile->streams[i];
        if (st->codec.codec_id == CODEC_ID_MPEG4 &&
            st->codec.extradata_size == 0) {
            mpeg4_count++;
        }
    }
    if (!mpeg4_count)
        return;

    printf("MPEG4 without extra data: trying to find header\n");
    while (mpeg4_count > 0) {
        if (av_read_packet(infile, &pkt) < 0)
            break;
        st = infile->streams[pkt.stream_index];
        if (st->codec.codec_id == CODEC_ID_MPEG4 &&
            st->codec.extradata_size == 0) {
            av_freep(&st->codec.extradata);
            /
            /
            p = pkt.data;
            while (p < pkt.data + pkt.size - 4) {
                /
                if (p[0] == 0x00 && p[1] == 0x00 && 
                    p[2] == 0x01 && p[3] == 0xb6) {
                    size = p - pkt.data;
                    //                    av_hex_dump(pkt.data, size);
                    st->codec.extradata = av_malloc(size);
                    st->codec.extradata_size = size;
                    memcpy(st->codec.extradata, pkt.data, size);
                    break;
                }
                p++;
            }
            mpeg4_count--;
        }
        av_free_packet(&pkt);
    }
}
