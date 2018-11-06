static void pred8x8_dc_rv40_c(uint8_t *src, int stride){
    int i;
    int dc0=0;

    for(i=0;i<4; i++){
        dc0+= src[-1+i*stride] + src[i-stride];
        dc0+= src[4+i-stride];
        dc0+= src[-1+(i+4)*stride];
    }
    dc0= 0x01010101*((dc0 + 8)>>4);

    for(i=0; i<4; i++){
        ((uint32_t*)(src+i*stride))[0]= dc0;
        ((uint32_t*)(src+i*stride))[1]= dc0;
    }
    for(i=4; i<8; i++){
        ((uint32_t*)(src+i*stride))[0]= dc0;
        ((uint32_t*)(src+i*stride))[1]= dc0;
    }
}
