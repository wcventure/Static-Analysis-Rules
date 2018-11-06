static void qtrle_decode_32bpp(QtrleContext *s)
{
    int stream_ptr;
    int header;
    int start_line;
    int lines_to_change;
    signed char rle_code;
    int row_ptr, pixel_ptr;
    int row_inc = s->frame.linesize[0];
    unsigned char r, g, b;
    unsigned int argb;
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
        pixel_ptr = row_ptr + (s->buf[stream_ptr++] - 1) * 4;

        while ((rle_code = (signed char)s->buf[stream_ptr++]) != -1) {
            if (rle_code == 0) {
                /
                CHECK_STREAM_PTR(1);
                pixel_ptr += (s->buf[stream_ptr++] - 1) * 4;
                CHECK_PIXEL_PTR(0);  /
            } else if (rle_code < 0) {
                /
                rle_code = -rle_code;
                CHECK_STREAM_PTR(4);
                stream_ptr++;  /
                r = s->buf[stream_ptr++];
                g = s->buf[stream_ptr++];
                b = s->buf[stream_ptr++];
                argb = (r << 16) | (g << 8) | (b << 0);

                CHECK_PIXEL_PTR(rle_code * 4);

                while (rle_code--) {
                    *(unsigned int *)(&rgb[pixel_ptr]) = argb;
                    pixel_ptr += 4;
                }
            } else {
                CHECK_STREAM_PTR(rle_code * 4);
                CHECK_PIXEL_PTR(rle_code * 4);

                /
                while (rle_code--) {
                    stream_ptr++;  /
                    r = s->buf[stream_ptr++];
                    g = s->buf[stream_ptr++];
                    b = s->buf[stream_ptr++];
                    argb = (r << 16) | (g << 8) | (b << 0);
                    *(unsigned int *)(&rgb[pixel_ptr]) = argb;
                    pixel_ptr += 4;
                }
            }
        }
        row_ptr += row_inc;
    }
}
