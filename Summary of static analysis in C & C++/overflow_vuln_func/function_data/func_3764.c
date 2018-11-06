static av_cold int init(AVFilterContext *ctx, const char *args)
{
    GradFunContext *gf = ctx->priv;
    float thresh = 1.2;
    int radius = 16;

    if (args)
        sscanf(args, "%f:%d", &thresh, &radius);

    thresh = av_clipf(thresh, 0.51, 255);
    gf->thresh = (1 << 15) / thresh;
    gf->radius = av_clip((radius + 1) & ~1, 4, 32);

    gf->blur_line = ff_gradfun_blur_line_c;
    gf->filter_line = ff_gradfun_filter_line_c;

    if (ARCH_X86)
        ff_gradfun_init_x86(gf);

    av_log(ctx, AV_LOG_VERBOSE, "threshold:%.2f radius:%d\n", thresh, gf->radius);

    return 0;
}
