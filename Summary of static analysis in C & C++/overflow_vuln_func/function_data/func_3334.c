static void do_audio_out(AVFormatContext *s,
                         AVOutputStream *ost,
                         AVInputStream *ist,
                         unsigned char *buf, int size)
{
    uint8_t *buftmp;
    int64_t audio_out_size, audio_buf_size;

    int size_out, frame_bytes, ret;
    AVCodecContext *enc= ost->st->codec;
    AVCodecContext *dec= ist->st->codec;
    int osize= av_get_bits_per_sample_format(enc->sample_fmt)/8;
    int isize= av_get_bits_per_sample_format(dec->sample_fmt)/8;
    const int coded_bps = av_get_bits_per_sample(enc->codec->id);

    audio_buf_size= (size + isize*dec->channels - 1) / (isize*dec->channels);
    audio_buf_size= (audio_buf_size*enc->sample_rate + dec->sample_rate) / dec->sample_rate;
    audio_buf_size= audio_buf_size*2 + 10000; //safety factors for the deprecated resampling API
    audio_buf_size*= osize*enc->channels;

    audio_out_size= FFMAX(audio_buf_size, enc->frame_size * osize * enc->channels);
    if(coded_bps > 8*osize)
        audio_out_size= audio_out_size * coded_bps / (8*osize);
    audio_out_size += FF_MIN_BUFFER_SIZE;

    if(audio_out_size > INT_MAX || audio_buf_size > INT_MAX){
        fprintf(stderr, "Buffer sizes too large\n");
        av_exit(1);
    }

    av_fast_malloc(&audio_buf, &allocated_audio_buf_size, audio_buf_size);
    av_fast_malloc(&audio_out, &allocated_audio_out_size, audio_out_size);
    if (!audio_buf || !audio_out){
        fprintf(stderr, "Out of memory in do_audio_out\n");
        av_exit(1);
    }

    if (enc->channels != dec->channels)
        ost->audio_resample = 1;

    if (ost->audio_resample && !ost->resample) {
        if (dec->sample_fmt != SAMPLE_FMT_S16)
            fprintf(stderr, "Warning, using s16 intermediate sample format for resampling\n");
        ost->resample = av_audio_resample_init(enc->channels,    dec->channels,
                                               enc->sample_rate, dec->sample_rate,
                                               enc->sample_fmt,  dec->sample_fmt,
                                               16, 10, 0, 0.8);
        if (!ost->resample) {
            fprintf(stderr, "Can not resample %d channels @ %d Hz to %d channels @ %d Hz\n",
                    dec->channels, dec->sample_rate,
                    enc->channels, enc->sample_rate);
            av_exit(1);
        }
    }

#define MAKE_SFMT_PAIR(a,b) ((a)+SAMPLE_FMT_NB*(b))
    if (!ost->audio_resample && dec->sample_fmt!=enc->sample_fmt &&
        MAKE_SFMT_PAIR(enc->sample_fmt,dec->sample_fmt)!=ost->reformat_pair) {
        if (ost->reformat_ctx)
            av_audio_convert_free(ost->reformat_ctx);
        ost->reformat_ctx = av_audio_convert_alloc(enc->sample_fmt, 1,
                                                   dec->sample_fmt, 1, NULL, 0);
        if (!ost->reformat_ctx) {
            fprintf(stderr, "Cannot convert %s sample format to %s sample format\n",
                avcodec_get_sample_fmt_name(dec->sample_fmt),
                avcodec_get_sample_fmt_name(enc->sample_fmt));
            av_exit(1);
        }
        ost->reformat_pair=MAKE_SFMT_PAIR(enc->sample_fmt,dec->sample_fmt);
    }

    if(audio_sync_method){
        double delta = get_sync_ipts(ost) * enc->sample_rate - ost->sync_opts
                - av_fifo_size(ost->fifo)/(ost->st->codec->channels * 2);
        double idelta= delta*ist->st->codec->sample_rate / enc->sample_rate;
        int byte_delta= ((int)idelta)*2*ist->st->codec->channels;

        //FIXME resample delay
        if(fabs(delta) > 50){
            if(ist->is_start || fabs(delta) > audio_drift_threshold*enc->sample_rate){
                if(byte_delta < 0){
                    byte_delta= FFMAX(byte_delta, -size);
                    size += byte_delta;
                    buf  -= byte_delta;
                    if(verbose > 2)
                        fprintf(stderr, "discarding %d audio samples\n", (int)-delta);
                    if(!size)
                        return;
                    ist->is_start=0;
                }else{
                    static uint8_t *input_tmp= NULL;
                    input_tmp= av_realloc(input_tmp, byte_delta + size);

                    if(byte_delta + size <= MAX_AUDIO_PACKET_SIZE)
                        ist->is_start=0;
                    else
                        byte_delta= MAX_AUDIO_PACKET_SIZE - size;

                    memset(input_tmp, 0, byte_delta);
                    memcpy(input_tmp + byte_delta, buf, size);
                    buf= input_tmp;
                    size += byte_delta;
                    if(verbose > 2)
                        fprintf(stderr, "adding %d audio samples of silence\n", (int)delta);
                }
            }else if(audio_sync_method>1){
                int comp= av_clip(delta, -audio_sync_method, audio_sync_method);
                assert(ost->audio_resample);
                if(verbose > 2)
                    fprintf(stderr, "compensating audio timestamp drift:%f compensation:%d in:%d\n", delta, comp, enc->sample_rate);
//                fprintf(stderr, "drift:%f len:%d opts:%"PRId64" ipts:%"PRId64" fifo:%d\n", delta, -1, ost->sync_opts, (int64_t)(get_sync_ipts(ost) * enc->sample_rate), av_fifo_size(ost->fifo)/(ost->st->codec->channels * 2));
                av_resample_compensate(*(struct AVResampleContext**)ost->resample, comp, enc->sample_rate);
            }
        }
    }else
        ost->sync_opts= lrintf(get_sync_ipts(ost) * enc->sample_rate)
                        - av_fifo_size(ost->fifo)/(ost->st->codec->channels * 2); //FIXME wrong

    if (ost->audio_resample) {
        buftmp = audio_buf;
        size_out = audio_resample(ost->resample,
                                  (short *)buftmp, (short *)buf,
                                  size / (ist->st->codec->channels * isize));
        size_out = size_out * enc->channels * osize;
    } else {
        buftmp = buf;
        size_out = size;
    }

    if (!ost->audio_resample && dec->sample_fmt!=enc->sample_fmt) {
        const void *ibuf[6]= {buftmp};
        void *obuf[6]= {audio_buf};
        int istride[6]= {isize};
        int ostride[6]= {osize};
        int len= size_out/istride[0];
        if (av_audio_convert(ost->reformat_ctx, obuf, ostride, ibuf, istride, len)<0) {
            printf("av_audio_convert() failed\n");
            if (exit_on_error)
                av_exit(1);
            return;
        }
        buftmp = audio_buf;
        size_out = len*osize;
    }

    /
    if (enc->frame_size > 1) {
        /
        if (av_fifo_realloc2(ost->fifo, av_fifo_size(ost->fifo) + size_out) < 0) {
            fprintf(stderr, "av_fifo_realloc2() failed\n");
            av_exit(1);
        }
        av_fifo_generic_write(ost->fifo, buftmp, size_out, NULL);

        frame_bytes = enc->frame_size * osize * enc->channels;

        while (av_fifo_size(ost->fifo) >= frame_bytes) {
            AVPacket pkt;
            av_init_packet(&pkt);

            av_fifo_generic_read(ost->fifo, audio_buf, frame_bytes, NULL);

            //FIXME pass ost->sync_opts as AVFrame.pts in avcodec_encode_audio()

            ret = avcodec_encode_audio(enc, audio_out, audio_out_size,
                                       (short *)audio_buf);
            if (ret < 0) {
                fprintf(stderr, "Audio encoding failed\n");
                av_exit(1);
            }
            audio_size += ret;
            pkt.stream_index= ost->index;
            pkt.data= audio_out;
            pkt.size= ret;
            if(enc->coded_frame && enc->coded_frame->pts != AV_NOPTS_VALUE)
                pkt.pts= av_rescale_q(enc->coded_frame->pts, enc->time_base, ost->st->time_base);
            pkt.flags |= PKT_FLAG_KEY;
            write_frame(s, &pkt, ost->st->codec, bitstream_filters[ost->file_index][pkt.stream_index]);

            ost->sync_opts += enc->frame_size;
        }
    } else {
        AVPacket pkt;
        av_init_packet(&pkt);

        ost->sync_opts += size_out / (osize * enc->channels);

        /
        /
        size_out /= osize;
        if (coded_bps)
            size_out = size_out*coded_bps/8;

        if(size_out > audio_out_size){
            fprintf(stderr, "Internal error, buffer size too small\n");
            av_exit(1);
        }

        //FIXME pass ost->sync_opts as AVFrame.pts in avcodec_encode_audio()
        ret = avcodec_encode_audio(enc, audio_out, size_out,
                                   (short *)buftmp);
        if (ret < 0) {
            fprintf(stderr, "Audio encoding failed\n");
            av_exit(1);
        }
        audio_size += ret;
        pkt.stream_index= ost->index;
        pkt.data= audio_out;
        pkt.size= ret;
        if(enc->coded_frame && enc->coded_frame->pts != AV_NOPTS_VALUE)
            pkt.pts= av_rescale_q(enc->coded_frame->pts, enc->time_base, ost->st->time_base);
        pkt.flags |= PKT_FLAG_KEY;
        write_frame(s, &pkt, ost->st->codec, bitstream_filters[ost->file_index][pkt.stream_index]);
    }
}
