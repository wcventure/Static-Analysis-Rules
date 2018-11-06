int av_grab(AVFormatContext *s)
{
    UINT8 audio_buf[AUDIO_FIFO_SIZE/2];
    UINT8 audio_buf1[AUDIO_FIFO_SIZE/2];
    UINT8 audio_out[AUDIO_FIFO_SIZE/2];
    UINT8 video_buffer[128*1024];
    char buf[256];
    short *samples;
    URLContext *audio_handle = NULL, *video_handle = NULL;
    int ret;
    AVCodecContext *enc, *first_video_enc = NULL;
    int frame_size, frame_bytes;
    int use_audio, use_video;
    int frame_rate, sample_rate, channels;
    int width, height, frame_number, i, pix_fmt = 0;
    AVOutputStream *ost_table[s->nb_streams], *ost;
    UINT8 *picture_in_buf = NULL, *picture_420p = NULL;
    int audio_fifo_size = 0, picture_size = 0;
    INT64 time_start;

    /
    for(i=0;i<s->nb_streams;i++)
        ost_table[i] = NULL;

    /
    for(i=0;i<s->nb_streams;i++) {
        ost = av_mallocz(sizeof(AVOutputStream));
        if (!ost)
            goto fail;
        ost->index = i;
        ost->st = s->streams[i];
        ost_table[i] = ost;
    }

    use_audio = 0;
    use_video = 0;
    frame_rate = 0;
    sample_rate = 0;
    frame_size = 0;
    channels = 1;
    width = 0;
    height = 0;
    frame_number = 0;
    
    for(i=0;i<s->nb_streams;i++) {
        AVCodec *codec;

        ost = ost_table[i];
        enc = &ost->st->codec;
        codec = avcodec_find_encoder(enc->codec_id);
        if (!codec) {
            fprintf(stderr, "Unknown codec\n");
            return -1;
        }
        if (avcodec_open(enc, codec) < 0) {
            fprintf(stderr, "Incorrect encode parameters\n");
            return -1;
        }
        switch(enc->codec_type) {
        case CODEC_TYPE_AUDIO:
            use_audio = 1;
            if (enc->sample_rate > sample_rate)
                sample_rate = enc->sample_rate;
            if (enc->frame_size > frame_size)
                frame_size = enc->frame_size;
            if (enc->channels > channels)
                channels = enc->channels;
            break;
        case CODEC_TYPE_VIDEO:
            if (!first_video_enc)
                first_video_enc = enc;
            use_video = 1;
            if (enc->frame_rate > frame_rate)
                frame_rate = enc->frame_rate;
            if (enc->width > width)
                width = enc->width;
            if (enc->height > height)
                height = enc->height;
            break;
        }
    }

    /
    samples = NULL;
    if (use_audio) {
        snprintf(buf, sizeof(buf), "audio:%d,%d", sample_rate, channels);
        ret = url_open(&audio_handle, buf, URL_RDONLY);
        if (ret < 0) {
            fprintf(stderr, "Could not open audio device: disabling audio capture\n");
            use_audio = 0;
        } else {
            URLFormat f;
            /
            if (url_getformat(audio_handle, &f) < 0) {
                fprintf(stderr, "could not read back video grab parameters\n");
                goto fail;
            }
            sample_rate = f.sample_rate;
            channels = f.channels;
            audio_fifo_size = ((AUDIO_FIFO_SIZE / 2) / audio_handle->packet_size) * 
                audio_handle->packet_size;
            fprintf(stderr, "Audio sampling: %d Hz, %s\n", 
                    sample_rate, channels == 2 ? "stereo" : "mono");
        }
    }
    
    /
    if (use_video) {
        snprintf(buf, sizeof(buf), "video:%d,%d,%f", 
                 width, height, (float)frame_rate / FRAME_RATE_BASE);

        ret = url_open(&video_handle, buf, URL_RDONLY);
        if (ret < 0) {
            fprintf(stderr,"Could not init video 4 linux capture: disabling video capture\n");
            use_video = 0;
        } else {
            URLFormat f;
            const char *pix_fmt_str;
            /
            if (url_getformat(video_handle, &f) < 0) {
                fprintf(stderr, "could not read back video grab parameters\n");
                goto fail;
            }
            width = f.width;
            height = f.height;
            pix_fmt = f.pix_fmt;
            switch(pix_fmt) {
            case PIX_FMT_YUV420P:
                pix_fmt_str = "420P";
                break;
            case PIX_FMT_YUV422:
                pix_fmt_str = "422";
                break;
            case PIX_FMT_RGB24:
                pix_fmt_str = "RGB24";
                break;
            case PIX_FMT_BGR24:
                pix_fmt_str = "BGR24";
                break;
            default:
                pix_fmt_str = "???";
                break;
            }
            picture_size = video_handle->packet_size;
            picture_in_buf = malloc(picture_size);
            if (!picture_in_buf)
                goto fail;
            /
            if (pix_fmt != PIX_FMT_YUV420P) {
                picture_420p = malloc((width * height * 3) / 2);
            }
            fprintf(stderr, "Video sampling: %dx%d, %s format, %0.2f fps\n", 
                    width, height, pix_fmt_str, (float)frame_rate / FRAME_RATE_BASE);
        }
    }

    if (!use_video && !use_audio) {
        fprintf(stderr,"Could not open grab devices : exiting\n");
        exit(1);
    }

    /
    for(i=0;i<s->nb_streams;i++) {
        ost = ost_table[i];
        enc = &ost->st->codec;
        switch(enc->codec_type) {
        case CODEC_TYPE_AUDIO:
            ost->audio_resample = 0;
            if ((enc->channels != channels ||
                 enc->sample_rate != sample_rate)) {
                ost->audio_resample = 1;
                ost->resample = audio_resample_init(enc->channels, channels,
                                                    enc->sample_rate, sample_rate);
            }
            if (fifo_init(&ost->fifo, (2 * audio_fifo_size * enc->sample_rate) / 
                          sample_rate))
                goto fail;
            break;
        case CODEC_TYPE_VIDEO:
            ost->video_resample = 0;
            if (enc->width != width ||
                enc->height != height) {
                UINT8 *buf;
                ost->video_resample = 1;
                buf = malloc((enc->width * enc->height * 3) / 2);
                if (!buf)
                    goto fail;
                ost->pict_tmp.data[0] = buf;
                ost->pict_tmp.data[1] = buf + enc->width * height;
                ost->pict_tmp.data[2] = ost->pict_tmp.data[1] + (enc->width * height) / 4;
                ost->pict_tmp.linesize[0] = enc->width;
                ost->pict_tmp.linesize[1] = enc->width / 2;
                ost->pict_tmp.linesize[2] = enc->width / 2;
                ost->img_resample_ctx = img_resample_init(
                                  ost->st->codec.width, ost->st->codec.height,
                                  width, height);
            }
        }
    }

    fprintf(stderr, "Press [q] to stop encoding\n");

    s->format->write_header(s);
    time_start = gettime();
    term_init();
    
    for(;;) {
        /
        if (read_key() == 'q')
            break;

        /
        if (use_audio) {
            int ret, nb_samples, nb_samples_out;
            UINT8 *buftmp;

            for(;;) {
                ret = url_read(audio_handle, audio_buf, audio_fifo_size);
                if (ret <= 0)
                    break;
                /
                nb_samples = ret / (channels * 2);
                for(i=0;i<s->nb_streams;i++) {
                    ost = ost_table[i];
                    enc = &ost->st->codec;
                    if (enc->codec_type == CODEC_TYPE_AUDIO) {
                        /
                        if (!ost->audio_resample) {
                            buftmp = audio_buf;
                            nb_samples_out = nb_samples;
                        } else {
                            buftmp = audio_buf1;
                            nb_samples_out = audio_resample(ost->resample, 
                                                            (short *)buftmp, (short *)audio_buf,
                                                            nb_samples);
                        }
                        fifo_write(&ost->fifo, buftmp, nb_samples_out * enc->channels * 2, 
                                   &ost->fifo.wptr);
                    }
                }
                
                /
                for(i=0;i<s->nb_streams;i++) {
                    ost = ost_table[i];
                    enc = &ost->st->codec;
                    if (enc->codec_type == CODEC_TYPE_AUDIO) {
                        frame_bytes = enc->frame_size * 2 * enc->channels;
                        
                        while (fifo_read(&ost->fifo, audio_buf, 
                                         frame_bytes, &ost->fifo.rptr) == 0) {
                            ret = avcodec_encode_audio(enc,
                                                       audio_out, sizeof(audio_out), 
                                                       (short *)audio_buf);
                            s->format->write_packet(s, ost->index, audio_out, ret);
                        }
                    }
                }
            }
        }

        if (use_video) {
            AVPicture *picture1, *picture2, *picture;
            AVPicture picture_tmp0, picture_tmp1;

            ret = url_read(video_handle, picture_in_buf, picture_size);
            if (ret < 0)
                break;
            
            picture2 = &picture_tmp0;
            avpicture_fill(picture2, picture_in_buf, pix_fmt, width, height);

            if (pix_fmt != PIX_FMT_YUV420P) {
                picture = &picture_tmp1;
                img_convert(picture, PIX_FMT_YUV420P,
                            picture2, pix_fmt, 
                            width, height);
            } else {
                picture = picture2;
            }
            
            for(i=0;i<s->nb_streams;i++) {
                ost = ost_table[i];
                enc = &ost->st->codec;
                if (enc->codec_type == CODEC_TYPE_VIDEO) {
                    int n1, n2, nb;

                    /
                    n1 = ((INT64)frame_number * enc->frame_rate) / frame_rate;
                    n2 = (((INT64)frame_number + 1) * enc->frame_rate) / frame_rate;
                    nb = n2 - n1;
                    if (nb > 0) {
                        /
                        if (ost->video_resample) {
                            picture1 = &ost->pict_tmp;
                            img_resample(ost->img_resample_ctx, 
                                         picture1, picture);
                        } else {
                            picture1 = picture;
                        }
                        ret = avcodec_encode_video(enc, video_buffer, 
                                                   sizeof(video_buffer), 
                                                   picture1);
                        s->format->write_packet(s, ost->index, video_buffer, ret);
                    }
                }
            }
            frame_number++;
        }
        
        /
        {
            char buf[1024];
            INT64 total_size;
            float ti, bitrate;
            static float last_ti;
            INT64 ti1;

            total_size = url_ftell(&s->pb);
            ti1 = gettime() - time_start;
            /
            if (recording_time && ti1 >= recording_time)
                break;

            ti = ti1 / 1000000.0;
            if (ti < 0.1)
                ti = 0.1;
            /
            if ((ti - last_ti) >= 0.5) {
                last_ti = ti;
                bitrate = (int)((total_size * 8) / ti / 1000.0);
                
                buf[0] = '\0';
                if (use_video) {
                    sprintf(buf + strlen(buf), "frame=%5d fps=%4.1f q=%2d ",
                            frame_number, (float)frame_number / ti, first_video_enc->quality);
                }
                
                sprintf(buf + strlen(buf), "size=%8LdkB time=%0.1f bitrate=%6.1fkbits/s", 
                        total_size / 1024, ti, bitrate);
                fprintf(stderr, "%s    \r", buf);
                fflush(stderr);
            }
        }
    }
    term_exit();

    for(i=0;i<s->nb_streams;i++) {
        ost = ost_table[i];
        enc = &ost->st->codec;
        avcodec_close(enc);
    }
    s->format->write_trailer(s);
    
    if (audio_handle)
        url_close(audio_handle);

    if (video_handle)
        url_close(video_handle);

    /
    {
        float ti, bitrate;
        INT64 total_size;

        total_size = url_ftell(&s->pb);

        ti = (gettime() - time_start) / 1000000.0;
        if (ti < 0.1)
            ti = 0.1;
        bitrate = (int)((total_size * 8) / ti / 1000.0);

        fprintf(stderr, "\033[K\nTotal time = %0.1f s, %Ld KBytes, %0.1f kbits/s\n", 
                ti, total_size / 1024, bitrate);
        if (use_video) {
            fprintf(stderr, "Total frames = %d\n", frame_number);
        }
    }

    ret = 0;
 fail1:
    if (picture_in_buf)
        free(picture_in_buf);
    if (picture_420p)
        free(picture_420p);
    for(i=0;i<s->nb_streams;i++) {
        ost = ost_table[i];
        if (ost) {
            if (ost->fifo.buffer)
                fifo_free(&ost->fifo);
            if (ost->pict_tmp.data[0])
                free(ost->pict_tmp.data[0]);
            if (ost->video_resample)
                img_resample_close(ost->img_resample_ctx);
            if (ost->audio_resample)
                audio_resample_close(ost->resample);
            free(ost);
        }
    }
    return ret;
 fail:
    ret = -ENOMEM;
    goto fail1;
}
