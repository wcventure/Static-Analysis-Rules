static void tput_read_config(struct gtk_graph *g)
{
    debug(DBS_FENTRY) puts("tput_read_config()");

    g->s.tput.width  = 4;
    g->s.tput.height = 4;
    g->s.tput.nsegs  = 20;

    g->title = (const char ** )g_malloc(2 * sizeof(char *));
    g->title[0] = "Throughput Graph";
    g->title[1] = NULL;
    g->y_axis->label    = (const char ** )g_malloc(3 * sizeof(char * ));
    g->y_axis->label[0] = "[B/s]";
    g->y_axis->label[1] = "Throughput";
    g->y_axis->label[2] = NULL;
    g->x_axis->label    = (const char ** )g_malloc(2 * sizeof(char * ));
    g->x_axis->label[0] = "Time[s]";
    g->x_axis->label[1] = NULL;
    g->s.tput.flags     = 0;
}
