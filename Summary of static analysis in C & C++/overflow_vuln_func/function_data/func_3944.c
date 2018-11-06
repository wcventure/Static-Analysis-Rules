// bilinear / bicubic scaling
static void hScale8To15_c(SwsContext *c, int16_t *dst, int dstW, const uint8_t *src,
                          const int16_t *filter, const int16_t *filterPos,
                          int filterSize)
{
    int i;
    for (i=0; i<dstW; i++) {
        int j;
        int srcPos= filterPos[i];
        int val=0;
        for (j=0; j<filterSize; j++) {
            val += ((int)src[srcPos + j])*filter[filterSize*i + j];
        }
        //filter += hFilterSize;
        dst[i] = FFMIN(val>>7, (1<<15)-1); // the cubic equation does overflow ...
        //dst[i] = val>>7;
    }
}
