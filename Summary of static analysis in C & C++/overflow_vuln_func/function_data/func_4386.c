static void gmc1_motion(MpegEncContext *s,
                        uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                        uint8_t **ref_picture)
{
    uint8_t *ptr;
    int offset, src_x, src_y, linesize, uvlinesize;
    int motion_x, motion_y;
    int emu=0;

    motion_x= s->sprite_offset[0][0];
    motion_y= s->sprite_offset[0][1];
    src_x = s->mb_x * 16 + (motion_x >> (s->sprite_warping_accuracy+1));
    src_y = s->mb_y * 16 + (motion_y >> (s->sprite_warping_accuracy+1));
    motion_x<<=(3-s->sprite_warping_accuracy);
    motion_y<<=(3-s->sprite_warping_accuracy);
    src_x = av_clip(src_x, -16, s->width);
    if (src_x == s->width)
        motion_x =0;
    src_y = av_clip(src_y, -16, s->height);
    if (src_y == s->height)
        motion_y =0;

    linesize = s->linesize;
    uvlinesize = s->uvlinesize;

    ptr = ref_picture[0] + (src_y * linesize) + src_x;

        if(   (unsigned)src_x >= FFMAX(s->h_edge_pos - 17, 0)
           || (unsigned)src_y >= FFMAX(s->v_edge_pos - 17, 0)){
            s->vdsp.emulated_edge_mc(s->edge_emu_buffer, ptr, linesize, 17, 17, src_x, src_y, s->h_edge_pos, s->v_edge_pos);
            ptr= s->edge_emu_buffer;
        }

    if((motion_x|motion_y)&7){
        s->dsp.gmc1(dest_y  , ptr  , linesize, 16, motion_x&15, motion_y&15, 128 - s->no_rounding);
        s->dsp.gmc1(dest_y+8, ptr+8, linesize, 16, motion_x&15, motion_y&15, 128 - s->no_rounding);
    }else{
        int dxy;

        dxy= ((motion_x>>3)&1) | ((motion_y>>2)&2);
        if (s->no_rounding){
            s->hdsp.put_no_rnd_pixels_tab[0][dxy](dest_y, ptr, linesize, 16);
        }else{
            s->hdsp.put_pixels_tab       [0][dxy](dest_y, ptr, linesize, 16);
        }
    }

    if(CONFIG_GRAY && s->flags&CODEC_FLAG_GRAY) return;

    motion_x= s->sprite_offset[1][0];
    motion_y= s->sprite_offset[1][1];
    src_x = s->mb_x * 8 + (motion_x >> (s->sprite_warping_accuracy+1));
    src_y = s->mb_y * 8 + (motion_y >> (s->sprite_warping_accuracy+1));
    motion_x<<=(3-s->sprite_warping_accuracy);
    motion_y<<=(3-s->sprite_warping_accuracy);
    src_x = av_clip(src_x, -8, s->width>>1);
    if (src_x == s->width>>1)
        motion_x =0;
    src_y = av_clip(src_y, -8, s->height>>1);
    if (src_y == s->height>>1)
        motion_y =0;

    offset = (src_y * uvlinesize) + src_x;
    ptr = ref_picture[1] + offset;
        if(   (unsigned)src_x >= FFMAX((s->h_edge_pos>>1) - 9, 0)
           || (unsigned)src_y >= FFMAX((s->v_edge_pos>>1) - 9, 0)){
            s->vdsp.emulated_edge_mc(s->edge_emu_buffer, ptr, uvlinesize, 9, 9, src_x, src_y, s->h_edge_pos>>1, s->v_edge_pos>>1);
            ptr= s->edge_emu_buffer;
            emu=1;
        }
    s->dsp.gmc1(dest_cb, ptr, uvlinesize, 8, motion_x&15, motion_y&15, 128 - s->no_rounding);

    ptr = ref_picture[2] + offset;
    if(emu){
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer, ptr, uvlinesize, 9, 9, src_x, src_y, s->h_edge_pos>>1, s->v_edge_pos>>1);
        ptr= s->edge_emu_buffer;
    }
    s->dsp.gmc1(dest_cr, ptr, uvlinesize, 8, motion_x&15, motion_y&15, 128 - s->no_rounding);

    return;
}
