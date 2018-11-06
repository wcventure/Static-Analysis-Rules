static void subband_scale(int *dst, int *src, int scale, int offset, int len)
{
    int ssign = scale < 0 ? -1 : 1;
    int s = FFABS(scale);
    unsigned int round;
    int i, out, c = exp2tab[s & 3];

    s = offset - (s >> 2);

    if (s > 0) {
        round = 1 << (s-1);
        for (i=0; i<len; i++) {
            out = (int)(((int64_t)src[i] * c) >> 32);
            dst[i] = ((int)(out+round) >> s) * ssign;
        }
    }
    else {
        s = s + 32;
        round = 1 << (s-1);
        for (i=0; i<len; i++) {
            out = (int)((int64_t)((int64_t)src[i] * c + round) >> s);
            dst[i] = out * ssign;
        }
    }
}
