static av_always_inline void mc_chroma_unscaled(VP9Context *s, vp9_mc_func (*mc)[2],
                                                uint8_t *dst_u, uint8_t *dst_v,
                                                ptrdiff_t dst_stride,
                                                const uint8_t *ref_u, ptrdiff_t src_stride_u,
                                                const uint8_t *ref_v, ptrdiff_t src_stride_v,
                                                ThreadFrame *ref_frame,
                                                ptrdiff_t y, ptrdiff_t x, const VP56mv *mv,
                                                int bw, int bh, int w, int h, int bytesperpixel)
{
    int mx = mv->x * (1 << !s->ss_h), my = mv->y * (1 << !s->ss_v), th;

    y += my >> 4;
    x += mx >> 4;
    ref_u += y * src_stride_u + x * bytesperpixel;
    ref_v += y * src_stride_v + x * bytesperpixel;
    mx &= 15;
    my &= 15;
    // FIXME bilinear filter only needs 0/1 pixels, not 3/4
    // we use +7 because the last 7 pixels of each sbrow can be changed in
    // the longest loopfilter of the next sbrow
    th = (y + bh + 4 * !!my + 7) >> (6 - s->ss_v);
    ff_thread_await_progress(ref_frame, FFMAX(th, 0), 0);
    if (x < !!mx * 3 || y < !!my * 3 ||
        x + !!mx * 4 > w - bw || y + !!my * 4 > h - bh) {
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer,
                                 ref_u - !!my * 3 * src_stride_u - !!mx * 3 * bytesperpixel,
                                 160, src_stride_u,
                                 bw + !!mx * 7, bh + !!my * 7,
                                 x - !!mx * 3, y - !!my * 3, w, h);
        ref_u = s->edge_emu_buffer + !!my * 3 * 160 + !!mx * 3 * bytesperpixel;
        mc[!!mx][!!my](dst_u, dst_stride, ref_u, 160, bh, mx, my);

        s->vdsp.emulated_edge_mc(s->edge_emu_buffer,
                                 ref_v - !!my * 3 * src_stride_v - !!mx * 3 * bytesperpixel,
                                 160, src_stride_v,
                                 bw + !!mx * 7, bh + !!my * 7,
                                 x - !!mx * 3, y - !!my * 3, w, h);
        ref_v = s->edge_emu_buffer + !!my * 3 * 160 + !!mx * 3 * bytesperpixel;
        mc[!!mx][!!my](dst_v, dst_stride, ref_v, 160, bh, mx, my);
    } else {
        mc[!!mx][!!my](dst_u, dst_stride, ref_u, src_stride_u, bh, mx, my);
        mc[!!mx][!!my](dst_v, dst_stride, ref_v, src_stride_v, bh, mx, my);
    }
}
