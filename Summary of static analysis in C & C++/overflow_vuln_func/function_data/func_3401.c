static int configure_video_filters(FilterGraph *fg)
{
    InputStream  *ist = fg->inputs[0]->ist;
    OutputStream *ost = fg->outputs[0]->ost;
    AVFilterContext *in_filter, *out_filter, *filter;
    AVCodecContext *codec = ost->st->codec;
    AVBufferSinkParams *buffersink_params = av_buffersink_params_alloc();
    char *pix_fmts;
    AVRational sample_aspect_ratio;
    char args[255];
    int ret;

    avfilter_graph_free(&fg->graph);
    fg->graph = avfilter_graph_alloc();
    if (!fg->graph)
        return AVERROR(ENOMEM);

    if (ist->st->sample_aspect_ratio.num) {
        sample_aspect_ratio = ist->st->sample_aspect_ratio;
    } else
        sample_aspect_ratio = ist->st->codec->sample_aspect_ratio;

    snprintf(args, 255, "%d:%d:%d:%d:%d:%d:%d:flags=%d", ist->st->codec->width,
             ist->st->codec->height, ist->st->codec->pix_fmt, 1, AV_TIME_BASE,
             sample_aspect_ratio.num, sample_aspect_ratio.den, SWS_BILINEAR + ((ist->st->codec->flags&CODEC_FLAG_BITEXACT) ? SWS_BITEXACT:0));

    ret = avfilter_graph_create_filter(&fg->inputs[0]->filter,
                                       avfilter_get_by_name("buffer"),
                                       "src", args, NULL, fg->graph);
    if (ret < 0)
        return ret;

#if FF_API_OLD_VSINK_API
    ret = avfilter_graph_create_filter(&fg->outputs[0]->filter,
                                       avfilter_get_by_name("buffersink"),
                                       "out", NULL, NULL, fg->graph);
#else
    ret = avfilter_graph_create_filter(&fg->outputs[0]->filter,
                                       avfilter_get_by_name("buffersink"),
                                       "out", NULL, buffersink_params, fg->graph);
#endif
    av_freep(&buffersink_params);

    if (ret < 0)
        return ret;
    in_filter  = fg->inputs[0]->filter;
    out_filter = fg->outputs[0]->filter;

    if (codec->width || codec->height) {
        snprintf(args, 255, "%d:%d:flags=0x%X",
                 codec->width,
                 codec->height,
                 (unsigned)ost->sws_flags);
        if ((ret = avfilter_graph_create_filter(&filter, avfilter_get_by_name("scale"),
                                                NULL, args, NULL, fg->graph)) < 0)
            return ret;
        if ((ret = avfilter_link(in_filter, 0, filter, 0)) < 0)
            return ret;
        in_filter = filter;
    }

    if ((pix_fmts = choose_pixel_fmts(ost))) {
        if ((ret = avfilter_graph_create_filter(&filter,
                                                avfilter_get_by_name("format"),
                                                "format", pix_fmts, NULL,
                                                fg->graph)) < 0)
            return ret;
        if ((ret = avfilter_link(filter, 0, out_filter, 0)) < 0)
            return ret;

        out_filter = filter;
        av_freep(&pix_fmts);
    }

    snprintf(args, sizeof(args), "flags=0x%X", (unsigned)ost->sws_flags);
    fg->graph->scale_sws_opts = av_strdup(args);

    if (ost->avfilter) {
        AVFilterInOut *outputs = avfilter_inout_alloc();
        AVFilterInOut *inputs  = avfilter_inout_alloc();

        outputs->name    = av_strdup("in");
        outputs->filter_ctx = in_filter;
        outputs->pad_idx = 0;
        outputs->next    = NULL;

        inputs->name    = av_strdup("out");
        inputs->filter_ctx = out_filter;
        inputs->pad_idx = 0;
        inputs->next    = NULL;

        if ((ret = avfilter_graph_parse(fg->graph, ost->avfilter, &inputs, &outputs, NULL)) < 0)
            return ret;
        av_freep(&ost->avfilter);
    } else {
        if ((ret = avfilter_link(in_filter, 0, out_filter, 0)) < 0)
            return ret;
    }

    if (ost->keep_pix_fmt)
        avfilter_graph_set_auto_convert(fg->graph,
                                        AVFILTER_AUTO_CONVERT_NONE);

    if ((ret = avfilter_graph_config(fg->graph, NULL)) < 0)
        return ret;

    ost->filter = fg->outputs[0];

    return 0;
}
