static int decode_dc_progressive(MJpegDecodeContext *s, int16_t *block,
                                 int component, int dc_index,
                                 uint16_t *quant_matrix, int Al)
{
    int val;
    s->bdsp.clear_block(block);
    val = mjpeg_decode_dc(s, dc_index);
    if (val == 0xfffff) {
        av_log(s->avctx, AV_LOG_ERROR, "error dc\n");
        return AVERROR_INVALIDDATA;
    }
    val = (val * (quant_matrix[0] << Al)) + s->last_dc[component];
    s->last_dc[component] = val;
    block[0] = val;
    return 0;
}
