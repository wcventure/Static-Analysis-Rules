static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    SPPContext *spp = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out = in;
    int qp_stride = 0;
    const int8_t *qp_table = NULL;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    const int sample_bytes = desc->comp[0].depth_minus1 < 8 ? 1 : 2;

    /
    if (!spp->qp) {
        qp_table = av_frame_get_qp_table(in, &qp_stride, &spp->qscale_type);

        if (qp_table && !spp->use_bframe_qp && in->pict_type != AV_PICTURE_TYPE_B) {
            int w, h;

            /
            if (!qp_stride) {
                w = FF_CEIL_RSHIFT(inlink->w, 4);
                h = 1;
            } else {
                w = qp_stride;
                h = FF_CEIL_RSHIFT(inlink->h, 4);
            }

            if (w * h > spp->non_b_qp_alloc_size) {
                int ret = av_reallocp_array(&spp->non_b_qp_table, w, h);
                if (ret < 0) {
                    spp->non_b_qp_alloc_size = 0;
                    return ret;
                }
                spp->non_b_qp_alloc_size = w * h;
            }

            av_assert0(w * h <= spp->non_b_qp_alloc_size);
            memcpy(spp->non_b_qp_table, qp_table, w * h);
        }
    }

    if (spp->log2_count && !ctx->is_disabled) {
        if (!spp->use_bframe_qp && spp->non_b_qp_table)
            qp_table = spp->non_b_qp_table;

        if (qp_table || spp->qp) {
            const int cw = FF_CEIL_RSHIFT(inlink->w, spp->hsub);
            const int ch = FF_CEIL_RSHIFT(inlink->h, spp->vsub);

            /
            if (!av_frame_is_writable(in) || (inlink->w & 7) || (inlink->h & 7)) {
                const int aligned_w = FFALIGN(inlink->w, 8);
                const int aligned_h = FFALIGN(inlink->h, 8);

                out = ff_get_video_buffer(outlink, aligned_w, aligned_h);
                if (!out) {
                    av_frame_free(&in);
                    return AVERROR(ENOMEM);
                }
                av_frame_copy_props(out, in);
                out->width  = in->width;
                out->height = in->height;
            }

            filter(spp, out->data[0], in->data[0], out->linesize[0], in->linesize[0], inlink->w, inlink->h, qp_table, qp_stride, 1, sample_bytes);

            if (out->data[2]) {
                filter(spp, out->data[1], in->data[1], out->linesize[1], in->linesize[1], cw,        ch,        qp_table, qp_stride, 0, sample_bytes);
                filter(spp, out->data[2], in->data[2], out->linesize[2], in->linesize[2], cw,        ch,        qp_table, qp_stride, 0, sample_bytes);
            }
            emms_c();
        }
    }

    if (in != out) {
        if (in->data[3])
            av_image_copy_plane(out->data[3], out->linesize[3],
                                in ->data[3], in ->linesize[3],
                                inlink->w, inlink->h);
        av_frame_free(&in);
    }
    return ff_filter_frame(outlink, out);
}
