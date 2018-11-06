void avcodec_string(char *buf, int buf_size, AVCodecContext *enc, int encode)
{
    const char *codec_type;
    const char *codec_name;
    const char *profile = NULL;
    const AVCodec *p;
    int64_t bitrate;
    int new_line = 0;
    AVRational display_aspect_ratio;
    const char *separator = enc->dump_separator ? (const char *)enc->dump_separator : ", ";

    if (!buf || buf_size <= 0)
        return;
    codec_type = av_get_media_type_string(enc->codec_type);
    codec_name = avcodec_get_name(enc->codec_id);
    if (enc->profile != FF_PROFILE_UNKNOWN) {
        if (enc->codec)
            p = enc->codec;
        else
            p = encode ? avcodec_find_encoder(enc->codec_id) :
                        avcodec_find_decoder(enc->codec_id);
        if (p)
            profile = av_get_profile_name(p, enc->profile);
    }

    snprintf(buf, buf_size, "%s: %s", codec_type ? codec_type : "unknown",
             codec_name);
    buf[0] ^= 'a' ^ 'A'; /

    if (enc->codec && strcmp(enc->codec->name, codec_name))
        snprintf(buf + strlen(buf), buf_size - strlen(buf), " (%s)", enc->codec->name);

    if (profile)
        snprintf(buf + strlen(buf), buf_size - strlen(buf), " (%s)", profile);
    if (   enc->codec_type == AVMEDIA_TYPE_VIDEO
        && av_log_get_level() >= AV_LOG_VERBOSE
        && enc->refs)
        snprintf(buf + strlen(buf), buf_size - strlen(buf),
                 ", %d reference frame%s",
                 enc->refs, enc->refs > 1 ? "s" : "");

    if (enc->codec_tag) {
        char tag_buf[32];
        av_get_codec_tag_string(tag_buf, sizeof(tag_buf), enc->codec_tag);
        snprintf(buf + strlen(buf), buf_size - strlen(buf),
                 " (%s / 0x%04X)", tag_buf, enc->codec_tag);
    }

    switch (enc->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        {
            char detail[256] = "(";

            av_strlcat(buf, separator, buf_size);

            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                 "%s", enc->pix_fmt == AV_PIX_FMT_NONE ? "none" :
                     av_get_pix_fmt_name(enc->pix_fmt));
            if (enc->bits_per_raw_sample && enc->pix_fmt != AV_PIX_FMT_NONE &&
                enc->bits_per_raw_sample < av_pix_fmt_desc_get(enc->pix_fmt)->comp[0].depth)
                av_strlcatf(detail, sizeof(detail), "%d bpc, ", enc->bits_per_raw_sample);
            if (enc->color_range != AVCOL_RANGE_UNSPECIFIED)
                av_strlcatf(detail, sizeof(detail), "%s, ",
                            av_color_range_name(enc->color_range));

            if (enc->colorspace != AVCOL_SPC_UNSPECIFIED ||
                enc->color_primaries != AVCOL_PRI_UNSPECIFIED ||
                enc->color_trc != AVCOL_TRC_UNSPECIFIED) {
                if (enc->colorspace != (int)enc->color_primaries ||
                    enc->colorspace != (int)enc->color_trc) {
                    new_line = 1;
                    av_strlcatf(detail, sizeof(detail), "%s/%s/%s, ",
                                av_color_space_name(enc->colorspace),
                                av_color_primaries_name(enc->color_primaries),
                                av_color_transfer_name(enc->color_trc));
                } else
                    av_strlcatf(detail, sizeof(detail), "%s, ",
                                av_get_colorspace_name(enc->colorspace));
            }

            if (av_log_get_level() >= AV_LOG_DEBUG &&
                enc->chroma_sample_location != AVCHROMA_LOC_UNSPECIFIED)
                av_strlcatf(detail, sizeof(detail), "%s, ",
                            av_chroma_location_name(enc->chroma_sample_location));

            if (strlen(detail) > 1) {
                detail[strlen(detail) - 2] = 0;
                av_strlcatf(buf, buf_size, "%s)", detail);
            }
        }

        if (enc->width) {
            av_strlcat(buf, new_line ? separator : ", ", buf_size);

            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     "%dx%d",
                     enc->width, enc->height);

            if (av_log_get_level() >= AV_LOG_VERBOSE &&
                (enc->width != enc->coded_width ||
                 enc->height != enc->coded_height))
                snprintf(buf + strlen(buf), buf_size - strlen(buf),
                         " (%dx%d)", enc->coded_width, enc->coded_height);

            if (enc->sample_aspect_ratio.num) {
                av_reduce(&display_aspect_ratio.num, &display_aspect_ratio.den,
                          enc->width * enc->sample_aspect_ratio.num,
                          enc->height * enc->sample_aspect_ratio.den,
                          1024 * 1024);
                snprintf(buf + strlen(buf), buf_size - strlen(buf),
                         " [SAR %d:%d DAR %d:%d]",
                         enc->sample_aspect_ratio.num, enc->sample_aspect_ratio.den,
                         display_aspect_ratio.num, display_aspect_ratio.den);
            }
            if (av_log_get_level() >= AV_LOG_DEBUG) {
                int g = av_gcd(enc->time_base.num, enc->time_base.den);
                snprintf(buf + strlen(buf), buf_size - strlen(buf),
                         ", %d/%d",
                         enc->time_base.num / g, enc->time_base.den / g);
            }
        }
        if (encode) {
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", q=%d-%d", enc->qmin, enc->qmax);
        } else {
            if (enc->properties & FF_CODEC_PROPERTY_CLOSED_CAPTIONS)
                snprintf(buf + strlen(buf), buf_size - strlen(buf),
                         ", Closed Captions");
            if (enc->properties & FF_CODEC_PROPERTY_LOSSLESS)
                snprintf(buf + strlen(buf), buf_size - strlen(buf),
                         ", lossless");
        }
        break;
    case AVMEDIA_TYPE_AUDIO:
        av_strlcat(buf, separator, buf_size);

        if (enc->sample_rate) {
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     "%d Hz, ", enc->sample_rate);
        }
        av_get_channel_layout_string(buf + strlen(buf), buf_size - strlen(buf), enc->channels, enc->channel_layout);
        if (enc->sample_fmt != AV_SAMPLE_FMT_NONE) {
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %s", av_get_sample_fmt_name(enc->sample_fmt));
        }
        if (   enc->bits_per_raw_sample > 0
            && enc->bits_per_raw_sample != av_get_bytes_per_sample(enc->sample_fmt) * 8)
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     " (%d bit)", enc->bits_per_raw_sample);
        break;
    case AVMEDIA_TYPE_DATA:
        if (av_log_get_level() >= AV_LOG_DEBUG) {
            int g = av_gcd(enc->time_base.num, enc->time_base.den);
            if (g)
                snprintf(buf + strlen(buf), buf_size - strlen(buf),
                         ", %d/%d",
                         enc->time_base.num / g, enc->time_base.den / g);
        }
        break;
    case AVMEDIA_TYPE_SUBTITLE:
        if (enc->width)
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %dx%d", enc->width, enc->height);
        break;
    default:
        return;
    }
    if (encode) {
        if (enc->flags & AV_CODEC_FLAG_PASS1)
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", pass 1");
        if (enc->flags & AV_CODEC_FLAG_PASS2)
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", pass 2");
    }
    bitrate = get_bit_rate(enc);
    if (bitrate != 0) {
        snprintf(buf + strlen(buf), buf_size - strlen(buf),
                 ", %"PRId64" kb/s", bitrate / 1000);
    } else if (enc->rc_max_rate > 0) {
        snprintf(buf + strlen(buf), buf_size - strlen(buf),
                 ", max. %"PRId64" kb/s", (int64_t)enc->rc_max_rate / 1000);
    }
}
