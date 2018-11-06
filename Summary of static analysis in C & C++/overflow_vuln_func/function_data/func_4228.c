static void backup_duplicate_context(MpegEncContext *bak, MpegEncContext *src){
#define COPY(a) bak->a= src->a
    COPY(allocated_edge_emu_buffer);
    COPY(edge_emu_buffer);
    COPY(me.scratchpad);
    COPY(me.temp);
    COPY(rd_scratchpad);
    COPY(b_scratchpad);
    COPY(obmc_scratchpad);
    COPY(me.map);
    COPY(me.score_map);
    COPY(blocks);
    COPY(block);
    COPY(start_mb_y);
    COPY(end_mb_y);
    COPY(me.map_generation);
    COPY(pb);
    COPY(dct_error_sum);
    COPY(dct_count[0]);
    COPY(dct_count[1]);
    COPY(ac_val_base);
    COPY(ac_val[0]);
    COPY(ac_val[1]);
    COPY(ac_val[2]);
#undef COPY
}
