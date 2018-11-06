static int filter_slice(AVFilterContext *ctx, void *arg, int job, int nb_jobs)
{
    FrameRateContext *s = ctx->priv;
    ThreadData *td = arg;
    uint16_t src1_factor = td->src1_factor;
    uint16_t src2_factor = td->src2_factor;
    int plane;

    for (plane = 0; plane < 4 && td->copy_src1->data[plane] && td->copy_src2->data[plane]; plane++) {
        int cpy_line_width = s->line_size[plane];
        uint8_t *cpy_src1_data = td->copy_src1->data[plane];
        int cpy_src1_line_size = td->copy_src1->linesize[plane];
        uint8_t *cpy_src2_data = td->copy_src2->data[plane];
        int cpy_src2_line_size = td->copy_src2->linesize[plane];
        int cpy_src_h = (plane > 0 && plane < 3) ? (td->copy_src1->height >> s->vsub) : (td->copy_src1->height);
        uint8_t *cpy_dst_data = s->work->data[plane];
        int cpy_dst_line_size = s->work->linesize[plane];
        const int start = (cpy_src_h *  job   ) / nb_jobs;
        const int end   = (cpy_src_h * (job+1)) / nb_jobs;
        cpy_src1_data += start * cpy_src1_line_size;
        cpy_src2_data += start * cpy_src2_line_size;
        cpy_dst_data += start * cpy_dst_line_size;

        s->blend(cpy_src1_data, cpy_src1_line_size,
                 cpy_src2_data, cpy_src2_line_size,
                 cpy_dst_data,  cpy_dst_line_size,
                 cpy_line_width, end - start,
                 src1_factor, src2_factor, s->max / 2, s->bitdepth);
    }

    return 0;
}
