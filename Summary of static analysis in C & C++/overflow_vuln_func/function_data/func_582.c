static void tput_make_elmtlist(struct gtk_graph *g)
{
    struct segment *tmp, *oldest;
    struct element *elements, *e;
    int    i, sum = 0;
    double dtime, tput;
    int    num_sack_ranges;

    if (g->elists->elements == NULL) {
        int n = 1 + get_num_dsegs(g) + get_num_acks(g, &num_sack_ranges);
        e = elements = (struct element * )g_malloc(n * sizeof(struct element));
    } else
        e = elements = g->elists->elements;

    for (oldest=g->tg.segments, tmp=g->tg.segments->next, i=0; tmp; tmp=tmp->next, i++) {
        double time_val = tmp->rel_secs + tmp->rel_usecs/1000000.0;
        dtime = time_val - (oldest->rel_secs + oldest->rel_usecs/1000000.0);
        if (i>g->s.tput.nsegs) {
            sum    -= oldest->th_seglen;
            oldest  = oldest->next;
        }
        sum  += tmp->th_seglen;
        tput  = sum / dtime;
        /

        e->type   = ELMT_ELLIPSE;
        e->parent = tmp;
        e->p.ellipse.dim.width  = g->s.tput.width;
        e->p.ellipse.dim.height = g->s.tput.height;
        e->p.ellipse.dim.x      = g->zoom.x*(time_val - g->bounds.x0) - g->s.tput.width/2.0;
        e->p.ellipse.dim.y      = g->zoom.y*tput + g->s.tput.height/2.0;
        e++;
    }
    e->type = ELMT_NONE;
    g->elists->elements = elements;
}
