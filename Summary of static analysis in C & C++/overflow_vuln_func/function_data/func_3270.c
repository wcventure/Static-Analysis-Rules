static void ict_int(void *_src0, void *_src1, void *_src2, int csize)
{
    int32_t *src0 = _src0, *src1 = _src1, *src2 = _src2;
    int32_t i0, i1, i2;
    int i;

    for (i = 0; i < csize; i++) {
        i0 = *src0 + (((i_ict_params[0] * *src2) + (1 << 15)) >> 16);
        i1 = *src0 - (((i_ict_params[1] * *src1) + (1 << 15)) >> 16)
                   - (((i_ict_params[2] * *src2) + (1 << 15)) >> 16);
        i2 = *src0 + (((i_ict_params[3] * *src1) + (1 << 15)) >> 16);
        *src0++ = i0;
        *src1++ = i1;
        *src2++ = i2;
    }
}
