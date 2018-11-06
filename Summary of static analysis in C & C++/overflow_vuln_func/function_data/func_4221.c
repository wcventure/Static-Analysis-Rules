static void pred4x4_horizontal_vp8_c(uint8_t *src, const uint8_t *topright, int stride){
    const int lt= src[-1-1*stride];
    LOAD_LEFT_EDGE

    AV_WN32A(src+0*stride, ((lt + 2*l0 + l1 + 2) >> 2)*0x01010101);
    AV_WN32A(src+1*stride, ((l0 + 2*l1 + l2 + 2) >> 2)*0x01010101);
    AV_WN32A(src+2*stride, ((l1 + 2*l2 + l3 + 2) >> 2)*0x01010101);
    AV_WN32A(src+3*stride, ((l2 + 2*l3 + l3 + 2) >> 2)*0x01010101);
}
