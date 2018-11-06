static int mpeg_mux_init(AVFormatContext *ctx)
{
    MpegMuxContext *s = ctx->priv_data;
    int bitrate, i, mpa_id, mpv_id, mps_id, ac3_id, dts_id, lpcm_id, j;
    AVStream *st;
    StreamInfo *stream;
    int audio_bitrate;
    int video_bitrate;

    s->packet_number = 0;
    s->is_vcd =    (CONFIG_MPEG1VCD_MUXER  && ctx->oformat == &ff_mpeg1vcd_muxer);
    s->is_svcd =   (CONFIG_MPEG2SVCD_MUXER && ctx->oformat == &ff_mpeg2svcd_muxer);
    s->is_mpeg2 = ((CONFIG_MPEG2VOB_MUXER  && ctx->oformat == &ff_mpeg2vob_muxer) ||
                   (CONFIG_MPEG2DVD_MUXER  && ctx->oformat == &ff_mpeg2dvd_muxer) ||
                   (CONFIG_MPEG2SVCD_MUXER && ctx->oformat == &ff_mpeg2svcd_muxer));
    s->is_dvd =    (CONFIG_MPEG2DVD_MUXER  && ctx->oformat == &ff_mpeg2dvd_muxer);

    if(ctx->packet_size) {
        if (ctx->packet_size < 20 || ctx->packet_size > (1 << 23) + 10) {
            av_log(ctx, AV_LOG_ERROR, "Invalid packet size %d\n",
                   ctx->packet_size);
            goto fail;
        }
        s->packet_size = ctx->packet_size;
    } else
        s->packet_size = 2048;

    s->vcd_padding_bytes_written = 0;
    s->vcd_padding_bitrate=0;

    s->audio_bound = 0;
    s->video_bound = 0;
    mpa_id = AUDIO_ID;
    ac3_id = AC3_ID;
    dts_id = DTS_ID;
    mpv_id = VIDEO_ID;
    mps_id = SUB_ID;
    lpcm_id = LPCM_ID;
    for(i=0;i<ctx->nb_streams;i++) {
        st = ctx->streams[i];
        stream = av_mallocz(sizeof(StreamInfo));
        if (!stream)
            goto fail;
        st->priv_data = stream;

        avpriv_set_pts_info(st, 64, 1, 90000);

        switch(st->codec->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            if        (st->codec->codec_id == CODEC_ID_AC3) {
                stream->id = ac3_id++;
            } else if (st->codec->codec_id == CODEC_ID_DTS) {
                stream->id = dts_id++;
            } else if (st->codec->codec_id == CODEC_ID_PCM_S16BE) {
                stream->id = lpcm_id++;
                for(j = 0; j < 4; j++) {
                    if (lpcm_freq_tab[j] == st->codec->sample_rate)
                        break;
                }
                if (j == 4)
                    goto fail;
                if (st->codec->channels > 8)
                    return -1;
                stream->lpcm_header[0] = 0x0c;
                stream->lpcm_header[1] = (st->codec->channels - 1) | (j << 4);
                stream->lpcm_header[2] = 0x80;
                stream->lpcm_align = st->codec->channels * 2;
            } else {
                stream->id = mpa_id++;
            }

            /
            stream->max_buffer_size = 4 * 1024;
            s->audio_bound++;
            break;
        case AVMEDIA_TYPE_VIDEO:
            stream->id = mpv_id++;
            if (st->codec->rc_buffer_size)
                stream->max_buffer_size = 6*1024 + st->codec->rc_buffer_size/8;
            else {
                av_log(ctx, AV_LOG_WARNING, "VBV buffer size not set, muxing may fail\n");
                stream->max_buffer_size = 230*1024; //FIXME this is probably too small as default
            }
#if 0
                /
                stream->max_buffer_size = 46 * 1024;
            else
                /                   Right now it is also used for everything else.*/
                stream->max_buffer_size = 230 * 1024;
#endif
            s->video_bound++;
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            stream->id = mps_id++;
            stream->max_buffer_size = 16 * 1024;
            break;
        default:
            return -1;
        }
        stream->fifo= av_fifo_alloc(16);
        if (!stream->fifo)
            goto fail;
    }
    bitrate = 0;
    audio_bitrate = 0;
    video_bitrate = 0;
    for(i=0;i<ctx->nb_streams;i++) {
        int codec_rate;
        st = ctx->streams[i];
        stream = (StreamInfo*) st->priv_data;

        if(st->codec->rc_max_rate || stream->id==VIDEO_ID)
            codec_rate= st->codec->rc_max_rate;
        else
            codec_rate= st->codec->bit_rate;

        if(!codec_rate)
            codec_rate= (1<<21)*8*50/ctx->nb_streams;

        bitrate += codec_rate;

        if ((stream->id & 0xe0) == AUDIO_ID)
            audio_bitrate += codec_rate;
        else if (stream->id==VIDEO_ID)
            video_bitrate += codec_rate;
    }

#if FF_API_MUXRATE
    if(ctx->mux_rate){
        s->mux_rate= (ctx->mux_rate + (8 * 50) - 1) / (8 * 50);
    } else
#endif
    if (!s->mux_rate) {
        /
        bitrate += bitrate*5/100;
        bitrate += 10000;
        s->mux_rate = (bitrate + (8 * 50) - 1) / (8 * 50);
    }

    if (s->is_vcd) {
        double overhead_rate;

        /
//        s->mux_rate=2352 * 75 / 50;    /

        /

        /
        overhead_rate  = ((audio_bitrate / 8.0) / 2279) * (2324 - 2279);
        overhead_rate += ((video_bitrate / 8.0) / 2294) * (2324 - 2294);
        overhead_rate *= 8;

        /
        s->vcd_padding_bitrate = 2324 * 75 * 8 - (bitrate + overhead_rate);
    }

    if (s->is_vcd || s->is_mpeg2)
        /
        s->pack_header_freq = 1;
    else
        /
        s->pack_header_freq = 2 * bitrate / s->packet_size / 8;

    /
    if (s->pack_header_freq == 0)
       s->pack_header_freq = 1;

    if (s->is_mpeg2)
        /
        s->system_header_freq = s->pack_header_freq * 40;
    else if (s->is_vcd)
        /
        s->system_header_freq = 0x7fffffff;
    else
        s->system_header_freq = s->pack_header_freq * 5;

    for(i=0;i<ctx->nb_streams;i++) {
        stream = ctx->streams[i]->priv_data;
        stream->packet_number = 0;
    }
    s->system_header_size = get_system_header_size(ctx);
    s->last_scr = 0;
    return 0;
 fail:
    for(i=0;i<ctx->nb_streams;i++) {
        av_free(ctx->streams[i]->priv_data);
    }
    return AVERROR(ENOMEM);
}
