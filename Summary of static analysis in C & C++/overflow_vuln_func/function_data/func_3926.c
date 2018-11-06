static char *sdp_write_media_attributes(char *buff, int size, AVCodecContext *c, int payload_type, AVFormatContext *fmt)
{
    char *config = NULL;

    switch (c->codec_id) {
        case AV_CODEC_ID_H264: {
            int mode = 1;
            if (fmt && fmt->oformat && fmt->oformat->priv_class &&
                av_opt_flag_is_set(fmt->priv_data, "rtpflags", "h264_mode0"))
                mode = 0;
            if (c->extradata_size) {
                config = extradata2psets(c);
            }
            av_strlcatf(buff, size, "a=rtpmap:%d H264/90000\r\n"
                                    "a=fmtp:%d packetization-mode=%d%s\r\n",
                                     payload_type,
                                     payload_type, mode, config ? config : "");
            break;
        }
        case AV_CODEC_ID_H263:
        case AV_CODEC_ID_H263P:
            /
            if (!fmt || !fmt->oformat->priv_class ||
                !av_opt_flag_is_set(fmt->priv_data, "rtpflags", "rfc2190") ||
                c->codec_id == AV_CODEC_ID_H263P)
            av_strlcatf(buff, size, "a=rtpmap:%d H263-2000/90000\r\n"
                                    "a=framesize:%d %d-%d\r\n",
                                    payload_type,
                                    payload_type, c->width, c->height);
            break;
        case AV_CODEC_ID_HEVC:
            if (c->extradata_size)
                av_log(NULL, AV_LOG_WARNING, "HEVC extradata not currently "
                                             "passed properly through SDP\n");
            av_strlcatf(buff, size, "a=rtpmap:%d H265/90000\r\n", payload_type);
            break;
        case AV_CODEC_ID_MPEG4:
            if (c->extradata_size) {
                config = extradata2config(c);
            }
            av_strlcatf(buff, size, "a=rtpmap:%d MP4V-ES/90000\r\n"
                                    "a=fmtp:%d profile-level-id=1%s\r\n",
                                     payload_type,
                                     payload_type, config ? config : "");
            break;
        case AV_CODEC_ID_AAC:
            if (fmt && fmt->oformat->priv_class &&
                av_opt_flag_is_set(fmt->priv_data, "rtpflags", "latm")) {
                config = latm_context2config(c);
                if (!config)
                    return NULL;
                av_strlcatf(buff, size, "a=rtpmap:%d MP4A-LATM/%d/%d\r\n"
                                        "a=fmtp:%d profile-level-id=%d;cpresent=0;config=%s\r\n",
                                         payload_type, c->sample_rate, c->channels,
                                         payload_type, latm_context2profilelevel(c), config);
            } else {
                if (c->extradata_size) {
                    config = extradata2config(c);
                } else {
                    /
                    av_log(c, AV_LOG_ERROR, "AAC with no global headers is currently not supported.\n");
                    return NULL;
                }
                if (!config) {
                    return NULL;
                }
                av_strlcatf(buff, size, "a=rtpmap:%d MPEG4-GENERIC/%d/%d\r\n"
                                        "a=fmtp:%d profile-level-id=1;"
                                        "mode=AAC-hbr;sizelength=13;indexlength=3;"
                                        "indexdeltalength=3%s\r\n",
                                         payload_type, c->sample_rate, c->channels,
                                         payload_type, config);
            }
            break;
        case AV_CODEC_ID_PCM_S16BE:
            if (payload_type >= RTP_PT_PRIVATE)
                av_strlcatf(buff, size, "a=rtpmap:%d L16/%d/%d\r\n",
                                         payload_type,
                                         c->sample_rate, c->channels);
            break;
        case AV_CODEC_ID_PCM_MULAW:
            if (payload_type >= RTP_PT_PRIVATE)
                av_strlcatf(buff, size, "a=rtpmap:%d PCMU/%d/%d\r\n",
                                         payload_type,
                                         c->sample_rate, c->channels);
            break;
        case AV_CODEC_ID_PCM_ALAW:
            if (payload_type >= RTP_PT_PRIVATE)
                av_strlcatf(buff, size, "a=rtpmap:%d PCMA/%d/%d\r\n",
                                         payload_type,
                                         c->sample_rate, c->channels);
            break;
        case AV_CODEC_ID_AMR_NB:
            av_strlcatf(buff, size, "a=rtpmap:%d AMR/%d/%d\r\n"
                                    "a=fmtp:%d octet-align=1\r\n",
                                     payload_type, c->sample_rate, c->channels,
                                     payload_type);
            break;
        case AV_CODEC_ID_AMR_WB:
            av_strlcatf(buff, size, "a=rtpmap:%d AMR-WB/%d/%d\r\n"
                                    "a=fmtp:%d octet-align=1\r\n",
                                     payload_type, c->sample_rate, c->channels,
                                     payload_type);
            break;
        case AV_CODEC_ID_VORBIS:
            if (c->extradata_size)
                config = xiph_extradata2config(c);
            else
                av_log(c, AV_LOG_ERROR, "Vorbis configuration info missing\n");
            if (!config)
                return NULL;

            av_strlcatf(buff, size, "a=rtpmap:%d vorbis/%d/%d\r\n"
                                    "a=fmtp:%d configuration=%s\r\n",
                                    payload_type, c->sample_rate, c->channels,
                                    payload_type, config);
            break;
        case AV_CODEC_ID_THEORA: {
            const char *pix_fmt;
            switch (c->pix_fmt) {
            case AV_PIX_FMT_YUV420P:
                pix_fmt = "YCbCr-4:2:0";
                break;
            case AV_PIX_FMT_YUV422P:
                pix_fmt = "YCbCr-4:2:2";
                break;
            case AV_PIX_FMT_YUV444P:
                pix_fmt = "YCbCr-4:4:4";
                break;
            default:
                av_log(c, AV_LOG_ERROR, "Unsupported pixel format.\n");
                return NULL;
            }

            if (c->extradata_size)
                config = xiph_extradata2config(c);
            else
                av_log(c, AV_LOG_ERROR, "Theora configuation info missing\n");
            if (!config)
                return NULL;

            av_strlcatf(buff, size, "a=rtpmap:%d theora/90000\r\n"
                                    "a=fmtp:%d delivery-method=inline; "
                                    "width=%d; height=%d; sampling=%s; "
                                    "configuration=%s\r\n",
                                    payload_type, payload_type,
                                    c->width, c->height, pix_fmt, config);
            break;
        }
        case AV_CODEC_ID_VP8:
            av_strlcatf(buff, size, "a=rtpmap:%d VP8/90000\r\n",
                                     payload_type);
            break;
        case AV_CODEC_ID_MJPEG:
            if (payload_type >= RTP_PT_PRIVATE)
                av_strlcatf(buff, size, "a=rtpmap:%d JPEG/90000\r\n",
                                         payload_type);
            break;
        case AV_CODEC_ID_ADPCM_G722:
            if (payload_type >= RTP_PT_PRIVATE)
                av_strlcatf(buff, size, "a=rtpmap:%d G722/%d/%d\r\n",
                                         payload_type,
                                         8000, c->channels);
            break;
        case AV_CODEC_ID_ADPCM_G726: {
            if (payload_type >= RTP_PT_PRIVATE)
                av_strlcatf(buff, size, "a=rtpmap:%d G726-%d/%d\r\n",
                                         payload_type,
                                         c->bits_per_coded_sample*8,
                                         c->sample_rate);
            break;
        }
        case AV_CODEC_ID_ILBC:
            av_strlcatf(buff, size, "a=rtpmap:%d iLBC/%d\r\n"
                                    "a=fmtp:%d mode=%d\r\n",
                                     payload_type, c->sample_rate,
                                     payload_type, c->block_align == 38 ? 20 : 30);
            break;
        case AV_CODEC_ID_SPEEX:
            av_strlcatf(buff, size, "a=rtpmap:%d speex/%d\r\n",
                                     payload_type, c->sample_rate);
            break;
        case AV_CODEC_ID_OPUS:
            /
            av_strlcatf(buff, size, "a=rtpmap:%d opus/48000/2\r\n",
                                     payload_type);
            if (c->channels == 2) {
                av_strlcatf(buff, size, "a=fmtp:%d sprop-stereo:1\r\n",
                                         payload_type);
            }
            break;
        default:
            /
            break;
    }

    av_free(config);

    return buff;
}
