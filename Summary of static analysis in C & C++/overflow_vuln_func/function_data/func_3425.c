float av_int2flt(int32_t v){
    if(v+v > 0xFF000000U)
        return NAN;
    return ldexp(((v&0x7FFFFF) + (1<<23)) * (v>>31|1), (v>>23&0xFF)-150);
}
