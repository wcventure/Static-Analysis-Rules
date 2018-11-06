    return out;
}
// 16.16 fixpoint log()
static uint64_t log16(uint64_t a){
    int i;
    int out=0;
    
    assert(a >= (1<<16));
    a<<=16;
    
    for(i=19;i>=0;i--){
        if(a<(exp16_table[i]<<16)) continue;
        out |= 1<<i;
        a = ((a<<16) + exp16_table[i]/2)/exp16_table[i];
    }
    return out;
}
