static void init_frame_decoder(APEContext *ctx)
{
    int i;
    init_entropy_decoder(ctx);
    init_predictor_decoder(ctx);

    for (i = 0; i < APE_FILTER_LEVELS; i++) {
        if (!ape_filter_orders[ctx->fset][i])
            break;
        init_filter(ctx, ctx->filters[i], ctx->filterbuf[i],
                    ape_filter_orders[ctx->fset][i]);
    }
}
