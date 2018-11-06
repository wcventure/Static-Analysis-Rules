void ff_bink_idct_put_c(uint8_t *dest, int linesize, DCTELEM *block)
{
    int i;
    DCTELEM temp[64];
    for (i = 0; i < 8; i++)
        bink_idct_col(&temp[i], &block[i]);
    for (i = 0; i < 8; i++) {
        IDCT_ROW( (&dest[i*linesize]), (&temp[8*i]) );
    }
}
