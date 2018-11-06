static void blend_frames_c(BLEND_FUNC_PARAMS)
{
    int line, pixel;
    for (line = 0; line < height; line++) {
        for (pixel = 0; pixel < width; pixel++) {
            // integer version of (src1 * factor1) + (src2 * factor2) + 0.5
            // 0.5 is for rounding
            // 128 is the integer representation of 0.5 << 8
            dst[pixel] = ((src1[pixel] * factor1) + (src2[pixel] * factor2) + 128) >> 8;
        }
        src1 += src1_linesize;
        src2 += src2_linesize;
        dst  += dst_linesize;
    }
}
