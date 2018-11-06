void FUNC(ff_simple_idct_add)(uint8_t *dest_, int line_size, DCTELEM *block)
{
    pixel *dest = (pixel *)dest_;
    int i;

    line_size /= sizeof(pixel);

    for (i = 0; i < 8; i++)
        FUNC(idctRowCondDC)(block + i*8);

    for (i = 0; i < 8; i++)
        FUNC(idctSparseColAdd)(dest + i, line_size, block + i);
}
