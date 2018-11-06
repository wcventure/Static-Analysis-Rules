static void chroma_4mv_motion(MpegEncContext *s,
                              uint8_t *dest_cb, uint8_t *dest_cr,
                              uint8_t **ref_picture,
                              op_pixels_func *pix_op,
                              int mx, int my)
{
    int dxy, emu=0, src_x, src_y, offset;
    uint8_t *ptr;

    /
    mx= ff_h263_round_chroma(mx);
    my= ff_h263_round_chroma(my);

    dxy = ((my & 1) << 1) | (mx & 1);
    mx >>= 1;
    my >>= 1;

    src_x = s->mb_x * 8 + mx;
    src_y = s->mb_y * 8 + my;
    src_x = av_clip(src_x, -8, (s->width >> 1));
    if (src_x == (s->width >> 1))
        dxy &= ~1;
    src_y = av_clip(src_y, -8, (s->height >> 1));
    if (src_y == (s->height >> 1))
        dxy &= ~2;

    offset = src_y * s->uvlinesize + src_x;
    ptr = ref_picture[1] + offset;
    if(s->flags&CODEC_FLAG_EMU_EDGE){
        if(   (unsigned)src_x > FFMAX((s->h_edge_pos>>1) - (dxy &1) - 8, 0)
           || (unsigned)src_y > FFMAX((s->v_edge_pos>>1) - (dxy>>1) - 8, 0)){
            s->vdsp.emulated_edge_mc(s->edge_emu_buffer, ptr, s->uvlinesize,
                                9, 9, src_x, src_y,
                                s->h_edge_pos>>1, s->v_edge_pos>>1);
            ptr= s->edge_emu_buffer;
            emu=1;
        }
    }
    pix_op[dxy](dest_cb, ptr, s->uvlinesize, 8);

    ptr = ref_picture[2] + offset;
    if(emu){
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer, ptr, s->uvlinesize,
                            9, 9, src_x, src_y,
                            s->h_edge_pos>>1, s->v_edge_pos>>1);
        ptr= s->edge_emu_buffer;
    }
    pix_op[dxy](dest_cr, ptr, s->uvlinesize, 8);
}
