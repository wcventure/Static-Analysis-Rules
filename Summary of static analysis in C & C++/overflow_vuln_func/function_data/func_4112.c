static void init_input_filter(FilterGraph *fg, AVFilterInOut *in)
{
    InputStream *ist;
    enum AVMediaType type = in->filter_ctx->input_pads[in->pad_idx].type;
    int i;

    // TODO: support other filter types
    if (type != AVMEDIA_TYPE_VIDEO) {
        av_log(NULL, AV_LOG_FATAL, "Only video filters supported currently.\n");
        exit_program(1);
    }

    if (in->name) {
        AVFormatContext *s;
        AVStream       *st = NULL;
        char *p;
        int file_idx = strtol(in->name, &p, 0);

        if (file_idx < 0 || file_idx > nb_input_files) {
            av_log(NULL, AV_LOG_FATAL, "Invalid file index %d in filtegraph description %s.\n",
                   file_idx, fg->graph_desc);
            exit_program(1);
        }
        s = input_files[file_idx]->ctx;

        for (i = 0; i < s->nb_streams; i++) {
            if (s->streams[i]->codec->codec_type != type)
                continue;
            if (check_stream_specifier(s, s->streams[i], *p == ':' ? p + 1 : p) == 1) {
                st = s->streams[i];
                break;
            }
        }
        if (!st) {
            av_log(NULL, AV_LOG_FATAL, "Stream specifier '%s' in filtergraph description %s "
                   "matches no streams.\n", p, fg->graph_desc);
            exit_program(1);
        }
        ist = input_streams[input_files[file_idx]->ist_index + st->index];
    } else {
        /
        for (i = 0; i < nb_input_streams; i++) {
            ist = input_streams[i];
            if (ist->st->codec->codec_type == type && ist->discard)
                break;
        }
        if (i == nb_input_streams) {
            av_log(NULL, AV_LOG_FATAL, "Cannot find a matching stream for "
                   "unlabeled input pad %d on filter %s", in->pad_idx,
                   in->filter_ctx->name);
            exit_program(1);
        }
    }
    ist->discard         = 0;
    ist->decoding_needed = 1;
    ist->st->discard = AVDISCARD_NONE;

    fg->inputs = grow_array(fg->inputs, sizeof(*fg->inputs),
                            &fg->nb_inputs, fg->nb_inputs + 1);
    if (!(fg->inputs[fg->nb_inputs - 1] = av_mallocz(sizeof(*fg->inputs[0]))))
        exit_program(1);
    fg->inputs[fg->nb_inputs - 1]->ist   = ist;
    fg->inputs[fg->nb_inputs - 1]->graph = fg;

    ist->filters = grow_array(ist->filters, sizeof(*ist->filters),
                              &ist->nb_filters, ist->nb_filters + 1);
    ist->filters[ist->nb_filters - 1] = fg->inputs[fg->nb_inputs - 1];
}
