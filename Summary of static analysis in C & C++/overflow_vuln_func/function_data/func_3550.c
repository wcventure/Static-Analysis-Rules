static void picmemset(PicContext *s, AVFrame *frame, int value, int run,
                      int *x, int *y, int *plane, int bits_per_plane)
{
    uint8_t *d;
    int shift = *plane * bits_per_plane;
    int mask  = ((1 << bits_per_plane) - 1) << shift;
    value   <<= shift;

    while (run > 0) {
        int j;
        for (j = 8-bits_per_plane; j >= 0; j -= bits_per_plane) {
            d = frame->data[0] + *y * frame->linesize[0];
            d[*x] |= (value >> j) & mask;
            *x += 1;
            if (*x == s->width) {
                *y -= 1;
                *x = 0;
                if (*y < 0) {
                   *y = s->height - 1;
                   *plane += 1;
                   value <<= bits_per_plane;
                   mask  <<= bits_per_plane;
                   if (*plane >= s->nb_planes)
                       return;
                }
            }
        }
        run--;
    }
}
