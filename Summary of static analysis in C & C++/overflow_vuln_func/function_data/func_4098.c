static int mov_parse_stsd_data(MOVContext *c, AVIOContext *pb,
                                AVStream *st, MOVStreamContext *sc,
                                int size)
{
    if (st->codec->codec_tag == MKTAG('t','m','c','d')) {
        if (ff_get_extradata(st->codec, pb, size) < 0)
            return AVERROR(ENOMEM);
        if (size > 16) {
            MOVStreamContext *tmcd_ctx = st->priv_data;
            int val;
            val = AV_RB32(st->codec->extradata + 4);
            tmcd_ctx->tmcd_flags = val;
            if (val & 1)
                st->codec->flags2 |= CODEC_FLAG2_DROP_FRAME_TIMECODE;
            st->codec->time_base.den = st->codec->extradata[16]; /
            st->codec->time_base.num = 1;
            if (size > 30) {
                uint32_t len = AV_RB32(st->codec->extradata + 18); /
                uint32_t format = AV_RB32(st->codec->extradata + 22);
                if (format == AV_RB32("name") && (int64_t)size >= (int64_t)len + 18) {
                    uint16_t str_size = AV_RB16(st->codec->extradata + 26); /
                    if (str_size > 0 && size >= (int)str_size + 26) {
                        char *reel_name = av_malloc(str_size + 1);
                        if (!reel_name)
                            return AVERROR(ENOMEM);
                        memcpy(reel_name, st->codec->extradata + 30, str_size);
                        reel_name[str_size] = 0; /
                        /
                        if (*reel_name == 0) {
                            av_free(reel_name);
                        } else {
                            av_dict_set(&st->metadata, "reel_name", reel_name,  AV_DICT_DONT_STRDUP_VAL);
                        }
                    }
                }
            }
        }
    } else {
        /
        avio_skip(pb, size);
    }
    return 0;
}
