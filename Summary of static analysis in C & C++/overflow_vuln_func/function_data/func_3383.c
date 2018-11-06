static int fic_decode_frame(AVCodecContext *avctx, void *data,
                            int *got_frame, AVPacket *avpkt)
{
    FICContext *ctx = avctx->priv_data;
    uint8_t *src = avpkt->data;
    int ret;
    int slice, nslices;
    int msize;
    int tsize;
    uint8_t *sdata;

    if ((ret = ff_reget_buffer(avctx, ctx->frame)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "reget_buffer() failed\n");
        return ret;
    }

    /
    if (avpkt->size < FIC_HEADER_SIZE + 4) {
        av_log(avctx, AV_LOG_ERROR, "Frame data is too small.\n");
        return AVERROR_INVALIDDATA;
    }

    /
    if (memcmp(src, fic_header, 7))
        av_log(avctx, AV_LOG_WARNING, "Invalid FIC Header.\n");

    /
    if (src[17])
        goto skip;

    nslices = src[13];
    if (!nslices) {
        av_log(avctx, AV_LOG_ERROR, "Zero slices found.\n");
        return AVERROR_INVALIDDATA;
    }

    /
    ctx->qmat = src[23] ? fic_qmat_hq : fic_qmat_lq;

    /
    tsize = AV_RB24(src + 24);
    if (tsize > avpkt->size - FIC_HEADER_SIZE) {
        av_log(avctx, AV_LOG_ERROR, "Invalid cursor data size.\n");
        return AVERROR_INVALIDDATA;
    }

    /
    ctx->slice_h = 16 * (ctx->aligned_height >> 4) / nslices;
    if (ctx->slice_h % 16)
        ctx->slice_h = FFALIGN(ctx->slice_h - 16, 16);

    /
    sdata = src + tsize + FIC_HEADER_SIZE + 4 * nslices;
    msize = avpkt->size - nslices * 4 - tsize - FIC_HEADER_SIZE;

    if (msize <= 0) {
        av_log(avctx, AV_LOG_ERROR, "Not enough frame data to decode.\n");
        return AVERROR_INVALIDDATA;
    }

    /
    ctx->frame->key_frame = 1;
    ctx->frame->pict_type = AV_PICTURE_TYPE_I;

    /
    av_fast_malloc(&ctx->slice_data, &ctx->slice_data_size,
                   nslices * sizeof(ctx->slice_data[0]));
    if (!ctx->slice_data_size) {
        av_log(avctx, AV_LOG_ERROR, "Could not allocate slice data.\n");
        return AVERROR(ENOMEM);
    }

    for (slice = 0; slice < nslices; slice++) {
        int slice_off = AV_RB32(src + tsize + FIC_HEADER_SIZE + slice * 4);
        int slice_size;
        int y_off   = ctx->slice_h * slice;
        int slice_h = ctx->slice_h;

        /
        if (slice == nslices - 1) {
            slice_size   = msize;
            slice_h      = FFALIGN(avctx->height - ctx->slice_h * (nslices - 1), 16);
        } else {
            slice_size = AV_RB32(src + tsize + FIC_HEADER_SIZE + slice * 4 + 4);
        }

        slice_size -= slice_off;

        if (slice_off > msize || slice_off + slice_size > msize)
            continue;

        ctx->slice_data[slice].src      = sdata + slice_off;
        ctx->slice_data[slice].src_size = slice_size;
        ctx->slice_data[slice].slice_h  = slice_h;
        ctx->slice_data[slice].y_off    = y_off;
    }

    if (ret = avctx->execute(avctx, fic_decode_slice, ctx->slice_data,
                             NULL, nslices, sizeof(ctx->slice_data[0])) < 0)
        return ret;

skip:
    *got_frame = 1;
    if ((ret = av_frame_ref(data, ctx->frame)) < 0)
        return ret;

    return avpkt->size;
}
