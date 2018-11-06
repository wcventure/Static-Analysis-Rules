static av_always_inline
void vp8_mc_chroma(VP8Context *s, VP8ThreadData *td, uint8_t *dst1, uint8_t *dst2,
                   ThreadFrame *ref, const VP56mv *mv, int x_off, int y_off,
                   int block_w, int block_h, int width, int height, int linesize,
                   vp8_mc_func mc_func[3][3])
{
    uint8_t *src1 = ref->f->data[1], *src2 = ref->f->data[2];

    if (AV_RN32A(mv)) {
        int mx = mv->x&7, mx_idx = subpel_idx[0][mx];
        int my = mv->y&7, my_idx = subpel_idx[0][my];

        x_off += mv->x >> 3;
        y_off += mv->y >> 3;

        // edge emulation
        src1 += y_off * linesize + x_off;
        src2 += y_off * linesize + x_off;
        ff_thread_await_progress(ref, (3 + y_off + block_h + subpel_idx[2][my]) >> 3, 0);
        if (x_off < mx_idx || x_off >= width  - block_w - subpel_idx[2][mx] ||
            y_off < my_idx || y_off >= height - block_h - subpel_idx[2][my]) {
            s->vdsp.emulated_edge_mc(td->edge_emu_buffer, src1 - my_idx * linesize - mx_idx, linesize,
                                     block_w + subpel_idx[1][mx], block_h + subpel_idx[1][my],
                                     x_off - mx_idx, y_off - my_idx, width, height);
            src1 = td->edge_emu_buffer + mx_idx + linesize * my_idx;
            mc_func[my_idx][mx_idx](dst1, linesize, src1, linesize, block_h, mx, my);

            s->vdsp.emulated_edge_mc(td->edge_emu_buffer, src2 - my_idx * linesize - mx_idx, linesize,
                                     block_w + subpel_idx[1][mx], block_h + subpel_idx[1][my],
                                     x_off - mx_idx, y_off - my_idx, width, height);
            src2 = td->edge_emu_buffer + mx_idx + linesize * my_idx;
            mc_func[my_idx][mx_idx](dst2, linesize, src2, linesize, block_h, mx, my);
        } else {
            mc_func[my_idx][mx_idx](dst1, linesize, src1, linesize, block_h, mx, my);
            mc_func[my_idx][mx_idx](dst2, linesize, src2, linesize, block_h, mx, my);
        }
    } else {
        ff_thread_await_progress(ref, (3 + y_off + block_h) >> 3, 0);
        mc_func[0][0](dst1, linesize, src1 + y_off * linesize + x_off, linesize, block_h, 0, 0);
        mc_func[0][0](dst2, linesize, src2 + y_off * linesize + x_off, linesize, block_h, 0, 0);
    }
}
