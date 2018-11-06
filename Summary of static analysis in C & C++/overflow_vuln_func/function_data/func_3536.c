static void compute_antialias_integer(MPADecodeContext *s,
                              GranuleDef *g)
{
    int32_t *ptr, *csa;
    int n, i;

    /
    if (g->block_type == 2) {
        if (!g->switch_point)
            return;
        /
        n = 1;
    } else {
        n = SBLIMIT - 1;
    }
    
    ptr = g->sb_hybrid + 18;
    for(i = n;i > 0;i--) {
        int tmp0, tmp1, tmp2;
        csa = &csa_table[0][0];
#define INT_AA(j) \
            tmp0 = 4*(ptr[-1-j]);\
            tmp1 = 4*(ptr[   j]);\
            tmp2= MULH(tmp0 + tmp1, csa[0+4*j]);\
            ptr[-1-j] = tmp2 - MULH(tmp1, csa[2+4*j]);\
            ptr[   j] = tmp2 + MULH(tmp0, csa[3+4*j]);

        INT_AA(0)
        INT_AA(1)
        INT_AA(2)
        INT_AA(3)
        INT_AA(4)
        INT_AA(5)
        INT_AA(6)
        INT_AA(7)
            
        ptr += 18;       
    }
}
