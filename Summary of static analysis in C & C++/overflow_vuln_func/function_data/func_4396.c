static av_always_inline void emulated_edge_mc(uint8_t *buf, const uint8_t *src,
                                              ptrdiff_t linesize_arg,
                                              int block_w, int block_h,
                                              int src_x, int src_y,
                                              int w, int h,
                                              emu_edge_core_func *core_fn)
{
    int start_y, start_x, end_y, end_x, src_y_add = 0;
    emuedge_linesize_type linesize = linesize_arg;

    if(!w || !h)
        return;

    if (src_y >= h) {
        src -= src_y*linesize;
        src_y_add = h - 1;
        src_y     = h - 1;
    } else if (src_y <= -block_h) {
        src -= src_y*linesize;
        src_y_add = 1 - block_h;
        src_y     = 1 - block_h;
    }
    if (src_x >= w) {
        src   += w - 1 - src_x;
        src_x  = w - 1;
    } else if (src_x <= -block_w) {
        src   += 1 - block_w - src_x;
        src_x  = 1 - block_w;
    }

    start_y = FFMAX(0, -src_y);
    start_x = FFMAX(0, -src_x);
    end_y   = FFMIN(block_h, h-src_y);
    end_x   = FFMIN(block_w, w-src_x);
    av_assert2(start_x < end_x && block_w > 0);
    av_assert2(start_y < end_y && block_h > 0);

    // fill in the to-be-copied part plus all above/below
    src += (src_y_add + start_y) * linesize + start_x;
    buf += start_x;
    core_fn(buf, src, linesize, start_y, end_y,
            block_h, start_x, end_x, block_w);
}
