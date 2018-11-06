static void FUNCC(ff_h264_add_pixels8)(uint8_t *_dst, int16_t *_src, int stride)
{
    int i;
    pixel *dst = (pixel *) _dst;
    dctcoef *src = (dctcoef *) _src;
    stride /= sizeof(pixel);

    for (i = 0; i < 8; i++) {
        dst[0] += src[0];
        dst[1] += src[1];
        dst[2] += src[2];
        dst[3] += src[3];
        dst[4] += src[4];
        dst[5] += src[5];
        dst[6] += src[6];
        dst[7] += src[7];

        dst += stride;
        src += 8;
    }

    memset(_src, 0, sizeof(dctcoef) * 64);
}
