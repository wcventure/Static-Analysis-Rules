void ff_mspel_motion(MpegEncContext *s,
                               uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                               uint8_t **ref_picture, op_pixels_func (*pix_op)[4],
                               int motion_x, int motion_y, int h)
{
    Wmv2Context * const w= (Wmv2Context*)s;
    uint8_t *ptr;
    int dxy, offset, mx, my, src_x, src_y, v_edge_pos, linesize, uvlinesize;
    int emu=0;

    dxy = ((motion_y & 1) << 1) | (motion_x & 1);
    dxy = 2*dxy + w->hshift;
    src_x = s->mb_x * 16 + (motion_x >> 1);
    src_y = s->mb_y * 16 + (motion_y >> 1);

    /
    v_edge_pos = s->v_edge_pos;
    src_x = av_clip(src_x, -16, s->width);
    src_y = av_clip(src_y, -16, s->height);

    if(src_x<=-16 || src_x >= s->width)
        dxy &= ~3;
    if(src_y<=-16 || src_y >= s->height)
        dxy &= ~4;

    linesize   = s->linesize;
    uvlinesize = s->uvlinesize;
    ptr = ref_picture[0] + (src_y * linesize) + src_x;

        if(src_x<1 || src_y<1 || src_x + 17  >= s->h_edge_pos
                              || src_y + h+1 >= v_edge_pos){
            s->vdsp.emulated_edge_mc(s->edge_emu_buffer, ptr - 1 - s->linesize, s->linesize, 19, 19,
                             src_x-1, src_y-1, s->h_edge_pos, s->v_edge_pos);
            ptr= s->edge_emu_buffer + 1 + s->linesize;
            emu=1;
        }

    s->dsp.put_mspel_pixels_tab[dxy](dest_y             , ptr             , linesize);
    s->dsp.put_mspel_pixels_tab[dxy](dest_y+8           , ptr+8           , linesize);
    s->dsp.put_mspel_pixels_tab[dxy](dest_y  +8*linesize, ptr  +8*linesize, linesize);
    s->dsp.put_mspel_pixels_tab[dxy](dest_y+8+8*linesize, ptr+8+8*linesize, linesize);

    if(s->flags&CODEC_FLAG_GRAY) return;

    if (s->out_format == FMT_H263) {
        dxy = 0;
        if ((motion_x & 3) != 0)
            dxy |= 1;
        if ((motion_y & 3) != 0)
            dxy |= 2;
        mx = motion_x >> 2;
        my = motion_y >> 2;
    } else {
        mx = motion_x / 2;
        my = motion_y / 2;
        dxy = ((my & 1) << 1) | (mx & 1);
        mx >>= 1;
        my >>= 1;
    }

    src_x = s->mb_x * 8 + mx;
    src_y = s->mb_y * 8 + my;
    src_x = av_clip(src_x, -8, s->width >> 1);
    if (src_x == (s->width >> 1))
        dxy &= ~1;
    src_y = av_clip(src_y, -8, s->height >> 1);
    if (src_y == (s->height >> 1))
        dxy &= ~2;
    offset = (src_y * uvlinesize) + src_x;
    ptr = ref_picture[1] + offset;
    if(emu){
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer, ptr, s->uvlinesize, 9, 9,
                         src_x, src_y, s->h_edge_pos>>1, s->v_edge_pos>>1);
        ptr= s->edge_emu_buffer;
    }
    pix_op[1][dxy](dest_cb, ptr, uvlinesize, h >> 1);

    ptr = ref_picture[2] + offset;
    if(emu){
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer, ptr, s->uvlinesize, 9, 9,
                         src_x, src_y, s->h_edge_pos>>1, s->v_edge_pos>>1);
        ptr= s->edge_emu_buffer;
    }
    pix_op[1][dxy](dest_cr, ptr, uvlinesize, h >> 1);
}
