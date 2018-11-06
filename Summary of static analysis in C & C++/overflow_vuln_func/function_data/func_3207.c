static int decode_p_frame(FourXContext *f, const uint8_t *buf, int length)
{
    int x, y;
    const int width  = f->avctx->width;
    const int height = f->avctx->height;
    uint16_t *src    = (uint16_t *)f->last_picture.data[0];
    uint16_t *dst    = (uint16_t *)f->current_picture.data[0];
    const int stride =             f->current_picture.linesize[0] >> 1;
    unsigned int bitstream_size, bytestream_size, wordstream_size, extra,
                 bytestream_offset, wordstream_offset;

    if (f->version > 1) {
        extra           = 20;
        if (length < extra)
            return -1;
        bitstream_size  = AV_RL32(buf + 8);
        wordstream_size = AV_RL32(buf + 12);
        bytestream_size = AV_RL32(buf + 16);
    } else {
        extra           = 0;
        bitstream_size  = AV_RL16(buf - 4);
        wordstream_size = AV_RL16(buf - 2);
        bytestream_size = FFMAX(length - bitstream_size - wordstream_size, 0);
    }

    if (bitstream_size > length ||
        bytestream_size > length - bitstream_size ||
        wordstream_size > length - bytestream_size - bitstream_size ||
        extra > length - bytestream_size - bitstream_size - wordstream_size) {
        av_log(f->avctx, AV_LOG_ERROR, "lengths %d %d %d %d\n", bitstream_size, bytestream_size, wordstream_size,
        bitstream_size+ bytestream_size+ wordstream_size - length);
        return -1;
    }

    av_fast_malloc(&f->bitstream_buffer, &f->bitstream_buffer_size,
                   bitstream_size + FF_INPUT_BUFFER_PADDING_SIZE);
    if (!f->bitstream_buffer)
        return AVERROR(ENOMEM);
    f->dsp.bswap_buf(f->bitstream_buffer, (const uint32_t*)(buf + extra),
                     bitstream_size / 4);
    memset((uint8_t*)f->bitstream_buffer + bitstream_size,
           0, FF_INPUT_BUFFER_PADDING_SIZE);
    init_get_bits(&f->gb, f->bitstream_buffer, 8 * bitstream_size);

    wordstream_offset = extra + bitstream_size;
    bytestream_offset = extra + bitstream_size + wordstream_size;
    bytestream2_init(&f->g2, buf + wordstream_offset,
                     length - wordstream_offset);
    bytestream2_init(&f->g, buf + bytestream_offset,
                     length - bytestream_offset);

    init_mv(f);

    for (y = 0; y < height; y += 8) {
        for (x = 0; x < width; x += 8)
            decode_p_block(f, dst + x, src + x, 3, 3, stride);
        src += 8 * stride;
        dst += 8 * stride;
    }

    return 0;
}
