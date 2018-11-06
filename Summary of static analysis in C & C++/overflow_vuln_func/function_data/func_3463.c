#ifdef EXTRA_SHIFT
static inline void FUNC(idctSparseCol_extrashift)(int16_t *col)
#else
static inline void FUNC(idctSparseColPut)(pixel *dest, int line_size,
                                          int16_t *col)
{
    int a0, a1, a2, a3, b0, b1, b2, b3;

    IDCT_COLS;

    dest[0] = av_clip_pixel((a0 + b0) >> COL_SHIFT);
    dest += line_size;
    dest[0] = av_clip_pixel((a1 + b1) >> COL_SHIFT);
    dest += line_size;
    dest[0] = av_clip_pixel((a2 + b2) >> COL_SHIFT);
    dest += line_size;
    dest[0] = av_clip_pixel((a3 + b3) >> COL_SHIFT);
    dest += line_size;
    dest[0] = av_clip_pixel((a3 - b3) >> COL_SHIFT);
    dest += line_size;
    dest[0] = av_clip_pixel((a2 - b2) >> COL_SHIFT);
    dest += line_size;
    dest[0] = av_clip_pixel((a1 - b1) >> COL_SHIFT);
    dest += line_size;
    dest[0] = av_clip_pixel((a0 - b0) >> COL_SHIFT);
}
