static void do_audio_out(AVFormatContext *s, OutputStream *ost,
                         InputStream *ist, AVFrame *decoded_frame)
{
    uint8_t *buftmp;

    int size_out, frame_bytes, resample_changed, ret;
    AVCodecContext *enc = ost->st->codec;
    AVCodecContext *dec = ist->st->codec;
    int osize = av_get_bytes_per_sample(enc->sample_fmt);
    int isize = av_get_bytes_per_sample(dec->sample_fmt);
    uint8_t *buf = decoded_frame->data[0];
    int size     = decoded_frame->nb_samples * dec->channels * isize;
    int out_linesize = 0;
    int buf_linesize = decoded_frame->linesize[0];

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

    if ((ost->audio_resample && !ost->avr) || resample_changed) {
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
            if (ost->avr)
                avresample_close(ost->avr);
        }
        /
        if (audio_sync_method <= 1 &&
            ost->resample_sample_fmt  == enc->sample_fmt &&
            ost->resample_channels    == enc->channels   &&
            ost->resample_channel_layout == enc->channel_layout &&
            ost->resample_sample_rate == enc->sample_rate) {
            ost->audio_resample = 0;
        } else if (ost->audio_resample) {
            if (!ost->avr) {
                ost->avr = avresample_alloc_context();
                if (!ost->avr) {
                    av_log(NULL, AV_LOG_FATAL, "Error allocating context for libavresample\n");
                    exit_program(1);
                }
            }

            av_opt_set_int(ost->avr, "in_channel_layout",  dec->channel_layout, 0);
            av_opt_set_int(ost->avr, "in_sample_fmt",      dec->sample_fmt,     0);
            av_opt_set_int(ost->avr, "in_sample_rate",     dec->sample_rate,    0);
            av_opt_set_int(ost->avr, "out_channel_layout", enc->channel_layout, 0);
            av_opt_set_int(ost->avr, "out_sample_fmt",     enc->sample_fmt,     0);
            av_opt_set_int(ost->avr, "out_sample_rate",    enc->sample_rate,    0);
            if (audio_sync_method > 1)
                av_opt_set_int(ost->avr, "force_resampling", 1, 0);

            /
            if (av_get_bytes_per_sample(dec->sample_fmt) <= 2 &&
                av_get_bytes_per_sample(enc->sample_fmt) <= 2) {
                av_opt_set_int(ost->avr, "internal_sample_fmt", AV_SAMPLE_FMT_S16P, 0);
            }

            ret = avresample_open(ost->avr);
            if (ret < 0) {
                av_log(NULL, AV_LOG_FATAL, "Error opening libavresample\n");
                exit_program(1);
            }
        }
    }

    if (audio_sync_method > 0) {
        double delta = get_sync_ipts(ost, ist->last_dts) * enc->sample_rate - ost->sync_opts -
                       av_fifo_size(ost->fifo) / (enc->channels * osize);
        int idelta = delta * dec->sample_rate / enc->sample_rate;
        int byte_delta = idelta * isize * dec->channels;

        // FIXME resample delay
        if (fabs(delta) > 50) {
            if (ist->is_start || fabs(delta) > audio_drift_threshold*enc->sample_rate) {
                if (byte_delta < 0) {
                    byte_delta = FFMAX(byte_delta, -size);
                    size += byte_delta;
                    buf  -= byte_delta;
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

                    generate_silence(async_buf, dec->sample_fmt, byte_delta);
                    memcpy(async_buf + byte_delta, buf, size);
                    buf = async_buf;
                    size += byte_delta;
                    buf_linesize = allocated_async_buf_size;
                    av_log(NULL, AV_LOG_VERBOSE, "adding %d audio samples of silence\n", idelta);
                }
            } else if (audio_sync_method > 1) {
                int comp = av_clip(delta, -audio_sync_method, audio_sync_method);
                av_log(NULL, AV_LOG_VERBOSE, "compensating audio timestamp drift:%f compensation:%d in:%d\n",
                       delta, comp, enc->sample_rate);
//                fprintf(stderr, "drift:%f len:%d opts:%"PRId64" ipts:%"PRId64" fifo:%d\n", delta, -1, ost->sync_opts, (int64_t)(get_sync_ipts(ost) * enc->sample_rate), av_fifo_size(ost->fifo)/(ost->st->codec->channels * 2));
                avresample_set_compensation(ost->avr, comp, enc->sample_rate);
            }
        }
    } else if (audio_sync_method == 0)
        ost->sync_opts = lrintf(get_sync_ipts(ost, ist->last_dts) * enc->sample_rate) -
                                av_fifo_size(ost->fifo) / (enc->channels * osize); // FIXME wrong

    if (ost->audio_resample) {
        buftmp = audio_buf;
        size_out = avresample_convert(ost->avr, (void **)&buftmp,
                                      allocated_audio_buf_size, out_linesize,
                                      (void **)&buf, buf_linesize,
                                      size / (dec->channels * isize));
        size_out = size_out * enc->channels * osize;
    } else {
        buftmp = buf;
        size_out = size;
    }

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
