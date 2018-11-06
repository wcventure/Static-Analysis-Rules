static void cmv_decode_inter(CmvContext * s, const uint8_t *buf, const uint8_t *buf_end){
    const uint8_t *raw = buf + (s->avctx->width*s->avctx->height/16);
    int x,y,i;

    i = 0;
    for(y=0; y<s->avctx->height/4; y++)
    for(x=0; x<s->avctx->width/4 && buf+i<buf_end; x++) {
        if (buf[i]==0xFF) {
            unsigned char *dst = s->frame.data[0] + (y*4)*s->frame.linesize[0] + x*4;
            if (raw+16<buf_end && *raw==0xFF) { /
                raw++;
                memcpy(dst, raw, 4);
                memcpy(dst+s->frame.linesize[0], raw+4, 4);
                memcpy(dst+2*s->frame.linesize[0], raw+8, 4);
                memcpy(dst+3*s->frame.linesize[0], raw+12, 4);
                raw+=16;
            }else if(raw<buf_end) {  /
                int xoffset = (*raw & 0xF) - 7;
                int yoffset = ((*raw >> 4)) - 7;
                if (s->last2_frame.data[0])
                    cmv_motcomp(s->frame.data[0], s->frame.linesize[0],
                                s->last2_frame.data[0], s->last2_frame.linesize[0],
                                x*4, y*4, xoffset, yoffset, s->avctx->width, s->avctx->height);
                raw++;
            }
        }else{  /
            int xoffset = (buf[i] & 0xF) - 7;
            int yoffset = ((buf[i] >> 4)) - 7;
            cmv_motcomp(s->frame.data[0], s->frame.linesize[0],
                      s->last_frame.data[0], s->last_frame.linesize[0],
                      x*4, y*4, xoffset, yoffset, s->avctx->width, s->avctx->height);
        }
        i++;
    }
}
