static av_always_inline void mc_luma_unscaled(VP9Context *s, vp9_mc_func (*mc)[2],
                                              uint8_t *dst, ptrdiff_t dst_stride,
                                              const uint8_t *ref, ptrdiff_t ref_stride,
                                              ThreadFrame *ref_frame,
                                              ptrdiff_t y, ptrdiff_t x, const VP56mv *mv,
                                              int bw, int bh, int w, int h, int bytesperpixel)
{
    int mx = mv->x, my = mv->y, th;

    y += my >> 3;
    x += mx >> 3;
    ref += y * ref_stride + x * bytesperpixel;
    mx &= 7;
    my &= 7;
    // FIXME bilinear filter only needs 0/1 pixels, not 3/4
    // we use +7 because the last 7 pixels of each sbrow can be changed in
    // the longest loopfilter of the next sbrow
    th = (y + bh + 4 * !!my + 7) >> 6;
    ff_thread_await_progress(ref_frame, FFMAX(th, 0), 0);
    if (x < !!mx * 3 || y < !!my * 3 ||
        x + !!mx * 4 > w - bw || y + !!my * 4 > h - bh) {
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer,
                                 ref - !!my * 3 * ref_stride - !!mx * 3 * bytesperpixel,
                                 160, ref_stride,
                                 bw + !!mx * 7, bh + !!my * 7,
                                 x - !!mx * 3, y - !!my * 3, w, h);
        ref = s->edge_emu_buffer + !!my * 3 * 160 + !!mx * 3 * bytesperpixel;
        ref_stride = 160;
    }
    mc[!!mx][!!my](dst, dst_stride, ref, ref_stride, bh, mx << 1, my << 1);
}
