static int tgv_decode_inter(TgvContext * s, const uint8_t *buf, const uint8_t *buf_end){
    unsigned char *frame0_end = s->last_frame.data[0] + s->avctx->height*s->last_frame.linesize[0];
    int num_mvs;
    int num_blocks_raw;
    int num_blocks_packed;
    int vector_bits;
    int i,j,x,y;
    GetBitContext gb;
    int mvbits;
    const unsigned char *blocks_raw;

    if(buf+12>buf_end)
        return -1;

    num_mvs           = AV_RL16(&buf[0]);
    num_blocks_raw    = AV_RL16(&buf[2]);
    num_blocks_packed = AV_RL16(&buf[4]);
    vector_bits       = AV_RL16(&buf[6]);
    buf += 12;

    /
    if (num_mvs > s->num_mvs) {
        s->mv_codebook = av_realloc(s->mv_codebook, num_mvs*2*sizeof(int));
        s->num_mvs = num_mvs;
    }

    if (num_blocks_packed > s->num_blocks_packed) {
        s->block_codebook = av_realloc(s->block_codebook, num_blocks_packed*16*sizeof(unsigned char));
        s->num_blocks_packed = num_blocks_packed;
    }

    /
    mvbits = (num_mvs*2*10+31) & ~31;

    if (buf+(mvbits>>3)+16*num_blocks_raw+8*num_blocks_packed>buf_end)
        return -1;

    init_get_bits(&gb, buf, mvbits);
    for (i=0; i<num_mvs; i++) {
        s->mv_codebook[i][0] = get_sbits(&gb, 10);
        s->mv_codebook[i][1] = get_sbits(&gb, 10);
    }
    buf += mvbits>>3;

    /
    blocks_raw = buf;
    buf += num_blocks_raw*16;

    /
    init_get_bits(&gb, buf, (buf_end-buf)<<3);
    for (i=0; i<num_blocks_packed; i++) {
        int tmp[4];
        for(j=0; j<4; j++)
            tmp[j] = get_bits(&gb, 8);
        for(j=0; j<16; j++)
            s->block_codebook[i][15-j] = tmp[get_bits(&gb, 2)];
    }

    if (get_bits_left(&gb) < vector_bits *
        (s->avctx->height/4) * (s->avctx->width/4))
        return -1;

    /
    for(y=0; y<s->avctx->height/4; y++)
    for(x=0; x<s->avctx->width/4; x++) {
        unsigned int vector = get_bits(&gb, vector_bits);
        const unsigned char *src;
        int src_stride;

        if (vector < num_mvs) {
            src = s->last_frame.data[0] +
                  (y*4 + s->mv_codebook[vector][1])*s->last_frame.linesize[0] +
                   x*4 + s->mv_codebook[vector][0];
            src_stride = s->last_frame.linesize[0];
            if (src < s->last_frame.data[0] || src+3*src_stride+3>=frame0_end)
                continue;
        }else{
            int offset = vector - num_mvs;
            if (offset<num_blocks_raw)
                src = blocks_raw + 16*offset;
            else if (offset-num_blocks_raw<num_blocks_packed)
                src = s->block_codebook[offset-num_blocks_raw];
            else
                continue;
            src_stride = 4;
        }

        for(j=0; j<4; j++)
        for(i=0; i<4; i++)
            s->frame.data[0][ (y*4+j)*s->frame.linesize[0] + (x*4+i)  ] =
               src[j*src_stride + i];
    }

    return 0;
}
