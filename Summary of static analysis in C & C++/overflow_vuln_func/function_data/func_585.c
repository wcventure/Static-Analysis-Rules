static void rtt_make_elmtlist(struct gtk_graph *g)
{
    struct segment *tmp;
    struct unack   *unack    = NULL, *u;
    struct element *elements, *e;
    guint32         seq_base = (guint32) g->bounds.x0;

    debug(DBS_FENTRY) puts("rtt_make_elmtlist()");

    if (g->elists->elements == NULL) {
        int n = 1 + get_num_dsegs(g);
        e = elements = (struct element * )g_malloc(n * sizeof(struct element));
    } else {
        e = elements = g->elists->elements;
    }

    for (tmp=g->tg.segments; tmp; tmp=tmp->next) {
        if (compare_headers(&g->tg.src_address, &g->tg.dst_address,
                            g->tg.src_port, g->tg.dst_port,
                            &tmp->ip_src, &tmp->ip_dst,
                            tmp->th_sport, tmp->th_dport,
                            COMPARE_CURR_DIR)) {
            guint32 seqno = tmp->th_seq -seq_base;

            if (tmp->th_seglen && !rtt_is_retrans(unack, seqno)) {
                double time_val = tmp->rel_secs + tmp->rel_usecs / 1000000.0;
                u = rtt_get_new_unack(time_val, seqno);
                if (!u) return;
                rtt_put_unack_on_list(&unack, u);
            }
        } else {
            guint32       ackno    = tmp->th_ack -seq_base;
            double        time_val = tmp->rel_secs + tmp->rel_usecs / 1000000.0;
            struct unack *v;

            for (u=unack; u; u=v) {
                if (ackno > u->seqno) {
                    double rtt = time_val - u->time;

                    e->type   = ELMT_ELLIPSE;
                    e->parent = tmp;
                    e->p.ellipse.dim.width  = g->s.rtt.width;
                    e->p.ellipse.dim.height = g->s.rtt.height;
                    e->p.ellipse.dim.x      = g->zoom.x * u->seqno - g->s.rtt.width/2.0;
                    e->p.ellipse.dim.y      = g->zoom.y * rtt + g->s.rtt.height/2.0;
                    e++;

                    v = u->next;
                    rtt_delete_unack_from_list(&unack, u);
                } else
                    v = u->next;
            }
        }
    }
    e->type = ELMT_NONE;
    g->elists->elements = elements;
}
