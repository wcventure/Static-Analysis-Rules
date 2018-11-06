void ff_eval_coefs(int *coefs, const int *refl)
{
    int buffer[LPC_ORDER];
    int *b1 = buffer;
    int *b2 = coefs;
    int i, j;

    for (i=0; i < LPC_ORDER; i++) {
        b1[i] = refl[i] * 16;

        for (j=0; j < i; j++)
            b1[j] = ((refl[i] * b2[i-j-1]) >> 12) + b2[j];

        FFSWAP(int *, b1, b2);
    }

    for (i=0; i < LPC_ORDER; i++)
        coefs[i] >>= 4;
}
