static void new_subtitle_stream(AVFormatContext *oc)
{
    AVStream *st;
    AVCodec *codec=NULL;
    AVCodecContext *subtitle_enc;

    st = av_new_stream(oc, oc->nb_streams < nb_streamid_map ? streamid_map[oc->nb_streams] : 0);
    if (!st) {
        fprintf(stderr, "Could not alloc stream\n");
        ffmpeg_exit(1);
    }
    subtitle_enc = st->codec;
    output_codecs = grow_array(output_codecs, sizeof(*output_codecs), &nb_output_codecs, nb_output_codecs + 1);
    if(!subtitle_stream_copy){
        subtitle_enc->codec_id = find_codec_or_die(subtitle_codec_name, AVMEDIA_TYPE_SUBTITLE, 1,
                                                   avcodec_opts[AVMEDIA_TYPE_SUBTITLE]->strict_std_compliance);
        codec= output_codecs[nb_output_codecs-1] = avcodec_find_encoder_by_name(subtitle_codec_name);
    }
    avcodec_get_context_defaults3(st->codec, codec);

    bitstream_filters[nb_output_files] =
        grow_array(bitstream_filters[nb_output_files],
                   sizeof(*bitstream_filters[nb_output_files]),
                   &nb_bitstream_filters[nb_output_files], oc->nb_streams);
    bitstream_filters[nb_output_files][oc->nb_streams - 1]= subtitle_bitstream_filters;
    subtitle_bitstream_filters= NULL;

    subtitle_enc->codec_type = AVMEDIA_TYPE_SUBTITLE;

    if(subtitle_codec_tag)
        subtitle_enc->codec_tag= subtitle_codec_tag;

    if (subtitle_stream_copy) {
        st->stream_copy = 1;
    } else {
        set_context_opts(avcodec_opts[AVMEDIA_TYPE_SUBTITLE], subtitle_enc, AV_OPT_FLAG_SUBTITLE_PARAM | AV_OPT_FLAG_ENCODING_PARAM, codec);
    }

    if (subtitle_language) {
        av_metadata_set2(&st->metadata, "language", subtitle_language, 0);
        av_freep(&subtitle_language);
    }

    subtitle_disable = 0;
    av_freep(&subtitle_codec_name);
    subtitle_stream_copy = 0;
}
