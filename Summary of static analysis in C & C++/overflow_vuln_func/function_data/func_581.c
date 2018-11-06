static void tseq_tcptrace_make_elmtlist(struct gtk_graph *g)
{
    struct segment *tmp;
    struct element *elements0, *e0;     /
    struct element *elements1, *e1;     /
    double   xx0, yy0;
    double   p_t             = 0; /
    double   p_ackno         = 0, p_win = 0;
    gboolean ack_seen        = FALSE;
    int      toggle          = 0;
    guint32  seq_base;
    guint32  seq_cur;
    int      num_sack_ranges = 0;

    debug(DBS_FENTRY) puts("tseq_tcptrace_make_elmtlist()");

    if (g->elists->elements == NULL ) {
        /
        int n = 1 + 3*get_num_dsegs(g);
        e0 = elements0 = (struct element * )g_malloc(n * sizeof(struct element));
    } else {
        /
        e0 = elements0 = g->elists->elements;
    }

    if (g->elists->next->elements == NULL) {
        /
        int n = 1 + 4*get_num_acks(g, &num_sack_ranges);
        n  += num_sack_ranges;
        e1  = elements1 = (struct element * )g_malloc(n * sizeof(struct element));
    } else {
        /
        e1 = elements1 = g->elists->next->elements;
    }

    xx0      = g->bounds.x0;
    yy0      = g->bounds.y0;
    seq_base = (guint32) yy0;

    for (tmp = g->tg.segments; tmp; tmp = tmp->next) {
        double secs, data;
        double x;

        secs = tmp->rel_secs + tmp->rel_usecs / 1000000.0;
        x  = secs - xx0;
        x *= g->zoom.x;
        if (compare_headers(&g->tg.src_address, &g->tg.dst_address,
                            g->tg.src_port, g->tg.dst_port,
                            &tmp->ip_src, &tmp->ip_dst,
                            tmp->th_sport, tmp->th_dport,
                            COMPARE_CURR_DIR)) {
            /
            double yy1, yy2;

            seq_cur = tmp->th_seq - seq_base;
            if (TCP_SYN(tmp->th_flags) || TCP_FIN(tmp->th_flags))
                data = 1;
            else
                data = tmp->th_seglen;

            yy1 = g->zoom.y * (seq_cur);
            yy2 = g->zoom.y * (seq_cur + data);
            e0->type   = ELMT_LINE;
            e0->parent = tmp;
            /
            e0->elment_color_p = &g->s.tseq_tcptrace.seq_color;
            e0->p.line.dim.x1 = e0->p.line.dim.x2 = x;
            e0->p.line.dim.y1 = yy1;
            e0->p.line.dim.y2 = yy2;
            e0++;
            e0->type   = ELMT_LINE;
            e0->parent = tmp;
            /
            e0->elment_color_p = &g->s.tseq_tcptrace.seq_color;
            e0->p.line.dim.x1 = x - 1;
            e0->p.line.dim.x2 = x + 1;
            e0->p.line.dim.y1 = e0->p.line.dim.y2 = yy1;
            e0++;
            e0->type   = ELMT_LINE;
            e0->parent = tmp;
            /
            e0->elment_color_p = &g->s.tseq_tcptrace.seq_color;
            e0->p.line.dim.x1 = x + 1;
            e0->p.line.dim.x2 = x - 1;
            e0->p.line.dim.y1 = e0->p.line.dim.y2 = yy2;
            e0++;
        } else {
            double ackno, win;
            if (! TCP_ACK(tmp->th_flags))
                /
                continue;
            /
            seq_cur = tmp->th_ack - seq_base;
            ackno   = seq_cur * g->zoom.y;
            win     = tmp->th_win * g->zoom.y;

            /
            if (ack_seen == TRUE) { /

                /
                e1->type   = ELMT_LINE;
                e1->parent = tmp;
                /
                e1->elment_color_p = &g->s.tseq_tcptrace.ack_color[toggle];
                e1->p.line.dim.x1 = p_t;
                e1->p.line.dim.y1 = p_ackno;
                e1->p.line.dim.x2 = x;
                e1->p.line.dim.y2 = p_ackno;
                e1++;

                /
                e1->type   = ELMT_LINE;
                e1->parent = tmp;
                /
                e1->elment_color_p = &g->s.tseq_tcptrace.ack_color[toggle];
                e1->p.line.dim.x1 = x;
                e1->p.line.dim.y1 = p_ackno;
                e1->p.line.dim.x2 = x;
                e1->p.line.dim.y2 = ((ackno != p_ackno) || (ackno < 4)) ? ackno : ackno - 4;
                e1++;

                /
                e1->type   = ELMT_LINE;
                e1->parent = tmp;
                /
                e1->elment_color_p = &g->s.tseq_tcptrace.ack_color[toggle];
                e1->p.line.dim.x1 = p_t;
                e1->p.line.dim.y1 = p_win + p_ackno;
                e1->p.line.dim.x2 = x;
                e1->p.line.dim.y2 = p_win + p_ackno;
                e1++;

                /
                e1->type   = ELMT_LINE;
                e1->parent = tmp;
                /
                e1->elment_color_p = &g->s.tseq_tcptrace.ack_color[toggle];
                e1->p.line.dim.x1 = x;
                e1->p.line.dim.y1 = p_win + p_ackno;
                e1->p.line.dim.x2 = x;
                e1->p.line.dim.y2 = win + ackno;
                e1++;

                /
                toggle = 1^toggle;
            }
            ack_seen = TRUE;
            p_ackno  = ackno;
            p_win    = win;
            p_t      = x;

            /
            if (tmp->num_sack_ranges) {
                int n;

                for (n=0; n < tmp->num_sack_ranges; n++) {
                    double left_edge  = (tmp->sack_left_edge[n]  - seq_base) * g->zoom.y;
                    double right_edge = (tmp->sack_right_edge[n] - seq_base) * g->zoom.y;

                    /
                    e1->type   = ELMT_LINE;
                    e1->parent = tmp;
                    /
                    e1->elment_color_p = (n==0) ? &g->s.tseq_tcptrace.sack_color[0] :
                                                  &g->s.tseq_tcptrace.sack_color[1];
                    e1->p.line.dim.x1 = x;
                    e1->p.line.dim.y1 = right_edge;
                    e1->p.line.dim.x2 = x;
                    e1->p.line.dim.y2 = left_edge;
                    e1++;
                }
            }
        }
    }

    /
    e0->type = ELMT_NONE;
    e1->type = ELMT_NONE;

    g->elists->elements = elements0;
    g->elists->next->elements = elements1;
    g->elists->next->next     = NULL;
}
