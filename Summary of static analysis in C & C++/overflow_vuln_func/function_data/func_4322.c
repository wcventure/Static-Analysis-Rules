int av_image_fill_linesizes(int linesizes[4], enum PixelFormat pix_fmt, int width)
{
    int i;
    const AVPixFmtDescriptor *desc = &av_pix_fmt_descriptors[pix_fmt];
    int max_step     [4];       /
    int max_step_comp[4];       /

    memset(linesizes, 0, 4*sizeof(linesizes[0]));

    if ((unsigned)pix_fmt >= PIX_FMT_NB || desc->flags & PIX_FMT_HWACCEL)
        return AVERROR(EINVAL);

    if (desc->flags & PIX_FMT_BITSTREAM) {
        linesizes[0] = (width * (desc->comp[0].step_minus1+1) + 7) >> 3;
        return 0;
    }

    av_image_fill_max_pixsteps(max_step, max_step_comp, desc);
    for (i = 0; i < 4; i++) {
        int s = (max_step_comp[i] == 1 || max_step_comp[i] == 2) ? desc->log2_chroma_w : 0;
        linesizes[i] = max_step[i] * (((width + (1 << s) - 1)) >> s);
    }

    return 0;
}
