av_cold void ff_h264_decode_init_vlc(void){
    static int done = 0;

    if (!done) {
        int i;
        int offset;
        done = 1;

        chroma_dc_coeff_token_vlc.table = chroma_dc_coeff_token_vlc_table;
        chroma_dc_coeff_token_vlc.table_allocated = chroma_dc_coeff_token_vlc_table_size;
        init_vlc(&chroma_dc_coeff_token_vlc, CHROMA_DC_COEFF_TOKEN_VLC_BITS, 4*5,
                 &chroma_dc_coeff_token_len [0], 1, 1,
                 &chroma_dc_coeff_token_bits[0], 1, 1,
                 INIT_VLC_USE_NEW_STATIC);

        chroma422_dc_coeff_token_vlc.table = chroma422_dc_coeff_token_vlc_table;
        chroma422_dc_coeff_token_vlc.table_allocated = chroma422_dc_coeff_token_vlc_table_size;
        init_vlc(&chroma422_dc_coeff_token_vlc, CHROMA422_DC_COEFF_TOKEN_VLC_BITS, 4*9,
                 &chroma422_dc_coeff_token_len [0], 1, 1,
                 &chroma422_dc_coeff_token_bits[0], 1, 1,
                 INIT_VLC_USE_NEW_STATIC);

        offset = 0;
        for(i=0; i<4; i++){
            coeff_token_vlc[i].table = coeff_token_vlc_tables+offset;
            coeff_token_vlc[i].table_allocated = coeff_token_vlc_tables_size[i];
            init_vlc(&coeff_token_vlc[i], COEFF_TOKEN_VLC_BITS, 4*17,
                     &coeff_token_len [i][0], 1, 1,
                     &coeff_token_bits[i][0], 1, 1,
                     INIT_VLC_USE_NEW_STATIC);
            offset += coeff_token_vlc_tables_size[i];
        }
        /
        av_assert0(offset == FF_ARRAY_ELEMS(coeff_token_vlc_tables));

        for(i=0; i<3; i++){
            chroma_dc_total_zeros_vlc[i].table = chroma_dc_total_zeros_vlc_tables[i];
            chroma_dc_total_zeros_vlc[i].table_allocated = chroma_dc_total_zeros_vlc_tables_size;
            init_vlc(&chroma_dc_total_zeros_vlc[i],
                     CHROMA_DC_TOTAL_ZEROS_VLC_BITS, 4,
                     &chroma_dc_total_zeros_len [i][0], 1, 1,
                     &chroma_dc_total_zeros_bits[i][0], 1, 1,
                     INIT_VLC_USE_NEW_STATIC);
        }

        for(i=0; i<7; i++){
            chroma422_dc_total_zeros_vlc[i].table = chroma422_dc_total_zeros_vlc_tables[i];
            chroma422_dc_total_zeros_vlc[i].table_allocated = chroma422_dc_total_zeros_vlc_tables_size;
            init_vlc(&chroma422_dc_total_zeros_vlc[i],
                     CHROMA422_DC_TOTAL_ZEROS_VLC_BITS, 8,
                     &chroma422_dc_total_zeros_len [i][0], 1, 1,
                     &chroma422_dc_total_zeros_bits[i][0], 1, 1,
                     INIT_VLC_USE_NEW_STATIC);
        }

        for(i=0; i<15; i++){
            total_zeros_vlc[i].table = total_zeros_vlc_tables[i];
            total_zeros_vlc[i].table_allocated = total_zeros_vlc_tables_size;
            init_vlc(&total_zeros_vlc[i],
                     TOTAL_ZEROS_VLC_BITS, 16,
                     &total_zeros_len [i][0], 1, 1,
                     &total_zeros_bits[i][0], 1, 1,
                     INIT_VLC_USE_NEW_STATIC);
        }

        for(i=0; i<6; i++){
            run_vlc[i].table = run_vlc_tables[i];
            run_vlc[i].table_allocated = run_vlc_tables_size;
            init_vlc(&run_vlc[i],
                     RUN_VLC_BITS, 7,
                     &run_len [i][0], 1, 1,
                     &run_bits[i][0], 1, 1,
                     INIT_VLC_USE_NEW_STATIC);
        }
        run7_vlc.table = run7_vlc_table,
        run7_vlc.table_allocated = run7_vlc_table_size;
        init_vlc(&run7_vlc, RUN7_VLC_BITS, 16,
                 &run_len [6][0], 1, 1,
                 &run_bits[6][0], 1, 1,
                 INIT_VLC_USE_NEW_STATIC);

        init_cavlc_level_tab();
    }
}
