static OutputStream *new_audio_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
    int n;
    AVStream *st;
    OutputStream *ost;
    AVCodecContext *audio_enc;

    ost = new_output_stream(o, oc, AVMEDIA_TYPE_AUDIO, source_index);
    st  = ost->st;

    audio_enc = st->codec;
    audio_enc->codec_type = AVMEDIA_TYPE_AUDIO;

    if (!ost->stream_copy) {
        char *sample_fmt = NULL;

        MATCH_PER_STREAM_OPT(audio_channels, i, audio_enc->channels, oc, st);

        MATCH_PER_STREAM_OPT(sample_fmts, str, sample_fmt, oc, st);
        if (sample_fmt &&
            (audio_enc->sample_fmt = av_get_sample_fmt(sample_fmt)) == AV_SAMPLE_FMT_NONE) {
            av_log(NULL, AV_LOG_FATAL, "Invalid sample format '%s'\n", sample_fmt);
            exit_program(1);
        }

        MATCH_PER_STREAM_OPT(audio_sample_rate, i, audio_enc->sample_rate, oc, st);

        ost->rematrix_volume=1.0;
        MATCH_PER_STREAM_OPT(rematrix_volume, f, ost->rematrix_volume, oc, st);
    }

    /
    for (n = 0; n < o->nb_audio_channel_maps; n++) {
        AudioChannelMap *map = &o->audio_channel_maps[n];
        InputStream *ist = input_streams[ost->source_index];
        if ((map->channel_idx == -1 || (ist->file_index == map->file_idx && ist->st->index == map->stream_idx)) &&
            (map->ofile_idx   == -1 || ost->file_index == map->ofile_idx) &&
            (map->ostream_idx == -1 || ost->st->index  == map->ostream_idx)) {
            if (ost->audio_channels_mapped < FF_ARRAY_ELEMS(ost->audio_channels_map))
                ost->audio_channels_map[ost->audio_channels_mapped++] = map->channel_idx;
            else
                av_log(NULL, AV_LOG_FATAL, "Max channel mapping for output %d.%d reached\n",
                       ost->file_index, ost->st->index);
        }
    }

    return ost;
}
