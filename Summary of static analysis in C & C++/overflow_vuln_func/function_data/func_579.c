static int get_num_acks(struct gtk_graph *g, int *num_sack_ranges)
{
    int count;
    struct segment *tmp;

    for (tmp = g->tg.segments, count=0; tmp; tmp = tmp->next) {
        if (!compare_headers(&g->tg.src_address, &g->tg.dst_address,
                             g->tg.src_port, g->tg.dst_port,
                             &tmp->ip_src, &tmp->ip_dst,
                             tmp->th_sport, tmp->th_dport,
                             COMPARE_CURR_DIR)) {
            count++;
            *num_sack_ranges += tmp->num_sack_ranges;
        }
    }
    return count;
}
