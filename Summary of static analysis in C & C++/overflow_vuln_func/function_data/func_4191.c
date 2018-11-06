static int config_props(AVFilterLink *link)
{
    SliceContext *slice = link->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(link->format);

    slice->vshift = desc->log2_chroma_h;

    return 0;
}
