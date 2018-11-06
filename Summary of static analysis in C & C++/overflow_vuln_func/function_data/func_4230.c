static int decode_i_frame(FourXContext *f, const uint8_t *buf, int length){
    int x, y;
    const int width= f->avctx->width;
    const int height= f->avctx->height;
    uint16_t *dst= (uint16_t*)f->current_picture.data[0];
    const int stride= f->current_picture.linesize[0]>>1;
    const unsigned int bitstream_size= AV_RL32(buf);
    const int token_count av_unused = AV_RL32(buf + bitstream_size + 8);
    unsigned int prestream_size= 4*AV_RL32(buf + bitstream_size + 4);
    const uint8_t *prestream= buf + bitstream_size + 12;

    if(prestream_size + bitstream_size + 12 != length
       || bitstream_size > (1<<26)
       || prestream_size > (1<<26)){
        av_log(f->avctx, AV_LOG_ERROR, "size mismatch %d %d %d\n", prestream_size, bitstream_size, length);
        return -1;
    }

    prestream= read_huffman_tables(f, prestream);

    init_get_bits(&f->gb, buf + 4, 8*bitstream_size);

    prestream_size= length + buf - prestream;

    av_fast_malloc(&f->bitstream_buffer, &f->bitstream_buffer_size, prestream_size + FF_INPUT_BUFFER_PADDING_SIZE);
    if (!f->bitstream_buffer)
        return AVERROR(ENOMEM);
    f->dsp.bswap_buf(f->bitstream_buffer, (const uint32_t*)prestream, prestream_size/4);
    memset((uint8_t*)f->bitstream_buffer + prestream_size, 0, FF_INPUT_BUFFER_PADDING_SIZE);
    init_get_bits(&f->pre_gb, f->bitstream_buffer, 8*prestream_size);

    f->last_dc= 0*128*8*8;

    for(y=0; y<height; y+=16){
        for(x=0; x<width; x+=16){
            if(decode_i_mb(f) < 0)
                return -1;

            idct_put(f, x, y);
        }
        dst += 16*stride;
    }

    if(get_vlc2(&f->pre_gb, f->pre_vlc.table, ACDC_VLC_BITS, 3) != 256)
        av_log(f->avctx, AV_LOG_ERROR, "end mismatch\n");

    return 0;
}
