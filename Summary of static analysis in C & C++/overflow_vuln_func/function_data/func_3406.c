static void get_default_channel_layouts(OutputStream *ost, InputStream *ist)
{
    char layout_name[256];
    AVCodecContext *enc = ost->st->codec;
    AVCodecContext *dec = ist->st->codec;

    if (dec->channel_layout &&
        av_get_channel_layout_nb_channels(dec->channel_layout) != dec->channels) {
        av_get_channel_layout_string(layout_name, sizeof(layout_name),
                                     dec->channels, dec->channel_layout);
        av_log(NULL, AV_LOG_ERROR, "New channel layout (%s) is invalid\n",
               layout_name);
        dec->channel_layout = 0;
    }
    if (!dec->channel_layout) {
        if (enc->channel_layout && dec->channels == enc->channels) {
            dec->channel_layout = enc->channel_layout;
        } else {
            dec->channel_layout = av_get_default_channel_layout(dec->channels);

            if (!dec->channel_layout) {
                av_log(NULL, AV_LOG_FATAL, "Unable to find default channel "
                       "layout for Input Stream #%d.%d\n", ist->file_index,
                       ist->st->index);
                exit_program(1);
            }
        }
        av_get_channel_layout_string(layout_name, sizeof(layout_name),
                                     dec->channels, dec->channel_layout);
        av_log(NULL, AV_LOG_WARNING, "Guessed Channel Layout for  Input Stream "
               "#%d.%d : %s\n", ist->file_index, ist->st->index, layout_name);
    }
    if (!enc->channel_layout) {
        if (dec->channels == enc->channels) {
            enc->channel_layout = dec->channel_layout;
            return;
        } else {
            enc->channel_layout = av_get_default_channel_layout(enc->channels);
        }
        if (!enc->channel_layout) {
            av_log(NULL, AV_LOG_FATAL, "Unable to find default channel layout "
                   "for Output Stream #%d.%d\n", ost->file_index,
                   ost->st->index);
            exit_program(1);
        }
        av_get_channel_layout_string(layout_name, sizeof(layout_name),
                                     enc->channels, enc->channel_layout);
        av_log(NULL, AV_LOG_WARNING, "Guessed Channel Layout for Output Stream "
               "#%d.%d : %s\n", ost->file_index, ost->st->index, layout_name);
    }
}
