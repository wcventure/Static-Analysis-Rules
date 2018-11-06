static av_always_inline void iadst4_1d(const dctcoef *in, ptrdiff_t stride,
                                       dctcoef *out, int pass)
{
    int t0, t1, t2, t3;

    t0 =  5283 * IN(0) + 15212 * IN(2) +  9929 * IN(3);
    t1 =  9929 * IN(0) -  5283 * IN(2) - 15212 * IN(3);
    t2 = 13377 * (IN(0) - IN(2) + IN(3));
    t3 = 13377 * IN(1);

    out[0] = (t0 + t3      + (1 << 13)) >> 14;
    out[1] = (t1 + t3      + (1 << 13)) >> 14;
    out[2] = (t2           + (1 << 13)) >> 14;
    out[3] = (t0 + t1 - t3 + (1 << 13)) >> 14;
}
