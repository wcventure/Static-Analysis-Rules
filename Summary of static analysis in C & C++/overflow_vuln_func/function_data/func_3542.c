int ff_scale_eval_dimensions(void *log_ctx,
    const char *w_expr, const char *h_expr,
    AVFilterLink *inlink, AVFilterLink *outlink,
    int *ret_w, int *ret_h)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    const AVPixFmtDescriptor *out_desc = av_pix_fmt_desc_get(outlink->format);
    const char *expr;
    int w, h;
    int factor_w, factor_h;
    int eval_w, eval_h;
    int ret;
    const char scale2ref = outlink->src->inputs[1] == inlink;
    double var_values[VARS_NB + VARS_S2R_NB], res;
    const AVPixFmtDescriptor *main_desc;
    const AVFilterLink *main_link;
    const char *const *names = scale2ref ? var_names_scale2ref : var_names;

    if (scale2ref) {
        main_link = outlink->src->inputs[0];
        main_desc = av_pix_fmt_desc_get(main_link->format);
    }

    var_values[VAR_PI]    = M_PI;
    var_values[VAR_PHI]   = M_PHI;
    var_values[VAR_E]     = M_E;
    var_values[VAR_IN_W]  = var_values[VAR_IW] = inlink->w;
    var_values[VAR_IN_H]  = var_values[VAR_IH] = inlink->h;
    var_values[VAR_OUT_W] = var_values[VAR_OW] = NAN;
    var_values[VAR_OUT_H] = var_values[VAR_OH] = NAN;
    var_values[VAR_A]     = (double) inlink->w / inlink->h;
    var_values[VAR_SAR]   = inlink->sample_aspect_ratio.num ?
        (double) inlink->sample_aspect_ratio.num / inlink->sample_aspect_ratio.den : 1;
    var_values[VAR_DAR]   = var_values[VAR_A] * var_values[VAR_SAR];
    var_values[VAR_HSUB]  = 1 << desc->log2_chroma_w;
    var_values[VAR_VSUB]  = 1 << desc->log2_chroma_h;
    var_values[VAR_OHSUB] = 1 << out_desc->log2_chroma_w;
    var_values[VAR_OVSUB] = 1 << out_desc->log2_chroma_h;

    if (scale2ref) {
        var_values[VARS_NB + VAR_S2R_MAIN_W] = main_link->w;
        var_values[VARS_NB + VAR_S2R_MAIN_H] = main_link->h;
        var_values[VARS_NB + VAR_S2R_MAIN_A] = (double) main_link->w / main_link->h;
        var_values[VARS_NB + VAR_S2R_MAIN_SAR] = main_link->sample_aspect_ratio.num ?
            (double) main_link->sample_aspect_ratio.num / main_link->sample_aspect_ratio.den : 1;
        var_values[VARS_NB + VAR_S2R_MAIN_DAR] = var_values[VARS_NB + VAR_S2R_MDAR] =
            var_values[VARS_NB + VAR_S2R_MAIN_A] * var_values[VARS_NB + VAR_S2R_MAIN_SAR];
        var_values[VARS_NB + VAR_S2R_MAIN_HSUB] = 1 << main_desc->log2_chroma_w;
        var_values[VARS_NB + VAR_S2R_MAIN_VSUB] = 1 << main_desc->log2_chroma_h;
    }

    /
    av_expr_parse_and_eval(&res, (expr = w_expr),
                           names, var_values,
                           NULL, NULL, NULL, NULL, NULL, 0, log_ctx);
    eval_w = var_values[VAR_OUT_W] = var_values[VAR_OW] = res;

    if ((ret = av_expr_parse_and_eval(&res, (expr = h_expr),
                                      names, var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, log_ctx)) < 0)
        goto fail;
    eval_h = var_values[VAR_OUT_H] = var_values[VAR_OH] = res;
    /
    if ((ret = av_expr_parse_and_eval(&res, (expr = w_expr),
                                      names, var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, log_ctx)) < 0)
        goto fail;
    eval_w = res;

    w = eval_w;
    h = eval_h;

    /
    factor_w = 1;
    factor_h = 1;
    if (w < -1) {
        factor_w = -w;
    }
    if (h < -1) {
        factor_h = -h;
    }

    if (w < 0 && h < 0)
        eval_w = eval_h = 0;

    if (!(w = eval_w))
        w = inlink->w;
    if (!(h = eval_h))
        h = inlink->h;

    /
    if (w < 0)
        w = av_rescale(h, inlink->w, inlink->h * factor_w) * factor_w;
    if (h < 0)
        h = av_rescale(w, inlink->h, inlink->w * factor_h) * factor_h;

    *ret_w = w;
    *ret_h = h;

    return 0;

fail:
    av_log(log_ctx, AV_LOG_ERROR,
           "Error when evaluating the expression '%s'.\n"
           "Maybe the expression for out_w:'%s' or for out_h:'%s' is self-referencing.\n",
           expr, w_expr, h_expr);
    return ret;
}
