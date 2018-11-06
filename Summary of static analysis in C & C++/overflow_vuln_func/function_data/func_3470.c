static av_always_inline void mc_luma_scaled(VP9Context *s, vp9_scaled_mc_func smc,
                                            vp9_mc_func (*mc)[2],
                                            uint8_t *dst, ptrdiff_t dst_stride,
                                            const uint8_t *ref, ptrdiff_t ref_stride,
                                            ThreadFrame *ref_frame,
                                            ptrdiff_t y, ptrdiff_t x, const VP56mv *in_mv,
                                            int px, int py, int pw, int ph,
                                            int bw, int bh, int w, int h, int bytesperpixel,
                                            const uint16_t *scale, const uint8_t *step)
{
    if (s->s.frames[CUR_FRAME].tf.f->width == ref_frame->f->width &&
        s->s.frames[CUR_FRAME].tf.f->height == ref_frame->f->height) {
        mc_luma_unscaled(s, mc, dst, dst_stride, ref, ref_stride, ref_frame,
                         y, x, in_mv, bw, bh, w, h, bytesperpixel);
    } else {
#define scale_mv(n, dim) (((int64_t)(n) * scale[dim]) >> 14)
    int mx, my;
    int refbw_m1, refbh_m1;
    int th;
    VP56mv mv;

    mv.x = av_clip(in_mv->x, -(x + pw - px + 4) * 8, (s->cols * 8 - x + px + 3) * 8);
    mv.y = av_clip(in_mv->y, -(y + ph - py + 4) * 8, (s->rows * 8 - y + py + 3) * 8);
    // BUG libvpx seems to scale the two components separately. This introduces
    // rounding errors but we have to reproduce them to be exactly compatible
    // with the output from libvpx...
    mx = scale_mv(mv.x * 2, 0) + scale_mv(x * 16, 0);
    my = scale_mv(mv.y * 2, 1) + scale_mv(y * 16, 1);

    y = my >> 4;
    x = mx >> 4;
    ref += y * ref_stride + x * bytesperpixel;
    mx &= 15;
    my &= 15;
    refbw_m1 = ((bw - 1) * step[0] + mx) >> 4;
    refbh_m1 = ((bh - 1) * step[1] + my) >> 4;
    // FIXME bilinear filter only needs 0/1 pixels, not 3/4
    // we use +7 because the last 7 pixels of each sbrow can be changed in
    // the longest loopfilter of the next sbrow
    th = (y + refbh_m1 + 4 + 7) >> 6;
    ff_thread_await_progress(ref_frame, FFMAX(th, 0), 0);
    if (x < 3 || y < 3 || x + 4 >= w - refbw_m1 || y + 4 >= h - refbh_m1) {
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer,
                                 ref - 3 * ref_stride - 3 * bytesperpixel,
                                 288, ref_stride,
                                 refbw_m1 + 8, refbh_m1 + 8,
                                 x - 3, y - 3, w, h);
        ref = s->edge_emu_buffer + 3 * 288 + 3 * bytesperpixel;
        ref_stride = 288;
    }
    smc(dst, dst_stride, ref, ref_stride, bh, mx, my, step[0], step[1]);
    }
}
