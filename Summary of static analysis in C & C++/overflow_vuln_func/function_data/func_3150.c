static int iff_read_header(AVFormatContext *s)
{
    IffDemuxContext *iff = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *st;
    uint8_t *buf;
    uint32_t chunk_id;
    uint64_t data_size;
    uint32_t screenmode = 0, num, den;
    unsigned transparency = 0;
    unsigned masking = 0; // no mask
    uint8_t fmt[16];
    int fmt_size;

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->codecpar->channels = 1;
    st->codecpar->channel_layout = AV_CH_LAYOUT_MONO;
    iff->is_64bit = avio_rl32(pb) == ID_FRM8;
    avio_skip(pb, iff->is_64bit ? 8 : 4);
    // codec_tag used by ByteRun1 decoder to distinguish progressive (PBM) and interlaced (ILBM) content
    st->codecpar->codec_tag = avio_rl32(pb);
    if (st->codecpar->codec_tag == ID_ANIM) {
        avio_skip(pb, 12);
    }
    iff->bitmap_compression = -1;
    iff->svx8_compression = -1;
    iff->maud_bits = -1;
    iff->maud_compression = -1;

    while(!avio_feof(pb)) {
        uint64_t orig_pos;
        int res;
        const char *metadata_tag = NULL;
        int version, nb_comments, i;
        chunk_id = avio_rl32(pb);
        data_size = iff->is_64bit ? avio_rb64(pb) : avio_rb32(pb);
        orig_pos = avio_tell(pb);

        switch(chunk_id) {
        case ID_VHDR:
            st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

            if (data_size < 14)
                return AVERROR_INVALIDDATA;
            avio_skip(pb, 12);
            st->codecpar->sample_rate = avio_rb16(pb);
            if (data_size >= 16) {
                avio_skip(pb, 1);
                iff->svx8_compression = avio_r8(pb);
            }
            break;

        case ID_MHDR:
            st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

            if (data_size < 32)
                return AVERROR_INVALIDDATA;
            avio_skip(pb, 4);
            iff->maud_bits = avio_rb16(pb);
            avio_skip(pb, 2);
            num = avio_rb32(pb);
            den = avio_rb16(pb);
            if (!den)
                return AVERROR_INVALIDDATA;
            avio_skip(pb, 2);
            st->codecpar->sample_rate = num / den;
            st->codecpar->channels = avio_rb16(pb);
            iff->maud_compression = avio_rb16(pb);
            if (st->codecpar->channels == 1)
                st->codecpar->channel_layout = AV_CH_LAYOUT_MONO;
            else if (st->codecpar->channels == 2)
                st->codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
            break;

        case ID_ABIT:
        case ID_BODY:
        case ID_DBOD:
        case ID_DSD:
        case ID_DST:
        case ID_MDAT:
            iff->body_pos = avio_tell(pb);
            iff->body_end = iff->body_pos + data_size;
            iff->body_size = data_size;
            if (chunk_id == ID_DST) {
                int ret = read_dst_frame(s, NULL);
                if (ret < 0)
                    return ret;
            }
            break;

        case ID_CHAN:
            if (data_size < 4)
                return AVERROR_INVALIDDATA;
            if (avio_rb32(pb) < 6) {
                st->codecpar->channels       = 1;
                st->codecpar->channel_layout = AV_CH_LAYOUT_MONO;
            } else {
                st->codecpar->channels       = 2;
                st->codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
            }
            break;

        case ID_CAMG:
            if (data_size < 4)
                return AVERROR_INVALIDDATA;
            screenmode                = avio_rb32(pb);
            break;

        case ID_CMAP:
            if (data_size < 3 || data_size > 768 || data_size % 3) {
                 av_log(s, AV_LOG_ERROR, "Invalid CMAP chunk size %"PRIu64"\n",
                        data_size);
                 return AVERROR_INVALIDDATA;
            }
            st->codecpar->extradata_size = data_size + IFF_EXTRA_VIDEO_SIZE;
            st->codecpar->extradata      = av_malloc(data_size + IFF_EXTRA_VIDEO_SIZE + AV_INPUT_BUFFER_PADDING_SIZE);
            if (!st->codecpar->extradata)
                return AVERROR(ENOMEM);
            if (avio_read(pb, st->codecpar->extradata + IFF_EXTRA_VIDEO_SIZE, data_size) < 0)
                return AVERROR(EIO);
            break;

        case ID_BMHD:
            st->codecpar->codec_type            = AVMEDIA_TYPE_VIDEO;
            if (data_size <= 8)
                return AVERROR_INVALIDDATA;
            st->codecpar->width                 = avio_rb16(pb);
            st->codecpar->height                = avio_rb16(pb);
            avio_skip(pb, 4); // x, y offset
            st->codecpar->bits_per_coded_sample = avio_r8(pb);
            if (data_size >= 10)
                masking                      = avio_r8(pb);
            if (data_size >= 11)
                iff->bitmap_compression      = avio_r8(pb);
            if (data_size >= 14) {
                avio_skip(pb, 1); // padding
                transparency                 = avio_rb16(pb);
            }
            if (data_size >= 16) {
                st->sample_aspect_ratio.num  = avio_r8(pb);
                st->sample_aspect_ratio.den  = avio_r8(pb);
            }
            break;

        case ID_ANHD:
            break;

        case ID_DPAN:
            avio_skip(pb, 2);
            st->duration = avio_rb16(pb);
            break;

        case ID_DPEL:
            if (data_size < 4 || (data_size & 3))
                return AVERROR_INVALIDDATA;
            if ((fmt_size = avio_read(pb, fmt, sizeof(fmt))) < 0)
                return fmt_size;
            if (fmt_size == sizeof(deep_rgb24) && !memcmp(fmt, deep_rgb24, sizeof(deep_rgb24)))
                st->codecpar->format = AV_PIX_FMT_RGB24;
            else if (fmt_size == sizeof(deep_rgba) && !memcmp(fmt, deep_rgba, sizeof(deep_rgba)))
                st->codecpar->format = AV_PIX_FMT_RGBA;
            else if (fmt_size == sizeof(deep_bgra) && !memcmp(fmt, deep_bgra, sizeof(deep_bgra)))
                st->codecpar->format = AV_PIX_FMT_BGRA;
            else if (fmt_size == sizeof(deep_argb) && !memcmp(fmt, deep_argb, sizeof(deep_argb)))
                st->codecpar->format = AV_PIX_FMT_ARGB;
            else if (fmt_size == sizeof(deep_abgr) && !memcmp(fmt, deep_abgr, sizeof(deep_abgr)))
                st->codecpar->format = AV_PIX_FMT_ABGR;
            else {
                avpriv_request_sample(s, "color format %.16s", fmt);
                return AVERROR_PATCHWELCOME;
            }
            break;

        case ID_DGBL:
            st->codecpar->codec_type         = AVMEDIA_TYPE_VIDEO;
            if (data_size < 8)
                return AVERROR_INVALIDDATA;
            st->codecpar->width              = avio_rb16(pb);
            st->codecpar->height             = avio_rb16(pb);
            iff->bitmap_compression          = avio_rb16(pb);
            st->sample_aspect_ratio.num      = avio_r8(pb);
            st->sample_aspect_ratio.den      = avio_r8(pb);
            st->codecpar->bits_per_coded_sample = 24;
            break;

        case ID_DLOC:
            if (data_size < 4)
                return AVERROR_INVALIDDATA;
            st->codecpar->width  = avio_rb16(pb);
            st->codecpar->height = avio_rb16(pb);
            break;

        case ID_TVDC:
            if (data_size < sizeof(iff->tvdc))
                return AVERROR_INVALIDDATA;
            res = avio_read(pb, iff->tvdc, sizeof(iff->tvdc));
            if (res < 0)
                return res;
            break;

        case ID_ANNO:
        case ID_TEXT:      metadata_tag = "comment";   break;
        case ID_AUTH:      metadata_tag = "artist";    break;
        case ID_COPYRIGHT: metadata_tag = "copyright"; break;
        case ID_NAME:      metadata_tag = "title";     break;

        /

        case MKTAG('F','V','E','R'):
            if (data_size < 4)
                return AVERROR_INVALIDDATA;
            version = avio_rb32(pb);
            av_log(s, AV_LOG_DEBUG, "DSIFF v%d.%d.%d.%d\n",version >> 24, (version >> 16) & 0xFF, (version >> 8) & 0xFF, version & 0xFF);
            st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
            break;

        case MKTAG('D','I','I','N'):
            res = parse_dsd_diin(s, st, orig_pos + data_size);
            if (res < 0)
                return res;
            break;

        case MKTAG('P','R','O','P'):
            if (data_size < 4)
                return AVERROR_INVALIDDATA;
            if (avio_rl32(pb) != MKTAG('S','N','D',' ')) {
                avpriv_request_sample(s, "unknown property type");
                break;
            }
            res = parse_dsd_prop(s, st, orig_pos + data_size);
            if (res < 0)
                return res;
            break;

        case MKTAG('C','O','M','T'):
            if (data_size < 2)
                return AVERROR_INVALIDDATA;
            nb_comments = avio_rb16(pb);
            for (i = 0; i < nb_comments; i++) {
                int year, mon, day, hour, min, type, ref;
                char tmp[24];
                const char *tag;
                int metadata_size;

                year = avio_rb16(pb);
                mon  = avio_r8(pb);
                day  = avio_r8(pb);
                hour = avio_r8(pb);
                min  = avio_r8(pb);
                snprintf(tmp, sizeof(tmp), "%04d-%02d-%02d %02d:%02d", year, mon, day, hour, min);
                av_dict_set(&st->metadata, "comment_time", tmp, 0);

                type = avio_rb16(pb);
                ref  = avio_rb16(pb);
                switch (type) {
                case 1:
                    if (!i)
                        tag = "channel_comment";
                    else {
                        snprintf(tmp, sizeof(tmp), "channel%d_comment", ref);
                        tag = tmp;
                    }
                    break;
                case 2:
                    tag = ref < FF_ARRAY_ELEMS(dsd_source_comment) ? dsd_source_comment[ref] : "source_comment";
                    break;
                case 3:
                    tag = ref < FF_ARRAY_ELEMS(dsd_history_comment) ? dsd_history_comment[ref] : "file_history";
                    break;
                default:
                    tag = "comment";
                }

                metadata_size  = avio_rb32(pb);
                if ((res = get_metadata(s, tag, metadata_size)) < 0) {
                    av_log(s, AV_LOG_ERROR, "cannot allocate metadata tag %s!\n", tag);
                    return res;
                }

                if (metadata_size & 1)
                    avio_skip(pb, 1);
            }
            break;
        }

        if (metadata_tag) {
            if ((res = get_metadata(s, metadata_tag, data_size)) < 0) {
                av_log(s, AV_LOG_ERROR, "cannot allocate metadata tag %s!\n", metadata_tag);
                return res;
            }
        }
        avio_skip(pb, data_size - (avio_tell(pb) - orig_pos) + (data_size & 1));
    }

    if (st->codecpar->codec_tag == ID_ANIM)
        avio_seek(pb, 12, SEEK_SET);
    else
        avio_seek(pb, iff->body_pos, SEEK_SET);

    switch(st->codecpar->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        avpriv_set_pts_info(st, 32, 1, st->codecpar->sample_rate);

        if (st->codecpar->codec_tag == ID_16SV)
            st->codecpar->codec_id = AV_CODEC_ID_PCM_S16BE_PLANAR;
        else if (st->codecpar->codec_tag == ID_MAUD) {
            if (iff->maud_bits == 8 && !iff->maud_compression) {
                st->codecpar->codec_id = AV_CODEC_ID_PCM_U8;
            } else if (iff->maud_bits == 16 && !iff->maud_compression) {
                st->codecpar->codec_id = AV_CODEC_ID_PCM_S16BE;
            } else if (iff->maud_bits ==  8 && iff->maud_compression == 2) {
                st->codecpar->codec_id = AV_CODEC_ID_PCM_ALAW;
            } else if (iff->maud_bits ==  8 && iff->maud_compression == 3) {
                st->codecpar->codec_id = AV_CODEC_ID_PCM_MULAW;
            } else {
                avpriv_request_sample(s, "compression %d and bit depth %d", iff->maud_compression, iff->maud_bits);
                return AVERROR_PATCHWELCOME;
            }
        } else if (st->codecpar->codec_tag != ID_DSD &&
                   st->codecpar->codec_tag != ID_DST) {
            switch (iff->svx8_compression) {
            case COMP_NONE:
                st->codecpar->codec_id = AV_CODEC_ID_PCM_S8_PLANAR;
                break;
            case COMP_FIB:
                st->codecpar->codec_id = AV_CODEC_ID_8SVX_FIB;
                break;
            case COMP_EXP:
                st->codecpar->codec_id = AV_CODEC_ID_8SVX_EXP;
                break;
            default:
                av_log(s, AV_LOG_ERROR,
                       "Unknown SVX8 compression method '%d'\n", iff->svx8_compression);
                return -1;
            }
        }

        st->codecpar->bits_per_coded_sample = av_get_bits_per_sample(st->codecpar->codec_id);
        st->codecpar->bit_rate = st->codecpar->channels * st->codecpar->sample_rate * st->codecpar->bits_per_coded_sample;
        st->codecpar->block_align = st->codecpar->channels * st->codecpar->bits_per_coded_sample;
        if (st->codecpar->codec_tag == ID_DSD && st->codecpar->block_align <= 0)
            return AVERROR_INVALIDDATA;
        break;

    case AVMEDIA_TYPE_VIDEO:
        iff->bpp          = st->codecpar->bits_per_coded_sample;
        if (st->codecpar->codec_tag == ID_ANIM)
            avpriv_set_pts_info(st, 32, 1, 60);
        if ((screenmode & 0x800 /) && iff->bpp <= 8) {
            iff->ham      = iff->bpp > 6 ? 6 : 4;
            st->codecpar->bits_per_coded_sample = 24;
        }
        iff->flags        = (screenmode & 0x80 /) && iff->bpp <= 8;
        iff->masking      = masking;
        iff->transparency = transparency;

        if (!st->codecpar->extradata) {
            st->codecpar->extradata_size = IFF_EXTRA_VIDEO_SIZE;
            st->codecpar->extradata      = av_malloc(IFF_EXTRA_VIDEO_SIZE + AV_INPUT_BUFFER_PADDING_SIZE);
            if (!st->codecpar->extradata)
                return AVERROR(ENOMEM);
        }
        av_assert0(st->codecpar->extradata_size >= IFF_EXTRA_VIDEO_SIZE);
        buf = st->codecpar->extradata;
        bytestream_put_be16(&buf, IFF_EXTRA_VIDEO_SIZE);
        bytestream_put_byte(&buf, iff->bitmap_compression);
        bytestream_put_byte(&buf, iff->bpp);
        bytestream_put_byte(&buf, iff->ham);
        bytestream_put_byte(&buf, iff->flags);
        bytestream_put_be16(&buf, iff->transparency);
        bytestream_put_byte(&buf, iff->masking);
        bytestream_put_buffer(&buf, iff->tvdc, sizeof(iff->tvdc));
        st->codecpar->codec_id = AV_CODEC_ID_IFF_ILBM;
        break;
    default:
        return -1;
    }

    return 0;
}
