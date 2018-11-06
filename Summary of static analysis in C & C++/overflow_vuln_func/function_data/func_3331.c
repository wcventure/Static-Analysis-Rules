static inline void bink_idct_col(DCTELEM *dest, const DCTELEM *src)
{
    if ((src[8]|src[16]|src[24]|src[32]|src[40]|src[48]|src[56])==0) {
        dest[0]  =
        dest[8]  =
        dest[16] =
        dest[24] =
        dest[32] =
        dest[40] =
        dest[48] =
        dest[56] = src[0];
    } else {
        IDCT_COL(dest, src);
    }
}
