static av_cold int init(AVFilterContext *ctx)
{
    HDCDContext *s = ctx->priv;
    int c;

    s->sample_count = 0;
    s->fctx = ctx;
    s->bad_config = 0;

    hdcd_detect_reset(&s->detect);
    for (c = 0; c < HDCD_MAX_CHANNELS; c++) {
        hdcd_reset(&s->state[c], 44100, s->cdt_ms);
    }

    av_log(ctx, AV_LOG_VERBOSE, "CDT period: %dms (%d samples @44100Hz)\n",
        s->cdt_ms, s->cdt_ms*44100/1000 );
    av_log(ctx, AV_LOG_VERBOSE, "Process mode: %s\n",
        (s->process_stereo) ? "process stereo channels together" : "process each channel separately");
    av_log(ctx, AV_LOG_VERBOSE, "Force PE: %s\n",
        (s->force_pe) ? "on" : "off");
    av_log(ctx, AV_LOG_VERBOSE, "Analyze mode: [%d] %s\n",
        s->analyze_mode, ana_mode_str[s->analyze_mode] );
    if (s->disable_autoconvert)
        avfilter_graph_set_auto_convert(ctx->graph, AVFILTER_AUTO_CONVERT_NONE);
    av_log(ctx, AV_LOG_VERBOSE, "Auto-convert: %s (requested: %s)\n",
        (ctx->graph->disable_auto_convert) ? "disabled" : "enabled",
        (s->disable_autoconvert) ? "disable" : "do not disable" );

    return 0;
}
