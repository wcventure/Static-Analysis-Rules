static always_inline uint8_t vc1_mspel_filter(const uint8_t *src, int stride, int mode, int r)
{
    switch(mode){
    case 0: //no shift
        return src[0];
    case 1: // 1/4 shift
        return (-4*src[-stride] + 53*src[0] + 18*src[stride] - 3*src[stride*2] + 32 - r) >> 6;
    case 2: // 1/2 shift
        return (-src[-stride] + 9*src[0] + 9*src[stride] - src[stride*2] + 8 - r) >> 4;
    case 3: // 3/4 shift
        return (-3*src[-stride] + 18*src[0] + 53*src[stride] - 4*src[stride*2] + 32 - r) >> 6;
    }
    return 0; //should not occur
}
