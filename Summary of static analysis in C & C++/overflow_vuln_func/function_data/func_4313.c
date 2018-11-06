void ff_prores_idct(DCTELEM *block, const int16_t *qmat)
{
    int i;

    for (i = 0; i < 64; i++)
        block[i] *= qmat[i];

    for (i = 0; i < 8; i++)
        idctRowCondDC_10(block + i*8);

    for (i = 0; i < 64; i++)
        block[i] >>= 2;

    for (i = 0; i < 8; i++)
        idctSparseCol_10(block + i);
}
