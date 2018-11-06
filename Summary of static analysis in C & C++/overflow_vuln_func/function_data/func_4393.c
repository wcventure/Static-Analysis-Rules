static av_always_inline
void vp8_mc_luma(VP8Context *s, VP8ThreadData *td, uint8_t *dst,
                 ThreadFrame *ref, const VP56mv *mv,
                 int x_off, int y_off, int block_w, int block_h,
                 int width, int height, int linesize,
                 vp8_mc_func mc_func[3][3])
{
    uint8_t *src = ref->f->data[0];

    if (AV_RN32A(mv)) {

        int mx = (mv->x << 1)&7, mx_idx = subpel_idx[0][mx];
        int my = (mv->y << 1)&7, my_idx = subpel_idx[0][my];

        x_off += mv->x >> 2;
        y_off += mv->y >> 2;

        // edge emulation
        ff_thread_await_progress(ref, (3 + y_off + block_h + subpel_idx[2][my]) >> 4, 0);
        src += y_off * linesize + x_off;
        if (x_off < mx_idx || x_off >= width  - block_w - subpel_idx[2][mx] ||
            y_off < my_idx || y_off >= height - block_h - subpel_idx[2][my]) {
            s->vdsp.emulated_edge_mc(td->edge_emu_buffer, src - my_idx * linesize - mx_idx, linesize,
                                     block_w + subpel_idx[1][mx], block_h + subpel_idx[1][my],
                                     x_off - mx_idx, y_off - my_idx, width, height);
            src = td->edge_emu_buffer + mx_idx + linesize * my_idx;
        }
        mc_func[my_idx][mx_idx](dst, linesize, src, linesize, block_h, mx, my);
    } else {
        ff_thread_await_progress(ref, (3 + y_off + block_h) >> 4, 0);
        mc_func[0][0](dst, linesize, src + y_off * linesize + x_off, linesize, block_h, 0, 0);
    }
}
