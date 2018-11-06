static int read_old_huffman_tables(HYuvContext *s){
#if 1
    GetBitContext gb;
    int i;

    init_get_bits(&gb, classic_shift_luma, sizeof(classic_shift_luma)*8);
    read_len_table(s->len[0], &gb);
    init_get_bits(&gb, classic_shift_chroma, sizeof(classic_shift_chroma)*8);
    read_len_table(s->len[1], &gb);

    for(i=0; i<256; i++) s->bits[0][i] = classic_add_luma  [i];
    for(i=0; i<256; i++) s->bits[1][i] = classic_add_chroma[i];

    if(s->bitstream_bpp >= 24){
        memcpy(s->bits[1], s->bits[0], 256*sizeof(uint32_t));
        memcpy(s->len[1] , s->len [0], 256*sizeof(uint8_t));
    }
    memcpy(s->bits[2], s->bits[1], 256*sizeof(uint32_t));
    memcpy(s->len[2] , s->len [1], 256*sizeof(uint8_t));

    for(i=0; i<3; i++){
        free_vlc(&s->vlc[i]);
        init_vlc(&s->vlc[i], VLC_BITS, 256, s->len[i], 1, 1, s->bits[i], 4, 4, 0);
    }

    generate_joint_tables(s);

    return 0;
#else
    av_log(s->avctx, AV_LOG_DEBUG, "v1 huffyuv is not supported \n");
    return -1;
#endif
}
