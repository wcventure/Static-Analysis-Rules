static int decode_lowdelay(DiracContext *s)
{
    AVCodecContext *avctx = s->avctx;
    int slice_x, slice_y, bytes = 0, bufsize;
    const uint8_t *buf;
    DiracSlice *slices;
    int slice_num = 0;

    slices = av_mallocz_array(s->num_x, s->num_y * sizeof(DiracSlice));
    if (!slices)
        return AVERROR(ENOMEM);

    align_get_bits(&s->gb);
    /
    buf = s->gb.buffer + get_bits_count(&s->gb)/8;
    bufsize = get_bits_left(&s->gb);

    if (s->hq_picture) {
        int i;

        for (slice_y = 0; bufsize > 0 && slice_y < s->num_y; slice_y++) {
            for (slice_x = 0; bufsize > 0 && slice_x < s->num_x; slice_x++) {
                bytes = s->highquality.prefix_bytes + 1;
                for (i = 0; i < 3; i++) {
                    if (bytes <= bufsize/8)
                        bytes += buf[bytes] * s->highquality.size_scaler + 1;
                }

                slices[slice_num].bytes   = bytes;
                slices[slice_num].slice_x = slice_x;
                slices[slice_num].slice_y = slice_y;
                init_get_bits(&slices[slice_num].gb, buf, bufsize);
                slice_num++;

                buf     += bytes;
                if (bufsize/8 >= bytes)
                    bufsize -= bytes*8;
                else
                    bufsize = 0;
            }
        }
        avctx->execute(avctx, decode_hq_slice, slices, NULL, slice_num,
                       sizeof(DiracSlice));
    } else {
        for (slice_y = 0; bufsize > 0 && slice_y < s->num_y; slice_y++) {
            for (slice_x = 0; bufsize > 0 && slice_x < s->num_x; slice_x++) {
                bytes = (slice_num+1) * s->lowdelay.bytes.num / s->lowdelay.bytes.den
                    - slice_num    * s->lowdelay.bytes.num / s->lowdelay.bytes.den;
                slices[slice_num].bytes   = bytes;
                slices[slice_num].slice_x = slice_x;
                slices[slice_num].slice_y = slice_y;
                init_get_bits(&slices[slice_num].gb, buf, bufsize);
                slice_num++;

                buf     += bytes;
                if (bufsize/8 >= bytes)
                    bufsize -= bytes*8;
                else
                    bufsize = 0;
            }
        }
        avctx->execute(avctx, decode_lowdelay_slice, slices, NULL, slice_num,
                       sizeof(DiracSlice)); /
    }

    if (s->dc_prediction) {
        if (s->pshift) {
            intra_dc_prediction_10(&s->plane[0].band[0][0]); /
            intra_dc_prediction_10(&s->plane[1].band[0][0]); /
            intra_dc_prediction_10(&s->plane[2].band[0][0]); /
        } else {
            intra_dc_prediction_8(&s->plane[0].band[0][0]);
            intra_dc_prediction_8(&s->plane[1].band[0][0]);
            intra_dc_prediction_8(&s->plane[2].band[0][0]);
        }
    }
    av_free(slices);
    return 0;
}
