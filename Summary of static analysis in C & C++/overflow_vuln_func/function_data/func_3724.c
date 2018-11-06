void FUNCC(ff_h264_idct8_add)(uint8_t *_dst, int16_t *_block, int stride){
    int i;
    pixel *dst = (pixel*)_dst;
    dctcoef *block = (dctcoef*)_block;
    stride >>= sizeof(pixel)-1;

    block[0] += 32;

    for( i = 0; i < 8; i++ )
    {
        const unsigned int a0 =  block[i+0*8] + block[i+4*8];
        const unsigned int a2 =  block[i+0*8] - block[i+4*8];
        const unsigned int a4 = (block[i+2*8]>>1) - block[i+6*8];
        const unsigned int a6 = (block[i+6*8]>>1) + block[i+2*8];

        const unsigned int b0 = a0 + a6;
        const unsigned int b2 = a2 + a4;
        const unsigned int b4 = a2 - a4;
        const unsigned int b6 = a0 - a6;

        const int a1 = -block[i+3*8] + (unsigned)block[i+5*8] - block[i+7*8] - (block[i+7*8]>>1);
        const int a3 =  block[i+1*8] + (unsigned)block[i+7*8] - block[i+3*8] - (block[i+3*8]>>1);
        const int a5 = -block[i+1*8] + (unsigned)block[i+7*8] + block[i+5*8] + (block[i+5*8]>>1);
        const int a7 =  block[i+3*8] + (unsigned)block[i+5*8] + block[i+1*8] + (block[i+1*8]>>1);

        const int b1 = (a7>>2) + a1;
        const int b3 =  a3 + (a5>>2);
        const int b5 = (a3>>2) - a5;
        const int b7 =  a7 - (a1>>2);

        block[i+0*8] = b0 + b7;
        block[i+7*8] = b0 - b7;
        block[i+1*8] = b2 + b5;
        block[i+6*8] = b2 - b5;
        block[i+2*8] = b4 + b3;
        block[i+5*8] = b4 - b3;
        block[i+3*8] = b6 + b1;
        block[i+4*8] = b6 - b1;
    }
    for( i = 0; i < 8; i++ )
    {
        const unsigned a0 =  block[0+i*8] + block[4+i*8];
        const unsigned a2 =  block[0+i*8] - block[4+i*8];
        const unsigned a4 = (block[2+i*8]>>1) - block[6+i*8];
        const unsigned a6 = (block[6+i*8]>>1) + block[2+i*8];

        const unsigned b0 = a0 + a6;
        const unsigned b2 = a2 + a4;
        const unsigned b4 = a2 - a4;
        const unsigned b6 = a0 - a6;

        const int a1 = -(unsigned)block[3+i*8] + block[5+i*8] - block[7+i*8] - (block[7+i*8]>>1);
        const int a3 =  (unsigned)block[1+i*8] + block[7+i*8] - block[3+i*8] - (block[3+i*8]>>1);
        const int a5 = -(unsigned)block[1+i*8] + block[7+i*8] + block[5+i*8] + (block[5+i*8]>>1);
        const int a7 =  (unsigned)block[3+i*8] + block[5+i*8] + block[1+i*8] + (block[1+i*8]>>1);

        const unsigned b1 = (a7>>2) + (unsigned)a1;
        const unsigned b3 =  (unsigned)a3 + (a5>>2);
        const unsigned b5 = (a3>>2) - (unsigned)a5;
        const unsigned b7 =  (unsigned)a7 - (a1>>2);

        dst[i + 0*stride] = av_clip_pixel( dst[i + 0*stride] + ((int)(b0 + b7) >> 6) );
        dst[i + 1*stride] = av_clip_pixel( dst[i + 1*stride] + ((int)(b2 + b5) >> 6) );
        dst[i + 2*stride] = av_clip_pixel( dst[i + 2*stride] + ((int)(b4 + b3) >> 6) );
        dst[i + 3*stride] = av_clip_pixel( dst[i + 3*stride] + ((int)(b6 + b1) >> 6) );
        dst[i + 4*stride] = av_clip_pixel( dst[i + 4*stride] + ((int)(b6 - b1) >> 6) );
        dst[i + 5*stride] = av_clip_pixel( dst[i + 5*stride] + ((int)(b4 - b3) >> 6) );
        dst[i + 6*stride] = av_clip_pixel( dst[i + 6*stride] + ((int)(b2 - b5) >> 6) );
        dst[i + 7*stride] = av_clip_pixel( dst[i + 7*stride] + ((int)(b0 - b7) >> 6) );
    }

    memset(block, 0, 64 * sizeof(dctcoef));
}
