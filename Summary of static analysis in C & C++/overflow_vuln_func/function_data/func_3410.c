static void do_audio_out(AVFormatContext *s, OutputStream *ost,
                         InputStream *ist, AVFrame *decoded_frame)
{
    uint8_t *buftmp;
    int64_t size_out;

    int frame_bytes, resample_changed;
    AVCodecContext *enc = ost->st->codec;
    AVCodecContext *dec = ist->st->codec;
    int osize = av_get_bytes_per_sample(enc->sample_fmt);
    int isize = av_get_bytes_per_sample(dec->sample_fmt);
    uint8_t *buf[AV_NUM_DATA_POINTERS];
    int size     = decoded_frame->nb_samples * dec->channels * isize;
    int planes   = av_sample_fmt_is_planar(dec->sample_fmt) ? dec->channels : 1;
    int i;
    int out_linesize = 0;

    av_assert0(planes <= AV_NUM_DATA_POINTERS);

    for(i=0; i<planes; i++)
        buf[i]= decoded_frame->data[i];


    get_default_channel_layouts(ost, ist);

    if (alloc_audio_output_buf(dec, enc, decoded_frame->nb_samples, &out_linesize) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Error allocating audio buffer\n");
        exit_program(1);
    }

    if (audio_sync_method > 1                      ||
        enc->channels       != dec->channels       ||
        enc->channel_layout != dec->channel_layout ||
        enc->sample_rate    != dec->sample_rate    ||
        dec->sample_fmt     != enc->sample_fmt)
        ost->audio_resample = 1;

    resample_changed = ost->resample_sample_fmt  != dec->sample_fmt ||
                       ost->resample_channels    != dec->channels   ||
                       ost->resample_channel_layout != dec->channel_layout ||
                       ost->resample_sample_rate != dec->sample_rate;

    if ((ost->audio_resample && !ost->swr) || resample_changed || ost->audio_channels_mapped) {

        if (resample_changed) {
            av_log(NULL, AV_LOG_INFO, "Input stream #%d:%d frame changed from rate:%d fmt:%s ch:%d chl:0x%"PRIx64" to rate:%d fmt:%s ch:%d chl:0x%"PRIx64"\n",
                   ist->file_index, ist->st->index,
                   ost->resample_sample_rate, av_get_sample_fmt_name(ost->resample_sample_fmt),
                   ost->resample_channels, ost->resample_channel_layout,
                   dec->sample_rate, av_get_sample_fmt_name(dec->sample_fmt),
                   dec->channels, dec->channel_layout);
            ost->resample_sample_fmt  = dec->sample_fmt;
            ost->resample_channels    = dec->channels;
            ost->resample_channel_layout = dec->channel_layout;
            ost->resample_sample_rate = dec->sample_rate;
            swr_free(&ost->swr);
        }
        /
        if (audio_sync_method <= 1 && !ost->audio_channels_mapped &&
            ost->resample_sample_fmt  == enc->sample_fmt &&
            ost->resample_channels    == enc->channels   &&
            ost->resample_channel_layout == enc->channel_layout &&
            ost->resample_sample_rate == enc->sample_rate) {
            //ost->swr = NULL;
            ost->audio_resample = 0;
        } else {
            ost->swr = swr_alloc_set_opts(ost->swr,
                                          enc->channel_layout, enc->sample_fmt, enc->sample_rate,
                                          dec->channel_layout, dec->sample_fmt, dec->sample_rate,
                                          0, NULL);
            av_opt_set_int(ost->swr, "dither_method", ost->swr_dither_method,0);
            av_opt_set_double(ost->swr, "dither_scale", ost->swr_dither_scale,0);
            if (ost->audio_channels_mapped)
                swr_set_channel_mapping(ost->swr, ost->audio_channels_map);
            av_opt_set_double(ost->swr, "rmvol", ost->rematrix_volume, 0);
            if (ost->audio_channels_mapped) {
                av_opt_set_int(ost->swr, "icl", av_get_default_channel_layout(ost->audio_channels_mapped), 0);
                av_opt_set_int(ost->swr, "uch", ost->audio_channels_mapped, 0);
            }
            if (av_opt_set_int(ost->swr, "ich", dec->channels, 0) < 0) {
                av_log(NULL, AV_LOG_FATAL, "Unsupported number of input channels\n");
                exit_program(1);
            }
            if (av_opt_set_int(ost->swr, "och", enc->channels, 0) < 0) {
                av_log(NULL, AV_LOG_FATAL, "Unsupported number of output channels\n");
                exit_program(1);
            }
            if(audio_sync_method>1) av_opt_set_int(ost->swr, "flags", SWR_FLAG_RESAMPLE, 0);
            if(ost->swr && swr_init(ost->swr) < 0){
                av_log(NULL, AV_LOG_FATAL, "swr_init() failed\n");
                swr_free(&ost->swr);
            }

            if (!ost->swr) {
                av_log(NULL, AV_LOG_FATAL, "Can not resample %d channels @ %d Hz to %d channels @ %d Hz\n",
                        dec->channels, dec->sample_rate,
                        enc->channels, enc->sample_rate);
                exit_program(1);
            }
        }
    }

    av_assert0(ost->audio_resample || dec->sample_fmt==enc->sample_fmt);

    if (audio_sync_method > 0) {
        double delta = get_sync_ipts(ost, ist->pts) * enc->sample_rate - ost->sync_opts -
                       av_fifo_size(ost->fifo) / (enc->channels * osize);
        int idelta = delta * dec->sample_rate / enc->sample_rate;
        int byte_delta = idelta * isize * dec->channels;

        // FIXME resample delay
        if (fabs(delta) > 50) {
            if (ist->is_start || fabs(delta) > audio_drift_threshold*enc->sample_rate) {
                if (byte_delta < 0) {
                    byte_delta = FFMAX(byte_delta, -size);
                    size += byte_delta;
                    for (i=0; i<planes; i++)
                        buf[i]  -= byte_delta/planes;
                    av_log(NULL, AV_LOG_VERBOSE, "discarding %d audio samples\n",
                           -byte_delta / (isize * dec->channels));
                    if (!size)
                        return;
                    ist->is_start = 0;
                } else {
                    av_fast_malloc(&async_buf, &allocated_async_buf_size,
                                   byte_delta + size);
                    if (!async_buf) {
                        av_log(NULL, AV_LOG_FATAL, "Out of memory in do_audio_out\n");
                        exit_program(1);
                    }

                    if (alloc_audio_output_buf(dec, enc, decoded_frame->nb_samples + idelta, &out_linesize) < 0) {
                        av_log(NULL, AV_LOG_FATAL, "Error allocating audio buffer\n");
                        exit_program(1);
                    }
                    ist->is_start = 0;

                    for (i=0; i<planes; i++) {
                        uint8_t *t = async_buf + i*((byte_delta + size)/planes);
                        generate_silence(t, dec->sample_fmt, byte_delta/planes);
                        memcpy(t + byte_delta/planes, buf[i], size/planes);
                        buf[i] = t;
                    }
                    size += byte_delta;
                    av_log(NULL, AV_LOG_VERBOSE, "adding %d audio samples of silence\n", idelta);
                }
            } else if (audio_sync_method > 1) {
                int comp = av_clip(delta, -audio_sync_method, audio_sync_method);
                av_log(NULL, AV_LOG_VERBOSE, "compensating audio timestamp drift:%f compensation:%d in:%d\n",
                       delta, comp, enc->sample_rate);
//                fprintf(stderr, "drift:%f len:%d opts:%"PRId64" ipts:%"PRId64" fifo:%d\n", delta, -1, ost->sync_opts, (int64_t)(get_sync_ipts(ost) * enc->sample_rate), av_fifo_size(ost->fifo)/(ost->st->codec->channels * 2));
                swr_set_compensation(ost->swr, comp, enc->sample_rate);
            }
        }
    } else if (audio_sync_method == 0)
        ost->sync_opts = lrintf(get_sync_ipts(ost, ist->pts) * enc->sample_rate) -
                                av_fifo_size(ost->fifo) / (enc->channels * osize); // FIXME wrong

    if (ost->audio_resample || ost->audio_channels_mapped) {
        buftmp = audio_buf;
        size_out = swr_convert(ost->swr, (      uint8_t*[]){buftmp},
                                      allocated_audio_buf_size / (enc->channels * osize),
                                      (const uint8_t **)buf,
                                      size / (dec->channels * isize));
        if (size_out < 0) {
            av_log(NULL, AV_LOG_FATAL, "swr_convert failed\n");
            exit_program(1);
        }
        size_out = size_out * enc->channels * osize;
    } else {
        buftmp = buf[0];
        size_out = size;
    }

    av_assert0(ost->audio_resample || dec->sample_fmt==enc->sample_fmt);

    /
    if (!(enc->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE)) {
        /
        if (av_fifo_realloc2(ost->fifo, av_fifo_size(ost->fifo) + size_out) < 0) {
            av_log(NULL, AV_LOG_FATAL, "av_fifo_realloc2() failed\n");
            exit_program(1);
        }
        av_fifo_generic_write(ost->fifo, buftmp, size_out, NULL);

        frame_bytes = enc->frame_size * osize * enc->channels;

        while (av_fifo_size(ost->fifo) >= frame_bytes) {
            av_fifo_generic_read(ost->fifo, audio_buf, frame_bytes, NULL);
            encode_audio_frame(s, ost, audio_buf, frame_bytes);
        }
    } else {
        encode_audio_frame(s, ost, buftmp, size_out);
    }
}
