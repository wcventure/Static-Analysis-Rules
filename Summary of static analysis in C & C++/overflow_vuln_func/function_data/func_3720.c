static int vdpau_frames_init(AVHWFramesContext *ctx)
{
    VDPAUDeviceContext *device_priv = ctx->device_ctx->internal->priv;
    VDPAUFramesContext        *priv = ctx->internal->priv;

    int i;

    switch (ctx->sw_format) {
    case AV_PIX_FMT_YUV420P: priv->chroma_type = VDP_CHROMA_TYPE_420; break;
    case AV_PIX_FMT_YUV422P: priv->chroma_type = VDP_CHROMA_TYPE_422; break;
    case AV_PIX_FMT_YUV444P: priv->chroma_type = VDP_CHROMA_TYPE_444; break;
    default:
        av_log(ctx, AV_LOG_ERROR, "Unsupported data layout: %s\n",
               av_get_pix_fmt_name(ctx->sw_format));
        return AVERROR(ENOSYS);
    }

    for (i = 0; i < FF_ARRAY_ELEMS(vdpau_pix_fmts); i++) {
        if (vdpau_pix_fmts[i].chroma_type == priv->chroma_type) {
            priv->chroma_idx  = i;
            priv->pix_fmts    = device_priv->pix_fmts[i];
            priv->nb_pix_fmts = device_priv->nb_pix_fmts[i];
            break;
        }
    }
    if (!priv->pix_fmts) {
        av_log(ctx, AV_LOG_ERROR, "Unsupported chroma type: %d\n", priv->chroma_type);
        return AVERROR(ENOSYS);
    }

    if (!ctx->pool) {
        ctx->internal->pool_internal = av_buffer_pool_init2(sizeof(VdpVideoSurface), ctx,
                                                            vdpau_pool_alloc, NULL);
        if (!ctx->internal->pool_internal)
            return AVERROR(ENOMEM);
    }

    priv->get_data = device_priv->get_data;
    priv->put_data = device_priv->put_data;

    return 0;
}
