static void qtrle_decode_16bpp(QtrleContext *s)
{
    int stream_ptr;
    int header;
    int start_line;
    int lines_to_change;
    signed char rle_code;
    int row_ptr, pixel_ptr;
    int row_inc = s->frame.linesize[0];
    unsigned short rgb16;
    unsigned char *rgb = s->frame.data[0];
    int pixel_limit = s->frame.linesize[0] * s->avctx->height;

    /
    if (s->size < 8)
        return;

    /
    stream_ptr = 4;

    /
    CHECK_STREAM_PTR(2);
    header = BE_16(&s->buf[stream_ptr]);
    stream_ptr += 2;

    /
    if (header & 0x0008) {
        CHECK_STREAM_PTR(8);
        start_line = BE_16(&s->buf[stream_ptr]);
        stream_ptr += 4;
        lines_to_change = BE_16(&s->buf[stream_ptr]);
        stream_ptr += 4;
    } else {
        start_line = 0;
        lines_to_change = s->avctx->height;
    }

    row_ptr = row_inc * start_line;
    while (lines_to_change--) {
        CHECK_STREAM_PTR(2);
        pixel_ptr = row_ptr + (s->buf[stream_ptr++] - 1) * 2;

        while ((rle_code = (signed char)s->buf[stream_ptr++]) != -1) {
            if (rle_code == 0) {
                /
                CHECK_STREAM_PTR(1);
                pixel_ptr += (s->buf[stream_ptr++] - 1) * 2;
                CHECK_PIXEL_PTR(0);  /
            } else if (rle_code < 0) {
                /
                rle_code = -rle_code;
                CHECK_STREAM_PTR(2);
                rgb16 = BE_16(&s->buf[stream_ptr]);
                stream_ptr += 2;

                CHECK_PIXEL_PTR(rle_code * 2);

                while (rle_code--) {
                    *(unsigned short *)(&rgb[pixel_ptr]) = rgb16;
                    pixel_ptr += 2;
                }
            } else {
                CHECK_STREAM_PTR(rle_code * 2);
                CHECK_PIXEL_PTR(rle_code * 2);

                /
                while (rle_code--) {
                    rgb16 = BE_16(&s->buf[stream_ptr]);
                    stream_ptr += 2;
                    *(unsigned short *)(&rgb[pixel_ptr]) = rgb16;
                    pixel_ptr += 2;
                }
            }
        }
        row_ptr += row_inc;
    }
}
