static int add_hfyu_left_prediction_int16_c(uint16_t *dst, const uint16_t *src, unsigned mask, int w, int acc){
    int i;

    for(i=0; i<w-1; i++){
        acc+= src[i];
        dst[i]= acc & mask;
        i++;
        acc+= src[i];
        dst[i]= acc & mask;
    }

    for(; i<w; i++){
        acc+= src[i];
        dst[i]= acc & mask;
    }

    return acc;
}
