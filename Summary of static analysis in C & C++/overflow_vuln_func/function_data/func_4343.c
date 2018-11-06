static inline void scale_mv(AVSContext *h, int *d_x, int *d_y,
                            cavs_vector *src, int distp)
{
    int den = h->scale_den[FFMAX(src->ref, 0)];

    *d_x = (src->x * distp * den + 256 + FF_SIGNBIT(src->x)) >> 9;
    *d_y = (src->y * distp * den + 256 + FF_SIGNBIT(src->y)) >> 9;
}
