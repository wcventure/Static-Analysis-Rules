void FUNC(ff_simple_idct)(DCTELEM *block)
{
    int i;

    for (i = 0; i < 8; i++)
        FUNC(idctRowCondDC)(block + i*8);

    for (i = 0; i < 8; i++)
        FUNC(idctSparseCol)(block + i);
}
