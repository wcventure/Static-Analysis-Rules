static void tput_initialize(struct gtk_graph *g)
{
    struct segment *tmp, *oldest/;
    int    i, sum = 0;
    double dtime, tput, tputmax = 0;
    double t, t0, tmax = 0, yy0, ymax;

    debug(DBS_FENTRY) puts("tput_initialize()");

    tput_read_config(g);

#if 0
    for (last=g->tg.segments; last->next; last=last->next);   /
#endif
    for (oldest=g->tg.segments, tmp=g->tg.segments->next, i=0; tmp; tmp=tmp->next, i++) {
        dtime = tmp->rel_secs + tmp->rel_usecs/1000000.0 -
            (oldest->rel_secs + oldest->rel_usecs/1000000.0);
        if (i > g->s.tput.nsegs) {
            sum    -= oldest->th_seglen;
            oldest  = oldest->next;
        }
        sum  += tmp->th_seglen;
        tput  = sum / dtime;
        debug(DBS_TPUT_ELMTS) printf("tput=%f\n", tput);
        if (tput > tputmax)
            tputmax = tput;
        t = tmp->rel_secs + tmp->rel_usecs / 1000000.0;
        if (t > tmax)
            tmax = t;
    }

    t0   = g->tg.segments->rel_secs + g->tg.segments->rel_usecs / 1000000.0;
    yy0  = 0;
    ymax = tputmax;

    g->bounds.x0 = t0;
    g->bounds.y0 = yy0;
    g->bounds.width  = tmax - t0;
    g->bounds.height = ymax - yy0;
    g->zoom.x = (g->geom.width - 1) / g->bounds.width;
    g->zoom.y = (g->geom.height -1) / g->bounds.height;
}
