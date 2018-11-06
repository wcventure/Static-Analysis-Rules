static void pred8x8_left_dc_rv40_c(uint8_t *src, int stride){
    int i;
    int dc0;

    dc0=0;
    for(i=0;i<8; i++)
        dc0+= src[-1+i*stride];
    dc0= 0x01010101*((dc0 + 4)>>3);

    for(i=0; i<8; i++){
        ((uint32_t*)(src+i*stride))[0]=
        ((uint32_t*)(src+i*stride))[1]= dc0;
    }
}
