void tcg_func_start(TCGContext *s)
{
    tcg_pool_reset(s);
    s->nb_temps = s->nb_globals;

    /
    memset(s->free_temps, 0, sizeof(s->free_temps));

    s->nb_labels = 0;
    s->current_frame_offset = s->frame_start;

#ifdef CONFIG_DEBUG_TCG
    s->goto_tb_issue_mask = 0;
#endif

    s->gen_op_buf[0].next = 1;
    s->gen_op_buf[0].prev = 0;
    s->gen_next_op_idx = 1;
}
