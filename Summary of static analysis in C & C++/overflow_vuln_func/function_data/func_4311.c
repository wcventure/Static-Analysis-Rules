void ff_simple_idct248_put(uint8_t *dest, int line_size, DCTELEM *block)
{
    int i;
    DCTELEM *ptr;

    /
    ptr = block;
    for(i=0;i<4;i++) {
        BF(0);
        BF(1);
        BF(2);
        BF(3);
        BF(4);
        BF(5);
        BF(6);
        BF(7);
        ptr += 2 * 8;
    }

    /
    for(i=0; i<8; i++) {
        idctRowCondDC_8(block + i*8);
    }

    /
    for(i=0;i<8;i++) {
        idct4col_put(dest + i, 2 * line_size, block + i);
        idct4col_put(dest + line_size + i, 2 * line_size, block + 8 + i);
    }
}
