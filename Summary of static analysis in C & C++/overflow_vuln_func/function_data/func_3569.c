static int dnxhd_decode_row(AVCodecContext *avctx, void *data,
                            int rownb, int threadnb)
{
    const DNXHDContext *ctx = avctx->priv_data;
    uint32_t offset = ctx->mb_scan_index[rownb];
    RowContext *row = ctx->rows + threadnb;
    int x;

    row->last_dc[0] =
    row->last_dc[1] =
    row->last_dc[2] = 1 << (ctx->bit_depth + 2); // for levels +2^(bitdepth-1)
    init_get_bits(&row->gb, ctx->buf + offset, (ctx->buf_size - offset) << 3);
    for (x = 0; x < ctx->mb_width; x++) {
        //START_TIMER;
        dnxhd_decode_macroblock(ctx, row, data, x, rownb);
        //STOP_TIMER("decode macroblock");
    }

    return 0;
}
