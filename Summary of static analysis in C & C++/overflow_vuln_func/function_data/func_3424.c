double av_int2dbl(int64_t v){
    if(v+v > 0xFFEULL<<52)
        return NAN;
    return ldexp(((v&((1LL<<52)-1)) + (1LL<<52)) * (v>>63|1), (v>>52&0x7FF)-1075);
}
