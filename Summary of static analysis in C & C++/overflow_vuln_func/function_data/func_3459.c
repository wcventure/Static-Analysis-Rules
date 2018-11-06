static void vp7_idct_add_c(uint8_t *dst, int16_t block[16], ptrdiff_t stride)
{
    int i, a1, b1, c1, d1;
    int16_t tmp[16];

    for (i = 0; i < 4; i++) {
        a1 = (block[i * 4 + 0] + block[i * 4 + 2]) * 23170;
        b1 = (block[i * 4 + 0] - block[i * 4 + 2]) * 23170;
        c1 = block[i * 4 + 1] * 12540 - block[i * 4 + 3] * 30274;
        d1 = block[i * 4 + 1] * 30274 + block[i * 4 + 3] * 12540;
        AV_ZERO64(block + i * 4);
        tmp[i * 4 + 0] = (a1 + d1) >> 14;
        tmp[i * 4 + 3] = (a1 - d1) >> 14;
        tmp[i * 4 + 1] = (b1 + c1) >> 14;
        tmp[i * 4 + 2] = (b1 - c1) >> 14;
    }

    for (i = 0; i < 4; i++) {
        a1 = (tmp[i + 0] + tmp[i + 8]) * 23170;
        b1 = (tmp[i + 0] - tmp[i + 8]) * 23170;
        c1 = tmp[i + 4] * 12540 - tmp[i + 12] * 30274;
        d1 = tmp[i + 4] * 30274 + tmp[i + 12] * 12540;
        dst[0 * stride + i] = av_clip_uint8(dst[0 * stride + i] +
                                            ((a1 + d1 + 0x20000) >> 18));
        dst[3 * stride + i] = av_clip_uint8(dst[3 * stride + i] +
                                            ((a1 - d1 + 0x20000) >> 18));
        dst[1 * stride + i] = av_clip_uint8(dst[1 * stride + i] +
                                            ((b1 + c1 + 0x20000) >> 18));
        dst[2 * stride + i] = av_clip_uint8(dst[2 * stride + i] +
                                            ((b1 - c1 + 0x20000) >> 18));
    }
}
