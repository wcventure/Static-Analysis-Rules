static inline void chroma_4mv_motion_lowres(MpegEncContext *s,
                                            uint8_t *dest_cb, uint8_t *dest_cr,
                                            uint8_t **ref_picture,
                                            h264_chroma_mc_func * pix_op,
                                            int mx, int my)
{
    const int lowres     = s->avctx->lowres;
    const int op_index   = FFMIN(lowres, 2);
    const int block_s    = 8 >> lowres;
    const int s_mask     = (2 << lowres) - 1;
    const int h_edge_pos = s->h_edge_pos >> lowres + 1;
    const int v_edge_pos = s->v_edge_pos >> lowres + 1;
    int emu = 0, src_x, src_y, offset, sx, sy;
    uint8_t *ptr;

    if (s->quarter_sample) {
        mx /= 2;
        my /= 2;
    }

    /
    mx = ff_h263_round_chroma(mx);
    my = ff_h263_round_chroma(my);

    sx = mx & s_mask;
    sy = my & s_mask;
    src_x = s->mb_x * block_s + (mx >> lowres + 1);
    src_y = s->mb_y * block_s + (my >> lowres + 1);

    offset = src_y * s->uvlinesize + src_x;
    ptr = ref_picture[1] + offset;
    if (s->flags & CODEC_FLAG_EMU_EDGE) {
        if ((unsigned) src_x > h_edge_pos - (!!sx) - block_s ||
            (unsigned) src_y > v_edge_pos - (!!sy) - block_s) {
            s->dsp.emulated_edge_mc(s->edge_emu_buffer, ptr, s->uvlinesize,
                                    9, 9, src_x, src_y, h_edge_pos, v_edge_pos);
            ptr = s->edge_emu_buffer;
            emu = 1;
        }
    }
    sx = (sx << 2) >> lowres;
    sy = (sy << 2) >> lowres;
    pix_op[op_index](dest_cb, ptr, s->uvlinesize, block_s, sx, sy);

    ptr = ref_picture[2] + offset;
    if (emu) {
        s->dsp.emulated_edge_mc(s->edge_emu_buffer, ptr, s->uvlinesize, 9, 9,
                                src_x, src_y, h_edge_pos, v_edge_pos);
        ptr = s->edge_emu_buffer;
    }
    pix_op[op_index](dest_cr, ptr, s->uvlinesize, block_s, sx, sy);
}
