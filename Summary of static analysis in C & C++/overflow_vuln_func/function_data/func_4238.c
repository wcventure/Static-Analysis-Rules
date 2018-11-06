static void new_audio_stream(AVFormatContext *oc)
{
    AVStream *st;
    AVCodec *codec= NULL;
    AVCodecContext *audio_enc;
    enum CodecID codec_id;

    st = av_new_stream(oc, oc->nb_streams < nb_streamid_map ? streamid_map[oc->nb_streams] : 0);
    if (!st) {
        fprintf(stderr, "Could not alloc stream\n");
        ffmpeg_exit(1);
    }

    output_codecs = grow_array(output_codecs, sizeof(*output_codecs), &nb_output_codecs, nb_output_codecs + 1);
    if(!audio_stream_copy){
        if (audio_codec_name) {
            codec_id = find_codec_or_die(audio_codec_name, AVMEDIA_TYPE_AUDIO, 1,
                                         avcodec_opts[AVMEDIA_TYPE_AUDIO]->strict_std_compliance);
            codec = avcodec_find_encoder_by_name(audio_codec_name);
            output_codecs[nb_output_codecs-1] = codec;
        } else {
            codec_id = av_guess_codec(oc->oformat, NULL, oc->filename, NULL, AVMEDIA_TYPE_AUDIO);
            codec = avcodec_find_encoder(codec_id);
        }
    }

    avcodec_get_context_defaults3(st->codec, codec);

    bitstream_filters[nb_output_files] =
        grow_array(bitstream_filters[nb_output_files],
                   sizeof(*bitstream_filters[nb_output_files]),
                   &nb_bitstream_filters[nb_output_files], oc->nb_streams);
    bitstream_filters[nb_output_files][oc->nb_streams - 1]= audio_bitstream_filters;
    audio_bitstream_filters= NULL;

    avcodec_thread_init(st->codec, thread_count);

    audio_enc = st->codec;
    audio_enc->codec_type = AVMEDIA_TYPE_AUDIO;

    if(audio_codec_tag)
        audio_enc->codec_tag= audio_codec_tag;

    if (oc->oformat->flags & AVFMT_GLOBALHEADER) {
        audio_enc->flags |= CODEC_FLAG_GLOBAL_HEADER;
        avcodec_opts[AVMEDIA_TYPE_AUDIO]->flags|= CODEC_FLAG_GLOBAL_HEADER;
    }
    if (audio_stream_copy) {
        st->stream_copy = 1;
        audio_enc->channels = audio_channels;
        audio_enc->sample_rate = audio_sample_rate;
    } else {
        audio_enc->codec_id = codec_id;
        set_context_opts(audio_enc, avcodec_opts[AVMEDIA_TYPE_AUDIO], AV_OPT_FLAG_AUDIO_PARAM | AV_OPT_FLAG_ENCODING_PARAM, codec);

        if (audio_qscale > QSCALE_NONE) {
            audio_enc->flags |= CODEC_FLAG_QSCALE;
            audio_enc->global_quality = st->quality = FF_QP2LAMBDA * audio_qscale;
        }
        audio_enc->channels = audio_channels;
        audio_enc->sample_fmt = audio_sample_fmt;
        audio_enc->sample_rate = audio_sample_rate;
        audio_enc->channel_layout = channel_layout;
        if (avcodec_channel_layout_num_channels(channel_layout) != audio_channels)
            audio_enc->channel_layout = 0;
        choose_sample_fmt(st, codec);
        choose_sample_rate(st, codec);
    }
    audio_enc->time_base= (AVRational){1, audio_sample_rate};
    if (audio_language) {
        av_metadata_set2(&st->metadata, "language", audio_language, 0);
        av_freep(&audio_language);
    }

    /
    audio_disable = 0;
    av_freep(&audio_codec_name);
    audio_stream_copy = 0;
}
