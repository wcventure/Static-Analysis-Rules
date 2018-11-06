static av_cold int init(AVFilterContext *ctx, const char *args)
{
    SliceContext *slice = ctx->priv;

    slice->h = 16;
    if (args) {
        if (!strcmp(args, "random")) {
            slice->use_random_h = 1;
        } else {
            sscanf(args, "%d", &slice->h);
        }
    }
    return 0;
}
