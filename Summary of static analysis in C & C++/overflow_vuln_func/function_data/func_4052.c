static void FUNCC(ff_h264_add_pixels4)(uint8_t *_dst, int16_t *_src, int stride)
{
    int i;
    pixel *dst = (pixel *) _dst;
    dctcoef *src = (dctcoef *) _src;
    stride /= sizeof(pixel);

    for (i = 0; i < 4; i++) {
        dst[0] += src[0];
        dst[1] += src[1];
        dst[2] += src[2];
        dst[3] += src[3];

        dst += stride;
        src += 4;
    }

    memset(_src, 0, sizeof(dctcoef) * 16);
}
