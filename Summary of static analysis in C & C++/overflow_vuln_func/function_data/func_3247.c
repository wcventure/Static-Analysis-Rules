static void chrRangeToJpeg16_c(int16_t *_dstU, int16_t *_dstV, int width)
{
    int i;
    int32_t *dstU = (int32_t *) _dstU;
    int32_t *dstV = (int32_t *) _dstV;
    for (i = 0; i < width; i++) {
        dstU[i] = (FFMIN(dstU[i],30775<<4)*4663 - (9289992<<4))>>12; //-264
        dstV[i] = (FFMIN(dstV[i],30775<<4)*4663 - (9289992<<4))>>12; //-264
    }
}
static void chrRangeFromJpeg16_c(int16_t *_dstU, int16_t *_dstV, int width)
{
    int i;
    int32_t *dstU = (int32_t *) _dstU;
    int32_t *dstV = (int32_t *) _dstV;
    for (i = 0; i < width; i++) {
        dstU[i] = (dstU[i]*1799 + (4081085<<4))>>11; //1469
        dstV[i] = (dstV[i]*1799 + (4081085<<4))>>11; //1469
    }
}
static void lumRangeToJpeg16_c(int16_t *_dst, int width)
{
    int i;
    int32_t *dst = (int32_t *) _dst;
    for (i = 0; i < width; i++)
        dst[i] = (FFMIN(dst[i],30189<<4)*19077 - (39057361<<4))>>14;
}
