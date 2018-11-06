static void read_len_table(uint8_t *dst, GetBitContext *gb){
    int i, val, repeat;

    for(i=0; i<256;){
        repeat= get_bits(gb, 3);
        val   = get_bits(gb, 5);
        if(repeat==0)
            repeat= get_bits(gb, 8);
//printf("%d %d\n", val, repeat);
        while (repeat--)
            dst[i++] = val;
    }
}
