static int decode_p_frame(FourXContext *f, const uint8_t *buf, int length){
    int x, y;
    const int width= f->avctx->width;
    const int height= f->avctx->height;
    uint16_t *src= (uint16_t*)f->last_picture.data[0];
    uint16_t *dst= (uint16_t*)f->current_picture.data[0];
    const int stride= f->current_picture.linesize[0]>>1;
    unsigned int bitstream_size, bytestream_size, wordstream_size, extra;

    if(f->version>1){
        extra=20;
        bitstream_size= AV_RL32(buf+8);
        wordstream_size= AV_RL32(buf+12);
        bytestream_size= AV_RL32(buf+16);
    }else{
        extra=0;
        bitstream_size = AV_RL16(buf-4);
        wordstream_size= AV_RL16(buf-2);
        bytestream_size= FFMAX(length - bitstream_size - wordstream_size, 0);
    }

    if(bitstream_size+ bytestream_size+ wordstream_size + extra != length
       || bitstream_size  > (1<<26)
       || bytestream_size > (1<<26)
       || wordstream_size > (1<<26)
       ){
        av_log(f->avctx, AV_LOG_ERROR, "lengths %d %d %d %d\n", bitstream_size, bytestream_size, wordstream_size,
        bitstream_size+ bytestream_size+ wordstream_size - length);
        return -1;
    }

    av_fast_malloc(&f->bitstream_buffer, &f->bitstream_buffer_size, bitstream_size + FF_INPUT_BUFFER_PADDING_SIZE);
    if (!f->bitstream_buffer)
        return AVERROR(ENOMEM);
    f->dsp.bswap_buf(f->bitstream_buffer, (const uint32_t*)(buf + extra), bitstream_size/4);
    memset((uint8_t*)f->bitstream_buffer + bitstream_size, 0, FF_INPUT_BUFFER_PADDING_SIZE);
    init_get_bits(&f->gb, f->bitstream_buffer, 8*bitstream_size);

    f->wordstream= (const uint16_t*)(buf + extra + bitstream_size);
    f->bytestream= buf + extra + bitstream_size + wordstream_size;

    init_mv(f);

    for(y=0; y<height; y+=8){
        for(x=0; x<width; x+=8){
            decode_p_block(f, dst + x, src + x, 3, 3, stride);
        }
        src += 8*stride;
        dst += 8*stride;
    }

    if(   bitstream_size != (get_bits_count(&f->gb)+31)/32*4
       || (((const char*)f->wordstream - (const char*)buf + 2)&~2) != extra + bitstream_size + wordstream_size
       || (((const char*)f->bytestream - (const char*)buf + 3)&~3) != extra + bitstream_size + wordstream_size + bytestream_size)
        av_log(f->avctx, AV_LOG_ERROR, " %d %td %td bytes left\n",
            bitstream_size - (get_bits_count(&f->gb)+31)/32*4,
            -(((const char*)f->bytestream - (const char*)buf + 3)&~3) + (extra + bitstream_size + wordstream_size + bytestream_size),
            -(((const char*)f->wordstream - (const char*)buf + 2)&~2) + (extra + bitstream_size + wordstream_size)
        );

    return 0;
}
