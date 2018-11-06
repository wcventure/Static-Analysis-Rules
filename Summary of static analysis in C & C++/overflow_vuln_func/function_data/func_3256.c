static inline int l1_unscale(int n, int mant, int scale_factor)
{
    int shift, mod;
    INT64 val;

    shift = scale_factor_modshift[scale_factor];
    mod = shift & 3;
    shift >>= 2;
    val = MUL64(mant + (-1 << n) + 1, scale_factor_mult[n-1][mod]);
    shift += n;
    return (int)((val + (1 << (shift - 1))) >> shift);
}
