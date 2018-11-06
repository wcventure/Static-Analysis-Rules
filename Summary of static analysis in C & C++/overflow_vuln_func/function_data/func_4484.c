static int audio_decode_frame(VideoState *is)
{
    int data_size, resampled_data_size;
    int64_t dec_channel_layout;
    int got_frame = 0;
    av_unused double audio_clock0;
    int wanted_nb_samples;
    AVRational tb;
    int ret;
    int reconfigure;

    if (!is->frame)
        if (!(is->frame = av_frame_alloc()))
            return AVERROR(ENOMEM);

    for (;;) {
        if (is->audioq.serial != is->auddec.pkt_serial)
            is->audio_buf_frames_pending = got_frame = 0;

        if (!got_frame)
            av_frame_unref(is->frame);

        if (is->paused)
            return -1;

        while (is->audio_buf_frames_pending || got_frame) {
            if (!is->audio_buf_frames_pending) {
                got_frame = 0;
                tb = (AVRational){1, is->frame->sample_rate};

#if CONFIG_AVFILTER
                dec_channel_layout = get_valid_channel_layout(is->frame->channel_layout, av_frame_get_channels(is->frame));

                reconfigure =
                    cmp_audio_fmts(is->audio_filter_src.fmt, is->audio_filter_src.channels,
                                   is->frame->format, av_frame_get_channels(is->frame))    ||
                    is->audio_filter_src.channel_layout != dec_channel_layout ||
                    is->audio_filter_src.freq           != is->frame->sample_rate ||
                    is->auddec.pkt_serial               != is->audio_last_serial;

                if (reconfigure) {
                    char buf1[1024], buf2[1024];
                    av_get_channel_layout_string(buf1, sizeof(buf1), -1, is->audio_filter_src.channel_layout);
                    av_get_channel_layout_string(buf2, sizeof(buf2), -1, dec_channel_layout);
                    av_log(NULL, AV_LOG_DEBUG,
                           "Audio frame changed from rate:%d ch:%d fmt:%s layout:%s serial:%d to rate:%d ch:%d fmt:%s layout:%s serial:%d\n",
                           is->audio_filter_src.freq, is->audio_filter_src.channels, av_get_sample_fmt_name(is->audio_filter_src.fmt), buf1, is->audio_last_serial,
                           is->frame->sample_rate, av_frame_get_channels(is->frame), av_get_sample_fmt_name(is->frame->format), buf2, is->auddec.pkt_serial);

                    is->audio_filter_src.fmt            = is->frame->format;
                    is->audio_filter_src.channels       = av_frame_get_channels(is->frame);
                    is->audio_filter_src.channel_layout = dec_channel_layout;
                    is->audio_filter_src.freq           = is->frame->sample_rate;
                    is->audio_last_serial               = is->auddec.pkt_serial;

                    if ((ret = configure_audio_filters(is, afilters, 1)) < 0)
                        return ret;
                }

                if ((ret = av_buffersrc_add_frame(is->in_audio_filter, is->frame)) < 0)
                    return ret;
#endif
            }
#if CONFIG_AVFILTER
            if ((ret = av_buffersink_get_frame_flags(is->out_audio_filter, is->frame, 0)) < 0) {
                if (ret == AVERROR(EAGAIN)) {
                    is->audio_buf_frames_pending = 0;
                    continue;
                }
                if (ret == AVERROR_EOF)
                    is->auddec.finished = is->auddec.pkt_serial;
                return ret;
            }
            is->audio_buf_frames_pending = 1;
            tb = is->out_audio_filter->inputs[0]->time_base;
#endif

            data_size = av_samples_get_buffer_size(NULL, av_frame_get_channels(is->frame),
                                                   is->frame->nb_samples,
                                                   is->frame->format, 1);

            dec_channel_layout =
                (is->frame->channel_layout && av_frame_get_channels(is->frame) == av_get_channel_layout_nb_channels(is->frame->channel_layout)) ?
                is->frame->channel_layout : av_get_default_channel_layout(av_frame_get_channels(is->frame));
            wanted_nb_samples = synchronize_audio(is, is->frame->nb_samples);

            if (is->frame->format        != is->audio_src.fmt            ||
                dec_channel_layout       != is->audio_src.channel_layout ||
                is->frame->sample_rate   != is->audio_src.freq           ||
                (wanted_nb_samples       != is->frame->nb_samples && !is->swr_ctx)) {
                swr_free(&is->swr_ctx);
                is->swr_ctx = swr_alloc_set_opts(NULL,
                                                 is->audio_tgt.channel_layout, is->audio_tgt.fmt, is->audio_tgt.freq,
                                                 dec_channel_layout,           is->frame->format, is->frame->sample_rate,
                                                 0, NULL);
                if (!is->swr_ctx || swr_init(is->swr_ctx) < 0) {
                    av_log(NULL, AV_LOG_ERROR,
                           "Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
                            is->frame->sample_rate, av_get_sample_fmt_name(is->frame->format), av_frame_get_channels(is->frame),
                            is->audio_tgt.freq, av_get_sample_fmt_name(is->audio_tgt.fmt), is->audio_tgt.channels);
                    break;
                }
                is->audio_src.channel_layout = dec_channel_layout;
                is->audio_src.channels       = av_frame_get_channels(is->frame);
                is->audio_src.freq = is->frame->sample_rate;
                is->audio_src.fmt = is->frame->format;
            }

            if (is->swr_ctx) {
                const uint8_t **in = (const uint8_t **)is->frame->extended_data;
                uint8_t **out = &is->audio_buf1;
                int out_count = (int64_t)wanted_nb_samples * is->audio_tgt.freq / is->frame->sample_rate + 256;
                int out_size  = av_samples_get_buffer_size(NULL, is->audio_tgt.channels, out_count, is->audio_tgt.fmt, 0);
                int len2;
                if (out_size < 0) {
                    av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
                    break;
                }
                if (wanted_nb_samples != is->frame->nb_samples) {
                    if (swr_set_compensation(is->swr_ctx, (wanted_nb_samples - is->frame->nb_samples) * is->audio_tgt.freq / is->frame->sample_rate,
                                                wanted_nb_samples * is->audio_tgt.freq / is->frame->sample_rate) < 0) {
                        av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
                        break;
                    }
                }
                av_fast_malloc(&is->audio_buf1, &is->audio_buf1_size, out_size);
                if (!is->audio_buf1)
                    return AVERROR(ENOMEM);
                len2 = swr_convert(is->swr_ctx, out, out_count, in, is->frame->nb_samples);
                if (len2 < 0) {
                    av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
                    break;
                }
                if (len2 == out_count) {
                    av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
                    swr_init(is->swr_ctx);
                }
                is->audio_buf = is->audio_buf1;
                resampled_data_size = len2 * is->audio_tgt.channels * av_get_bytes_per_sample(is->audio_tgt.fmt);
            } else {
                is->audio_buf = is->frame->data[0];
                resampled_data_size = data_size;
            }

            audio_clock0 = is->audio_clock;
            /
            if (is->frame->pts != AV_NOPTS_VALUE)
                is->audio_clock = is->frame->pts * av_q2d(tb) + (double) is->frame->nb_samples / is->frame->sample_rate;
            else
                is->audio_clock = NAN;
            is->audio_clock_serial = is->auddec.pkt_serial;
#ifdef DEBUG
            {
                static double last_clock;
                printf("audio: delay=%0.3f clock=%0.3f clock0=%0.3f\n",
                       is->audio_clock - last_clock,
                       is->audio_clock, audio_clock0);
                last_clock = is->audio_clock;
            }
#endif
            return resampled_data_size;
        }

        if ((got_frame = decoder_decode_frame(&is->auddec, is->frame)) < 0)
            return -1;

        if (is->auddec.flushed)
            is->audio_buf_frames_pending = 0;
    }
}
