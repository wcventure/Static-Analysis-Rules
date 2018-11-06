static int int_pow(int i, int *exp_ptr)
{
    int e, er, eq, j;
    int a, a1;
    
    /
    a = i;
    e = POW_FRAC_BITS;
    while (a < (1 << (POW_FRAC_BITS - 1))) {
        a = a << 1;
        e--;
    }
    a -= (1 << POW_FRAC_BITS);
    a1 = 0;
    for(j = DEV_ORDER - 1; j >= 0; j--)
        a1 = POW_MULL(a, dev_4_3_coefs[j] + a1);
    a = (1 << POW_FRAC_BITS) + a1;
    /
    e = e * 4;
    er = e % 3;
    eq = e / 3;
    a = POW_MULL(a, pow_mult3[er]);
    while (a >= 2 * POW_FRAC_ONE) {
        a = a >> 1;
        eq++;
    }
    /
    while (a < POW_FRAC_ONE) {
        a = a << 1;
        eq--;
    }
    /
#if (POW_FRAC_BITS - 1) > FRAC_BITS
    a = (a + (1 << (POW_FRAC_BITS - FRAC_BITS - 1))) >> (POW_FRAC_BITS - FRAC_BITS);
    /
    if (a >= 2 * (1 << FRAC_BITS)) {
        a = a >> 1;
        eq++;
    }
#endif
    *exp_ptr = eq;
    return a;
}
