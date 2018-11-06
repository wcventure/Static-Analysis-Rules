void FUNCC(ff_h264_chroma422_dc_dequant_idct)(int16_t *_block, int qmul){
    const int stride= 16*2;
    const int xStride= 16;
    int i;
    int temp[8];
    static const uint8_t x_offset[2]={0, 16};
    dctcoef *block = (dctcoef*)_block;

    for(i=0; i<4; i++){
        temp[2*i+0] = block[stride*i + xStride*0] + block[stride*i + xStride*1];
        temp[2*i+1] = block[stride*i + xStride*0] - block[stride*i + xStride*1];
    }

    for(i=0; i<2; i++){
        const int offset= x_offset[i];
        const int z0= temp[2*0+i] + temp[2*2+i];
        const int z1= temp[2*0+i] - temp[2*2+i];
        const int z2= temp[2*1+i] - temp[2*3+i];
        const int z3= temp[2*1+i] + temp[2*3+i];

        block[stride*0+offset]= ((z0 + z3)*qmul + 128) >> 8;
        block[stride*1+offset]= ((z1 + z2)*qmul + 128) >> 8;
        block[stride*2+offset]= ((z1 - z2)*qmul + 128) >> 8;
        block[stride*3+offset]= ((z0 - z3)*qmul + 128) >> 8;
    }
}
