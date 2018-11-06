static av_always_inline void
yuv2mono_2_c_template(SwsContext *c, const int16_t *buf[2],
                      const int16_t *ubuf[2], const int16_t *vbuf[2],
                      const int16_t *abuf[2], uint8_t *dest, int dstW,
                      int yalpha, int uvalpha, int y,
                      enum PixelFormat target)
{
    const int16_t *buf0  = buf[0],  *buf1  = buf[1];
    const uint8_t * const d128 = dither_8x8_220[y & 7];
    int  yalpha1 = 4095 - yalpha;
    int i;

    for (i = 0; i < dstW; i += 8) {
        int Y, acc = 0;

        Y = (buf0[i + 0] * yalpha1 + buf1[i + 0] * yalpha) >> 19;
        accumulate_bit(acc, Y + d128[0]);
        Y = (buf0[i + 1] * yalpha1 + buf1[i + 1] * yalpha) >> 19;
        accumulate_bit(acc, Y + d128[1]);
        Y = (buf0[i + 2] * yalpha1 + buf1[i + 2] * yalpha) >> 19;
        accumulate_bit(acc, Y + d128[2]);
        Y = (buf0[i + 3] * yalpha1 + buf1[i + 3] * yalpha) >> 19;
        accumulate_bit(acc, Y + d128[3]);
        Y = (buf0[i + 4] * yalpha1 + buf1[i + 4] * yalpha) >> 19;
        accumulate_bit(acc, Y + d128[4]);
        Y = (buf0[i + 5] * yalpha1 + buf1[i + 5] * yalpha) >> 19;
        accumulate_bit(acc, Y + d128[5]);
        Y = (buf0[i + 6] * yalpha1 + buf1[i + 6] * yalpha) >> 19;
        accumulate_bit(acc, Y + d128[6]);
        Y = (buf0[i + 7] * yalpha1 + buf1[i + 7] * yalpha) >> 19;
        accumulate_bit(acc, Y + d128[7]);

        output_pixel(*dest++, acc);
    }
}
