void FUNCC(ff_h264_luma_dc_dequant_idct)(int16_t *_output, int16_t *_input, int qmul){
#define stride 16
    int i;
    int temp[16];
    static const uint8_t x_offset[4]={0, 2*stride, 8*stride, 10*stride};
    dctcoef *input = (dctcoef*)_input;
    dctcoef *output = (dctcoef*)_output;

    for(i=0; i<4; i++){
        const int z0= input[4*i+0] + input[4*i+1];
        const int z1= input[4*i+0] - input[4*i+1];
        const int z2= input[4*i+2] - input[4*i+3];
        const int z3= input[4*i+2] + input[4*i+3];

        temp[4*i+0]= z0+z3;
        temp[4*i+1]= z0-z3;
        temp[4*i+2]= z1-z2;
        temp[4*i+3]= z1+z2;
    }

    for(i=0; i<4; i++){
        const int offset= x_offset[i];
        const int z0= temp[4*0+i] + temp[4*2+i];
        const int z1= temp[4*0+i] - temp[4*2+i];
        const int z2= temp[4*1+i] - temp[4*3+i];
        const int z3= temp[4*1+i] + temp[4*3+i];

        output[stride* 0+offset]= ((((z0 + z3)*qmul + 128 ) >> 8));
        output[stride* 1+offset]= ((((z1 + z2)*qmul + 128 ) >> 8));
        output[stride* 4+offset]= ((((z1 - z2)*qmul + 128 ) >> 8));
        output[stride* 5+offset]= ((((z0 - z3)*qmul + 128 ) >> 8));
    }
#undef stride
}
