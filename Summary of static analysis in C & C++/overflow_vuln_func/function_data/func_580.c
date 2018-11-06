static void tseq_stevens_make_elmtlist(struct gtk_graph *g)
{
    struct segment *tmp;
    struct element *elements, *e;
    double  xx0      = g->bounds.x0, yy0 = g->bounds.y0;
    guint32 seq_base = (guint32) yy0;
    guint32 seq_cur;

    debug(DBS_FENTRY) puts("tseq_stevens_make_elmtlist()");
    if (g->elists->elements == NULL) {
        int n = 1 + get_num_dsegs(g);
        e = elements = (struct element * )g_malloc(n*sizeof(struct element));
    } else
        e = elements = g->elists->elements;

    for (tmp = g->tg.segments; tmp; tmp = tmp->next) {
        double secs, seqno;

        if (!compare_headers(&g->tg.src_address, &g->tg.dst_address,
                             g->tg.src_port, g->tg.dst_port,
                             &tmp->ip_src, &tmp->ip_dst,
                             tmp->th_sport, tmp->th_dport,
                             COMPARE_CURR_DIR)) {
            continue;
        }
        /
        seq_cur = tmp->th_seq - seq_base;
        secs    = g->zoom.x * (tmp->rel_secs + tmp->rel_usecs / 1000000.0 - xx0);
        seqno   = g->zoom.y * seq_cur;

        e->type   = ELMT_ELLIPSE;
        e->parent = tmp;
        e->p.ellipse.dim.width  = g->s.tseq_stevens.seq_width;
        e->p.ellipse.dim.height = g->s.tseq_stevens.seq_height;
        e->p.ellipse.dim.x      = secs - g->s.tseq_stevens.seq_width/2.0;
        e->p.ellipse.dim.y      = seqno + g->s.tseq_stevens.seq_height/2.0;
        e++;
    }
    e->type = ELMT_NONE;
    g->elists->elements = elements;
}
