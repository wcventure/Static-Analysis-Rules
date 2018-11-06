void av_read_image_line(uint16_t *dst, const uint8_t *data[4], const int linesize[4],
                        const AVPixFmtDescriptor *desc, int x, int y, int c, int w, int read_pal_component)
{
    AVComponentDescriptor comp= desc->comp[c];
    int plane= comp.plane;
    int depth= comp.depth_minus1+1;
    int mask = (1<<depth)-1;
    int shift= comp.shift;
    int step = comp.step_minus1+1;
    int flags= desc->flags;

    if (flags & PIX_FMT_BITSTREAM){
        int skip = x*step + comp.offset_plus1-1;
        const uint8_t *p = data[plane] + y*linesize[plane] + (skip>>3);
        int shift = 8 - depth - (skip&7);

        while(w--){
            int val = (*p >> shift) & mask;
            if(read_pal_component)
                val= data[1][4*val + c];
            shift -= step;
            p -= shift>>3;
            shift &= 7;
            *dst++= val;
        }
    } else {
        const uint8_t *p = data[plane]+ y*linesize[plane] + x*step + comp.offset_plus1-1;

        while(w--){
            int val = flags & PIX_FMT_BE ? AV_RB16(p) : AV_RL16(p);
            val = (val>>shift) & mask;
            if(read_pal_component)
                val= data[1][4*val + c];
            p+= step;
            *dst++= val;
        }
    }
}
