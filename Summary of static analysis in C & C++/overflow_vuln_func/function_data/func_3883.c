static int rm_read_header(AVFormatContext *s, AVFormatParameters *ap)
{
    RMContext *rm = s->priv_data;
    AVStream *st;
    ByteIOContext *pb = &s->pb;
    unsigned int tag, v;
    int tag_size, size, codec_data_size, i;
    int64_t codec_pos;
    unsigned int h263_hack_version, start_time, duration;
    char buf[128];
    int flags = 0;

    tag = get_le32(pb);
    if (tag == MKTAG('.', 'r', 'a', 0xfd)) {
        /
        return rm_read_header_old(s, ap);
    } else if (tag != MKTAG('.', 'R', 'M', 'F')) {
        return AVERROR_IO;
    }

    get_be32(pb); /
    get_be16(pb);
    get_be32(pb);
    get_be32(pb); /

    for(;;) {
        if (url_feof(pb))
            goto fail;
        tag = get_le32(pb);
        tag_size = get_be32(pb);
        get_be16(pb);
#if 0
        printf("tag=%c%c%c%c (%08x) size=%d\n",
               (tag) & 0xff,
               (tag >> 8) & 0xff,
               (tag >> 16) & 0xff,
               (tag >> 24) & 0xff,
               tag,
               tag_size);
#endif
        if (tag_size < 10 && tag != MKTAG('D', 'A', 'T', 'A'))
            goto fail;
        switch(tag) {
        case MKTAG('P', 'R', 'O', 'P'):
            /
            get_be32(pb); /
            get_be32(pb); /
            get_be32(pb); /
            get_be32(pb); /
            get_be32(pb); /
            get_be32(pb); /
            get_be32(pb); /
            get_be32(pb); /
            get_be32(pb); /
            get_be16(pb); /
            flags = get_be16(pb); /
            break;
        case MKTAG('C', 'O', 'N', 'T'):
            get_str(pb, s->title, sizeof(s->title));
            get_str(pb, s->author, sizeof(s->author));
            get_str(pb, s->copyright, sizeof(s->copyright));
            get_str(pb, s->comment, sizeof(s->comment));
            break;
        case MKTAG('M', 'D', 'P', 'R'):
            st = av_new_stream(s, 0);
            if (!st)
                goto fail;
            st->id = get_be16(pb);
            get_be32(pb); /
            st->codec->bit_rate = get_be32(pb); /
            get_be32(pb); /
            get_be32(pb); /
            start_time = get_be32(pb); /
            get_be32(pb); /
            duration = get_be32(pb); /
            st->start_time = start_time;
            st->duration = duration;
            get_str8(pb, buf, sizeof(buf)); /
            get_str8(pb, buf, sizeof(buf)); /
            codec_data_size = get_be32(pb);
            codec_pos = url_ftell(pb);
            st->codec->codec_type = CODEC_TYPE_DATA;
            av_set_pts_info(st, 64, 1, 1000);

            v = get_be32(pb);
            if (v == MKTAG(0xfd, 'a', 'r', '.')) {
                /
                rm_read_audio_stream_info(s, st, 0);
            } else {
                int fps, fps2;
                if (get_le32(pb) != MKTAG('V', 'I', 'D', 'O')) {
                fail1:
                    av_log(st->codec, AV_LOG_ERROR, "Unsupported video codec\n");
                    goto skip;
                }
                st->codec->codec_tag = get_le32(pb);
//                av_log(NULL, AV_LOG_DEBUG, "%X %X\n", st->codec->codec_tag, MKTAG('R', 'V', '2', '0'));
                if (   st->codec->codec_tag != MKTAG('R', 'V', '1', '0')
                    && st->codec->codec_tag != MKTAG('R', 'V', '2', '0')
                    && st->codec->codec_tag != MKTAG('R', 'V', '3', '0')
                    && st->codec->codec_tag != MKTAG('R', 'V', '4', '0'))
                    goto fail1;
                st->codec->width = get_be16(pb);
                st->codec->height = get_be16(pb);
                st->codec->time_base.num= 1;
                fps= get_be16(pb);
                st->codec->codec_type = CODEC_TYPE_VIDEO;
                get_be32(pb);
                fps2= get_be16(pb);
                get_be16(pb);

                st->codec->extradata_size= codec_data_size - (url_ftell(pb) - codec_pos);

                if(st->codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE <= (unsigned)st->codec->extradata_size){
                    //check is redundant as get_buffer() will catch this
                    av_log(s, AV_LOG_ERROR, "st->codec->extradata_size too large\n");
                    return -1;
                }
                st->codec->extradata= av_mallocz(st->codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
                get_buffer(pb, st->codec->extradata, st->codec->extradata_size);

//                av_log(NULL, AV_LOG_DEBUG, "fps= %d fps2= %d\n", fps, fps2);
                st->codec->time_base.den = fps * st->codec->time_base.num;
                /
#ifdef WORDS_BIGENDIAN
                h263_hack_version = ((uint32_t*)st->codec->extradata)[1];
#else
                h263_hack_version = bswap_32(((uint32_t*)st->codec->extradata)[1]);
#endif
                st->codec->sub_id = h263_hack_version;
                switch((h263_hack_version>>28)){
                case 1: st->codec->codec_id = CODEC_ID_RV10; break;
                case 2: st->codec->codec_id = CODEC_ID_RV20; break;
                case 3: st->codec->codec_id = CODEC_ID_RV30; break;
                case 4: st->codec->codec_id = CODEC_ID_RV40; break;
                default: goto fail1;
                }
            }
skip:
            /
            size = url_ftell(pb) - codec_pos;
            url_fskip(pb, codec_data_size - size);
            break;
        case MKTAG('D', 'A', 'T', 'A'):
            goto header_end;
        default:
            /
            url_fskip(pb, tag_size - 10);
            break;
        }
    }
 header_end:
    rm->nb_packets = get_be32(pb); /
    if (!rm->nb_packets && (flags & 4))
        rm->nb_packets = 3600 * 25;
    get_be32(pb); /
    return 0;

 fail:
    for(i=0;i<s->nb_streams;i++) {
        av_free(s->streams[i]);
    }
    return AVERROR_IO;
}
