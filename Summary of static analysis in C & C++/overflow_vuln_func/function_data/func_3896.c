static void blend_frames16_c(BLEND_FUNC_PARAMS)
{
    int line, pixel;
    uint16_t *dstw = (uint16_t *)dst;
    uint16_t *src1w = (uint16_t *)src1;
    uint16_t *src2w = (uint16_t *)src2;
    width /= 2;
    src1_linesize /= 2;
    src2_linesize /= 2;
    dst_linesize /= 2;
    for (line = 0; line < height; line++) {
        for (pixel = 0; pixel < width; pixel++)
            dstw[pixel] = ((src1w[pixel] * factor1) + (src2w[pixel] * factor2) + half) >> shift;
        src1w += src1_linesize;
        src2w += src2_linesize;
        dstw  += dst_linesize;
    }
}
