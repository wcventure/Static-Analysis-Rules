static void wscale_make_elmtlist(struct gtk_graph *g)
{
    struct segment *segm     = NULL;
    struct element *elements = NULL;
    struct element *e        = NULL;
    gdouble         sec_base = -1.0;

    debug(DBS_FENTRY) puts("wscale_make_elmtlist()");

    /
    if (g->elists->elements == NULL)
    {
        int n = 1 + get_num_dsegs(g);
        e = elements = (struct element*)g_malloc(n*sizeof(struct element));
    }
    else
        e = elements = g->elists->elements;


    for ( segm=g->tg.segments; segm; segm=segm->next ) {
        if (compare_headers(&g->tg.src_address, &g->tg.dst_address,
                            g->tg.src_port, g->tg.dst_port,
                            &segm->ip_src, &segm->ip_dst,
                            segm->th_sport, segm->th_dport,
                            COMPARE_CURR_DIR))
        {
            gdouble sec   = segm->rel_secs + (segm->rel_usecs / 1000000.0);
            guint16 flags = segm->th_flags;
            guint32 wsize = segm->th_win;

            /
            if ((sec_base < 0) && (sec > 0))
                sec_base = sec;

            /
            if ( (flags & (TH_SYN|TH_RST)) == 0 )
            {
                e->type   = ELMT_ELLIPSE;
                e->parent = segm;
                e->p.ellipse.dim.width  = g->s.wscale.win_width;
                e->p.ellipse.dim.height = g->s.wscale.win_height;
                e->p.ellipse.dim.x      = g->zoom.x * (sec - sec_base) - g->s.wscale.win_width / 2.0;
                e->p.ellipse.dim.y      = g->zoom.y * wsize - g->s.wscale.win_height / 2.0;
                e++;
            }
        }
    }
    /
    e->type = ELMT_NONE;
    g->elists->elements = elements;
}
