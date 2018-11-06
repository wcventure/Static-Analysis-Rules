static int read_huffman_tables(HYuvContext *s, uint8_t *src, int length){
    GetBitContext gb;
    int i;

    init_get_bits(&gb, src, length*8);

    for(i=0; i<3; i++){
        read_len_table(s->len[i], &gb);

        if(generate_bits_table(s->bits[i], s->len[i])<0){
            return -1;
        }
#if 0
for(j=0; j<256; j++){
printf("%6X, %2d,  %3d\n", s->bits[i][j], s->len[i][j], j);
}
#endif
        free_vlc(&s->vlc[i]);
        init_vlc(&s->vlc[i], VLC_BITS, 256, s->len[i], 1, 1, s->bits[i], 4, 4, 0);
    }

    generate_joint_tables(s);

    return (get_bits_count(&gb)+7)/8;
}
