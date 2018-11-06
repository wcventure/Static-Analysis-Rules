static int init_input_stream(int ist_index, char *error, int error_len)
{
    int i;
    InputStream *ist = input_streams[ist_index];
    if (ist->decoding_needed) {
        AVCodec *codec = ist->dec;
        if (!codec) {
            snprintf(error, error_len, "Decoder (codec id %d) not found for input stream #%d:%d",
                    ist->st->codec->codec_id, ist->file_index, ist->st->index);
            return AVERROR(EINVAL);
        }

        /
        for (i = 0; i < nb_output_streams; i++) {
            OutputStream *ost = output_streams[i];
            if (ost->source_index == ist_index) {
                update_sample_fmt(ist->st->codec, codec, ost->st->codec);
                break;
            }
        }

        if (codec->type == AVMEDIA_TYPE_VIDEO && codec->capabilities & CODEC_CAP_DR1) {
            ist->st->codec->get_buffer     = codec_get_buffer;
            ist->st->codec->release_buffer = codec_release_buffer;
            ist->st->codec->opaque         = ist;
        }

        if (!av_dict_get(ist->opts, "threads", NULL, 0))
            av_dict_set(&ist->opts, "threads", "auto", 0);
        if (avcodec_open2(ist->st->codec, codec, &ist->opts) < 0) {
            snprintf(error, error_len, "Error while opening decoder for input stream #%d:%d",
                    ist->file_index, ist->st->index);
            return AVERROR(EINVAL);
        }
        assert_codec_experimental(ist->st->codec, 0);
        assert_avoptions(ist->opts);

        if (ist->st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            for (i = 0; i < nb_output_streams; i++) {
                OutputStream *ost = output_streams[i];
                if (ost->source_index == ist_index) {
                    if (!ist->st->codec->channel_layout || !ost->st->codec->channel_layout)
                        get_default_channel_layouts(ost, ist);
                    break;
                }
            }
        }
    }

    ist->last_dts = ist->st->avg_frame_rate.num ? - ist->st->codec->has_b_frames * AV_TIME_BASE / av_q2d(ist->st->avg_frame_rate) : 0;
    ist->next_dts = AV_NOPTS_VALUE;
    init_pts_correction(&ist->pts_ctx);
    ist->is_start = 1;

    return 0;
}
