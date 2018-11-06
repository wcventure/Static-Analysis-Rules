static av_always_inline void mc_chroma_scaled(VP9Context *s, vp9_scaled_mc_func smc,
                                              vp9_mc_func (*mc)[2],
                                              uint8_t *dst_u, uint8_t *dst_v,
                                              ptrdiff_t dst_stride,
                                              const uint8_t *ref_u, ptrdiff_t src_stride_u,
                                              const uint8_t *ref_v, ptrdiff_t src_stride_v,
                                              ThreadFrame *ref_frame,
                                              ptrdiff_t y, ptrdiff_t x, const VP56mv *in_mv,
                                              int px, int py, int pw, int ph,
                                              int bw, int bh, int w, int h, int bytesperpixel,
                                              const uint16_t *scale, const uint8_t *step)
{
    if (s->s.frames[CUR_FRAME].tf.f->width == ref_frame->f->width &&
        s->s.frames[CUR_FRAME].tf.f->height == ref_frame->f->height) {
        mc_chroma_unscaled(s, mc, dst_u, dst_v, dst_stride, ref_u, src_stride_u,
                           ref_v, src_stride_v, ref_frame,
                           y, x, in_mv, bw, bh, w, h, bytesperpixel);
    } else {
    int mx, my;
    int refbw_m1, refbh_m1;
    int th;
    VP56mv mv;

    if (s->ss_h) {
        // BUG https://code.google.com/p/webm/issues/detail?id=820
        mv.x = av_clip(in_mv->x, -(x + pw - px + 4) * 16, (s->cols * 4 - x + px + 3) * 16);
        mx = scale_mv(mv.x, 0) + (scale_mv(x * 16, 0) & ~15) + (scale_mv(x * 32, 0) & 15);
    } else {
        mv.x = av_clip(in_mv->x, -(x + pw - px + 4) * 8, (s->cols * 8 - x + px + 3) * 8);
        mx = scale_mv(mv.x * 2, 0) + scale_mv(x * 16, 0);
    }
    if (s->ss_v) {
        // BUG https://code.google.com/p/webm/issues/detail?id=820
        mv.y = av_clip(in_mv->y, -(y + ph - py + 4) * 16, (s->rows * 4 - y + py + 3) * 16);
        my = scale_mv(mv.y, 1) + (scale_mv(y * 16, 1) & ~15) + (scale_mv(y * 32, 1) & 15);
    } else {
        mv.y = av_clip(in_mv->y, -(y + ph - py + 4) * 8, (s->rows * 8 - y + py + 3) * 8);
        my = scale_mv(mv.y * 2, 1) + scale_mv(y * 16, 1);
    }
#undef scale_mv
    y = my >> 4;
    x = mx >> 4;
    ref_u += y * src_stride_u + x * bytesperpixel;
    ref_v += y * src_stride_v + x * bytesperpixel;
    mx &= 15;
    my &= 15;
    refbw_m1 = ((bw - 1) * step[0] + mx) >> 4;
    refbh_m1 = ((bh - 1) * step[1] + my) >> 4;
    // FIXME bilinear filter only needs 0/1 pixels, not 3/4
    // we use +7 because the last 7 pixels of each sbrow can be changed in
    // the longest loopfilter of the next sbrow
    th = (y + refbh_m1 + 4 + 7) >> (6 - s->ss_v);
    ff_thread_await_progress(ref_frame, FFMAX(th, 0), 0);
    if (x < 3 || y < 3 || x + 4 >= w - refbw_m1 || y + 4 >= h - refbh_m1) {
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer,
                                 ref_u - 3 * src_stride_u - 3 * bytesperpixel,
                                 288, src_stride_u,
                                 refbw_m1 + 8, refbh_m1 + 8,
                                 x - 3, y - 3, w, h);
        ref_u = s->edge_emu_buffer + 3 * 288 + 3 * bytesperpixel;
        smc(dst_u, dst_stride, ref_u, 288, bh, mx, my, step[0], step[1]);

        s->vdsp.emulated_edge_mc(s->edge_emu_buffer,
                                 ref_v - 3 * src_stride_v - 3 * bytesperpixel,
                                 288, src_stride_v,
                                 refbw_m1 + 8, refbh_m1 + 8,
                                 x - 3, y - 3, w, h);
        ref_v = s->edge_emu_buffer + 3 * 288 + 3 * bytesperpixel;
        smc(dst_v, dst_stride, ref_v, 288, bh, mx, my, step[0], step[1]);
    } else {
        smc(dst_u, dst_stride, ref_u, src_stride_u, bh, mx, my, step[0], step[1]);
        smc(dst_v, dst_stride, ref_v, src_stride_v, bh, mx, my, step[0], step[1]);
    }
    }
}
