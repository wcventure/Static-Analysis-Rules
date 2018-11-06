static int configure_output_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out)
{
    char *pix_fmts;
    AVCodecContext *codec = ofilter->ost->st->codec;
    AVFilterContext *last_filter = out->filter_ctx;
    int pad_idx = out->pad_idx;
    int ret;


    ret = avfilter_graph_create_filter(&ofilter->filter,
                                       avfilter_get_by_name("buffersink"),
                                       "out", NULL, pix_fmts, fg->graph);
    if (ret < 0)
        return ret;

    if (codec->width || codec->height) {
        char args[255];
        AVFilterContext *filter;

        snprintf(args, sizeof(args), "%d:%d:flags=0x%X",
                 codec->width,
                 codec->height,
                 (unsigned)ofilter->ost->sws_flags);
        if ((ret = avfilter_graph_create_filter(&filter, avfilter_get_by_name("scale"),
                                                NULL, args, NULL, fg->graph)) < 0)
            return ret;
        if ((ret = avfilter_link(last_filter, pad_idx, filter, 0)) < 0)
            return ret;

        last_filter = filter;
        pad_idx = 0;
    }

    if ((pix_fmts = choose_pixel_fmts(ofilter->ost))) {
        AVFilterContext *filter;
        if ((ret = avfilter_graph_create_filter(&filter,
                                                avfilter_get_by_name("format"),
                                                "format", pix_fmts, NULL,
                                                fg->graph)) < 0)
            return ret;
        if ((ret = avfilter_link(last_filter, pad_idx, filter, 0)) < 0)
            return ret;

        last_filter = filter;
        pad_idx     = 0;
        av_freep(&pix_fmts);
    }

    if ((ret = avfilter_link(last_filter, pad_idx, ofilter->filter, 0)) < 0)
        return ret;

    return 0;
}
