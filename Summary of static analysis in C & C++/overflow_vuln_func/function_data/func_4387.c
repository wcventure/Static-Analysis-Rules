static inline void qpel_motion(MpegEncContext *s,
                               uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                               int field_based, int bottom_field, int field_select,
                               uint8_t **ref_picture, op_pixels_func (*pix_op)[4],
                               qpel_mc_func (*qpix_op)[16],
                               int motion_x, int motion_y, int h)
{
    uint8_t *ptr_y, *ptr_cb, *ptr_cr;
    int dxy, uvdxy, mx, my, src_x, src_y, uvsrc_x, uvsrc_y, v_edge_pos, linesize, uvlinesize;

    dxy = ((motion_y & 3) << 2) | (motion_x & 3);
    src_x = s->mb_x *  16                 + (motion_x >> 2);
    src_y = s->mb_y * (16 >> field_based) + (motion_y >> 2);

    v_edge_pos = s->v_edge_pos >> field_based;
    linesize = s->linesize << field_based;
    uvlinesize = s->uvlinesize << field_based;

    if(field_based){
        mx= motion_x/2;
        my= motion_y>>1;
    }else if(s->workaround_bugs&FF_BUG_QPEL_CHROMA2){
        static const int rtab[8]= {0,0,1,1,0,0,0,1};
        mx= (motion_x>>1) + rtab[motion_x&7];
        my= (motion_y>>1) + rtab[motion_y&7];
    }else if(s->workaround_bugs&FF_BUG_QPEL_CHROMA){
        mx= (motion_x>>1)|(motion_x&1);
        my= (motion_y>>1)|(motion_y&1);
    }else{
        mx= motion_x/2;
        my= motion_y/2;
    }
    mx= (mx>>1)|(mx&1);
    my= (my>>1)|(my&1);

    uvdxy= (mx&1) | ((my&1)<<1);
    mx>>=1;
    my>>=1;

    uvsrc_x = s->mb_x *  8                 + mx;
    uvsrc_y = s->mb_y * (8 >> field_based) + my;

    ptr_y  = ref_picture[0] +   src_y *   linesize +   src_x;
    ptr_cb = ref_picture[1] + uvsrc_y * uvlinesize + uvsrc_x;
    ptr_cr = ref_picture[2] + uvsrc_y * uvlinesize + uvsrc_x;

    if(   (unsigned)src_x > FFMAX(s->h_edge_pos - (motion_x&3) - 16, 0)
       || (unsigned)src_y > FFMAX(   v_edge_pos - (motion_y&3) - h , 0)){
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer, ptr_y, s->linesize,
                            17, 17+field_based, src_x, src_y<<field_based,
                            s->h_edge_pos, s->v_edge_pos);
        ptr_y= s->edge_emu_buffer;
        if(!CONFIG_GRAY || !(s->flags&CODEC_FLAG_GRAY)){
            uint8_t *uvbuf= s->edge_emu_buffer + 18*s->linesize;
            s->vdsp.emulated_edge_mc(uvbuf, ptr_cb, s->uvlinesize,
                                9, 9 + field_based,
                                uvsrc_x, uvsrc_y<<field_based,
                                s->h_edge_pos>>1, s->v_edge_pos>>1);
            s->vdsp.emulated_edge_mc(uvbuf + 16, ptr_cr, s->uvlinesize,
                                9, 9 + field_based,
                                uvsrc_x, uvsrc_y<<field_based,
                                s->h_edge_pos>>1, s->v_edge_pos>>1);
            ptr_cb= uvbuf;
            ptr_cr= uvbuf + 16;
        }
    }

    if(!field_based)
        qpix_op[0][dxy](dest_y, ptr_y, linesize);
    else{
        if(bottom_field){
            dest_y += s->linesize;
            dest_cb+= s->uvlinesize;
            dest_cr+= s->uvlinesize;
        }

        if(field_select){
            ptr_y  += s->linesize;
            ptr_cb += s->uvlinesize;
            ptr_cr += s->uvlinesize;
        }
        //damn interlaced mode
        //FIXME boundary mirroring is not exactly correct here
        qpix_op[1][dxy](dest_y  , ptr_y  , linesize);
        qpix_op[1][dxy](dest_y+8, ptr_y+8, linesize);
    }
    if(!CONFIG_GRAY || !(s->flags&CODEC_FLAG_GRAY)){
        pix_op[1][uvdxy](dest_cr, ptr_cr, uvlinesize, h >> 1);
        pix_op[1][uvdxy](dest_cb, ptr_cb, uvlinesize, h >> 1);
    }
}
