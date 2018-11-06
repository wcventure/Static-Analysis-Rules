static int decode_frame_headers(Indeo3DecodeContext *ctx, AVCodecContext *avctx,
                                const uint8_t *buf, int buf_size)
{
    GetByteContext gb;
    const uint8_t   *bs_hdr;
    uint32_t        frame_num, word2, check_sum, data_size;
    uint32_t        y_offset, u_offset, v_offset, starts[3], ends[3];
    uint16_t        height, width;
    int             i, j;

    bytestream2_init(&gb, buf, buf_size);

    /
    frame_num = bytestream2_get_le32(&gb);
    word2     = bytestream2_get_le32(&gb);
    check_sum = bytestream2_get_le32(&gb);
    data_size = bytestream2_get_le32(&gb);

    if ((frame_num ^ word2 ^ data_size ^ OS_HDR_ID) != check_sum) {
        av_log(avctx, AV_LOG_ERROR, "OS header checksum mismatch!\n");
        return AVERROR_INVALIDDATA;
    }

    /
    bs_hdr = gb.buffer;

    if (bytestream2_get_le16(&gb) != 32) {
        av_log(avctx, AV_LOG_ERROR, "Unsupported codec version!\n");
        return AVERROR_INVALIDDATA;
    }

    ctx->frame_num   =  frame_num;
    ctx->frame_flags =  bytestream2_get_le16(&gb);
    ctx->data_size   = (bytestream2_get_le32(&gb) + 7) >> 3;
    ctx->cb_offset   =  bytestream2_get_byte(&gb);

    if (ctx->data_size == 16)
        return 4;
    ctx->data_size = FFMIN(ctx->data_size, buf_size - 16);

    bytestream2_skip(&gb, 3); // skip reserved byte and checksum

    /
    height = bytestream2_get_le16(&gb);
    width  = bytestream2_get_le16(&gb);
    if (av_image_check_size(width, height, 0, avctx))
        return AVERROR_INVALIDDATA;

    if (width != ctx->width || height != ctx->height) {
        int res;

        av_dlog(avctx, "Frame dimensions changed!\n");

        if (width  < 16 || width  > 640 ||
            height < 16 || height > 480 ||
            width  &  3 || height &   3) {
            av_log(avctx, AV_LOG_ERROR,
                   "Invalid picture dimensions: %d x %d!\n", width, height);
            return AVERROR_INVALIDDATA;
        }
        free_frame_buffers(ctx);
        if ((res = allocate_frame_buffers(ctx, avctx, width, height)) < 0)
             return res;
        if ((res = ff_set_dimensions(avctx, width, height)) < 0)
            return res;
    }

    y_offset = bytestream2_get_le32(&gb);
    v_offset = bytestream2_get_le32(&gb);
    u_offset = bytestream2_get_le32(&gb);
    bytestream2_skip(&gb, 4);

    /
    /
    starts[0] = y_offset;
    starts[1] = v_offset;
    starts[2] = u_offset;

    for (j = 0; j < 3; j++) {
        ends[j] = ctx->data_size;
        for (i = 2; i >= 0; i--)
            if (starts[i] < ends[j] && starts[i] > starts[j])
                ends[j] = starts[i];
    }

    ctx->y_data_size = ends[0] - starts[0];
    ctx->v_data_size = ends[1] - starts[1];
    ctx->u_data_size = ends[2] - starts[2];
    if (FFMAX3(y_offset, v_offset, u_offset) >= ctx->data_size - 16 ||
        FFMIN3(y_offset, v_offset, u_offset) < gb.buffer - bs_hdr + 16 ||
        FFMIN3(ctx->y_data_size, ctx->v_data_size, ctx->u_data_size) <= 0) {
        av_log(avctx, AV_LOG_ERROR, "One of the y/u/v offsets is invalid\n");
        return AVERROR_INVALIDDATA;
    }

    ctx->y_data_ptr = bs_hdr + y_offset;
    ctx->v_data_ptr = bs_hdr + v_offset;
    ctx->u_data_ptr = bs_hdr + u_offset;
    ctx->alt_quant  = gb.buffer;

    if (ctx->data_size == 16) {
        av_log(avctx, AV_LOG_DEBUG, "Sync frame encountered!\n");
        return 16;
    }

    if (ctx->frame_flags & BS_8BIT_PEL) {
        avpriv_request_sample(avctx, "8-bit pixel format");
        return AVERROR_PATCHWELCOME;
    }

    if (ctx->frame_flags & BS_MV_X_HALF || ctx->frame_flags & BS_MV_Y_HALF) {
        avpriv_request_sample(avctx, "Halfpel motion vectors");
        return AVERROR_PATCHWELCOME;
    }

    return 0;
}
