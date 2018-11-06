static void pred4x4_vertical_vp8_c(uint8_t *src, const uint8_t *topright, int stride){
    const int lt= src[-1-1*stride];
    LOAD_TOP_EDGE
    LOAD_TOP_RIGHT_EDGE
    uint32_t v = PACK_4U8((lt + 2*t0 + t1 + 2) >> 2,
                          (t0 + 2*t1 + t2 + 2) >> 2,
                          (t1 + 2*t2 + t3 + 2) >> 2,
                          (t2 + 2*t3 + t4 + 2) >> 2);

    AV_WN32A(src+0*stride, v);
    AV_WN32A(src+1*stride, v);
    AV_WN32A(src+2*stride, v);
    AV_WN32A(src+3*stride, v);
}
