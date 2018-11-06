void FUNCC(ff_h264_idct_add)(uint8_t *_dst, int16_t *_block, int stride)
{
    int i;
    pixel *dst = (pixel*)_dst;
    dctcoef *block = (dctcoef*)_block;
    stride >>= sizeof(pixel)-1;

    block[0] += 1 << 5;

    for(i=0; i<4; i++){
        const SUINT z0=  block[i + 4*0]     +  block[i + 4*2];
        const SUINT z1=  block[i + 4*0]     -  block[i + 4*2];
        const SUINT z2= (block[i + 4*1]>>1) -  block[i + 4*3];
        const SUINT z3=  block[i + 4*1]     + (block[i + 4*3]>>1);

        block[i + 4*0]= z0 + z3;
        block[i + 4*1]= z1 + z2;
        block[i + 4*2]= z1 - z2;
        block[i + 4*3]= z0 - z3;
    }

    for(i=0; i<4; i++){
        const SUINT z0=  block[0 + 4*i]     +  (SUINT)block[2 + 4*i];
        const SUINT z1=  block[0 + 4*i]     -  (SUINT)block[2 + 4*i];
        const SUINT z2= (block[1 + 4*i]>>1) -  (SUINT)block[3 + 4*i];
        const SUINT z3=  block[1 + 4*i]     + (SUINT)(block[3 + 4*i]>>1);

        dst[i + 0*stride]= av_clip_pixel(dst[i + 0*stride] + ((int)(z0 + z3) >> 6));
        dst[i + 1*stride]= av_clip_pixel(dst[i + 1*stride] + ((int)(z1 + z2) >> 6));
        dst[i + 2*stride]= av_clip_pixel(dst[i + 2*stride] + ((int)(z1 - z2) >> 6));
        dst[i + 3*stride]= av_clip_pixel(dst[i + 3*stride] + ((int)(z0 - z3) >> 6));
    }

    memset(block, 0, 16 * sizeof(dctcoef));
}
