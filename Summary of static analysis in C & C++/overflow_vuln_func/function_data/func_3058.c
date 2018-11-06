static int decode_lowdelay(DiracContext *s)
{
    AVCodecContext *avctx = s->avctx;
    int slice_x, slice_y, bytes, bufsize;
    const uint8_t *buf;
    struct lowdelay_slice *slices;
    int slice_num = 0;

    slices = av_mallocz_array(s->lowdelay.num_x, s->lowdelay.num_y * sizeof(struct lowdelay_slice));
    if (!slices)
        return AVERROR(ENOMEM);

    align_get_bits(&s->gb);
    /
    buf = s->gb.buffer + get_bits_count(&s->gb)/8;
    bufsize = get_bits_left(&s->gb);

    for (slice_y = 0; bufsize > 0 && slice_y < s->lowdelay.num_y; slice_y++)
        for (slice_x = 0; bufsize > 0 && slice_x < s->lowdelay.num_x; slice_x++) {
            bytes = (slice_num+1) * s->lowdelay.bytes.num / s->lowdelay.bytes.den
                - slice_num    * s->lowdelay.bytes.num / s->lowdelay.bytes.den;

            slices[slice_num].bytes   = bytes;
            slices[slice_num].slice_x = slice_x;
            slices[slice_num].slice_y = slice_y;
            init_get_bits(&slices[slice_num].gb, buf, bufsize);
            slice_num++;

            buf     += bytes;
            bufsize -= bytes*8;
        }

    avctx->execute(avctx, decode_lowdelay_slice, slices, NULL, slice_num,
                   sizeof(struct lowdelay_slice)); /
    intra_dc_prediction(&s->plane[0].band[0][0]);  /
    intra_dc_prediction(&s->plane[1].band[0][0]);  /
    intra_dc_prediction(&s->plane[2].band[0][0]);  /
    av_free(slices);
    return 0;
}
