static int dnxhd_decode_macroblock(const DNXHDContext *ctx, RowContext *row,
                                   AVFrame *frame, int x, int y)
{
    int shift1 = ctx->bit_depth == 10;
    int dct_linesize_luma   = frame->linesize[0];
    int dct_linesize_chroma = frame->linesize[1];
    uint8_t *dest_y, *dest_u, *dest_v;
    int dct_y_offset, dct_x_offset;
    int qscale, i, act;
    int interlaced_mb = 0;

    if (ctx->mbaff) {
        interlaced_mb = get_bits1(&row->gb);
        qscale = get_bits(&row->gb, 10);
    } else
        qscale = get_bits(&row->gb, 11);
    act = get_bits1(&row->gb);
    if (act) {
        static int warned = 0;
        if (!warned) {
            warned = 1;
            av_log(ctx->avctx, AV_LOG_ERROR,
                   "Unsupported adaptive color transform, patch welcome.\n");
        }
    }

    if (qscale != row->last_qscale) {
        for (i = 0; i < 64; i++) {
            row->luma_scale[i]   = qscale * ctx->cid_table->luma_weight[i];
            row->chroma_scale[i] = qscale * ctx->cid_table->chroma_weight[i];
        }
        row->last_qscale = qscale;
    }

    for (i = 0; i < 8 + 4 * ctx->is_444; i++) {
        ctx->decode_dct_block(ctx, row, i);
    }

    if (frame->interlaced_frame) {
        dct_linesize_luma   <<= 1;
        dct_linesize_chroma <<= 1;
    }

    dest_y = frame->data[0] + ((y * dct_linesize_luma)   << 4) + (x << (4 + shift1));
    dest_u = frame->data[1] + ((y * dct_linesize_chroma) << 4) + (x << (3 + shift1 + ctx->is_444));
    dest_v = frame->data[2] + ((y * dct_linesize_chroma) << 4) + (x << (3 + shift1 + ctx->is_444));

    if (frame->interlaced_frame && ctx->cur_field) {
        dest_y += frame->linesize[0];
        dest_u += frame->linesize[1];
        dest_v += frame->linesize[2];
    }
    if (interlaced_mb) {
        dct_linesize_luma   <<= 1;
        dct_linesize_chroma <<= 1;
    }

    dct_y_offset = interlaced_mb ? frame->linesize[0] : (dct_linesize_luma << 3);
    dct_x_offset = 8 << shift1;
    if (!ctx->is_444) {
        ctx->idsp.idct_put(dest_y,                               dct_linesize_luma, row->blocks[0]);
        ctx->idsp.idct_put(dest_y + dct_x_offset,                dct_linesize_luma, row->blocks[1]);
        ctx->idsp.idct_put(dest_y + dct_y_offset,                dct_linesize_luma, row->blocks[4]);
        ctx->idsp.idct_put(dest_y + dct_y_offset + dct_x_offset, dct_linesize_luma, row->blocks[5]);

        if (!(ctx->avctx->flags & AV_CODEC_FLAG_GRAY)) {
            dct_y_offset = interlaced_mb ? frame->linesize[1] : (dct_linesize_chroma << 3);
            ctx->idsp.idct_put(dest_u,                dct_linesize_chroma, row->blocks[2]);
            ctx->idsp.idct_put(dest_v,                dct_linesize_chroma, row->blocks[3]);
            ctx->idsp.idct_put(dest_u + dct_y_offset, dct_linesize_chroma, row->blocks[6]);
            ctx->idsp.idct_put(dest_v + dct_y_offset, dct_linesize_chroma, row->blocks[7]);
        }
    } else {
        ctx->idsp.idct_put(dest_y,                               dct_linesize_luma, row->blocks[0]);
        ctx->idsp.idct_put(dest_y + dct_x_offset,                dct_linesize_luma, row->blocks[1]);
        ctx->idsp.idct_put(dest_y + dct_y_offset,                dct_linesize_luma, row->blocks[6]);
        ctx->idsp.idct_put(dest_y + dct_y_offset + dct_x_offset, dct_linesize_luma, row->blocks[7]);

        if (!(ctx->avctx->flags & AV_CODEC_FLAG_GRAY)) {
            dct_y_offset = interlaced_mb ? frame->linesize[1] : (dct_linesize_chroma << 3);
            ctx->idsp.idct_put(dest_u,                               dct_linesize_chroma, row->blocks[2]);
            ctx->idsp.idct_put(dest_u + dct_x_offset,                dct_linesize_chroma, row->blocks[3]);
            ctx->idsp.idct_put(dest_u + dct_y_offset,                dct_linesize_chroma, row->blocks[8]);
            ctx->idsp.idct_put(dest_u + dct_y_offset + dct_x_offset, dct_linesize_chroma, row->blocks[9]);
            ctx->idsp.idct_put(dest_v,                               dct_linesize_chroma, row->blocks[4]);
            ctx->idsp.idct_put(dest_v + dct_x_offset,                dct_linesize_chroma, row->blocks[5]);
            ctx->idsp.idct_put(dest_v + dct_y_offset,                dct_linesize_chroma, row->blocks[10]);
            ctx->idsp.idct_put(dest_v + dct_y_offset + dct_x_offset, dct_linesize_chroma, row->blocks[11]);
        }
    }

    return 0;
}
