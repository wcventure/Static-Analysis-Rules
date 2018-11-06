static int config_input_ref(AVFilterLink *inlink)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    AVFilterContext *ctx  = inlink->dst;
    PSNRContext *s = ctx->priv;
    unsigned sum;
    int j;

    s->nb_components = desc->nb_components;
    if (ctx->inputs[0]->w != ctx->inputs[1]->w ||
        ctx->inputs[0]->h != ctx->inputs[1]->h) {
        av_log(ctx, AV_LOG_ERROR, "Width and height of input videos must be same.\n");
        return AVERROR(EINVAL);
    }
    if (ctx->inputs[0]->format != ctx->inputs[1]->format) {
        av_log(ctx, AV_LOG_ERROR, "Inputs must be of same pixel format.\n");
        return AVERROR(EINVAL);
    }

    s->max[0] = (1 << (desc->comp[0].depth_minus1 + 1)) - 1;
    s->max[1] = (1 << (desc->comp[1].depth_minus1 + 1)) - 1;
    s->max[2] = (1 << (desc->comp[2].depth_minus1 + 1)) - 1;
    s->max[3] = (1 << (desc->comp[3].depth_minus1 + 1)) - 1;

    s->is_rgb = ff_fill_rgba_map(s->rgba_map, inlink->format) >= 0;
    s->comps[0] = s->is_rgb ? 'r' : 'y' ;
    s->comps[1] = s->is_rgb ? 'g' : 'u' ;
    s->comps[2] = s->is_rgb ? 'b' : 'v' ;
    s->comps[3] = 'a';

    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->planewidth[1]  = s->planewidth[2]  = FF_CEIL_RSHIFT(inlink->w, desc->log2_chroma_w);
    s->planewidth[0]  = s->planewidth[3]  = inlink->w;
    sum = 0;
    for (j = 0; j < s->nb_components; j++)
        sum += s->planeheight[j] * s->planewidth[j];
    for (j = 0; j < s->nb_components; j++) {
        s->planeweight[j] = (double) s->planeheight[j] * s->planewidth[j] / sum;
        s->average_max += s->max[j] * s->planeweight[j];
    }

    s->compute_mse = desc->comp[0].depth_minus1 > 7 ? compute_images_mse_16bit : compute_images_mse;

    return 0;
}
