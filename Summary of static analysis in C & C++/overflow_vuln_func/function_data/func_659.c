static void
iostat_init(const char *optarg, void* userdata _U_)
{
    gdouble interval_float;
    guint32 idx=0, i;
    io_stat_t *io;
    const gchar *filters, *str, *pos;

    if ((*(optarg+(strlen(optarg)-1)) == ',') ||
        (sscanf(optarg, "io,stat,%lf%n", &interval_float, (int *)&idx) != 1) ||
        (idx < 8)) {
        fprintf(stderr, "\ntshark: invalid \"-z io,stat,<interval>[,<filter>][,<filter>]...\" argument\n");
        exit(1);
    }

    filters=optarg+idx;
    if (*filters) {
        if (*filters != ',') {
            /
            --filters;
            if (*filters != ',') {
                fprintf(stderr, "\ntshark: invalid \"-z io,stat,<interval>[,<filter>][,<filter>]...\" argument\n");
                exit(1);
            }
        }
    } else
        filters=NULL;

    switch (timestamp_get_type()) {
    case TS_DELTA:
    case TS_DELTA_DIS:
    case TS_EPOCH:
    case TS_UTC:
    case TS_UTC_WITH_DATE:
        fprintf(stderr, "\ntshark: invalid -t operand. io,stat only supports -t <r|a|ad>\n");
        exit(1);
    default:
        break;
    }

    io = (io_stat_t *) g_malloc(sizeof(io_stat_t));

    /
    if (interval_float==0) {
        io->interval = G_MAXINT32;
        io->invl_prec = 0;
    } else {
        /
        io->interval = (gint64)(interval_float*1000000.0+0.5);
        /
        io->invl_prec = 6;
        for (i=10; i<10000000; i*=10) {
            if (io->interval%i > 0)
                break;
            io->invl_prec--;
        }
    }
    if (io->interval < 1){
        fprintf(stderr,
            "\ntshark: \"-z\" interval must be >=0.000001 seconds or \"0\" for the entire capture duration.\n");
        exit(10);
    }

    /
    io->num_cols = 1;
    io->start_time=0;

    if (filters && (*filters != '\0')) {
        str = filters;
        while((str = strchr(str, ','))) {
            io->num_cols++;
            str++;
        }
    }

    io->items = (io_stat_item_t *) g_malloc(sizeof(io_stat_item_t) * io->num_cols);
    io->filters = (const char **)g_malloc(sizeof(char *) * io->num_cols);
    io->max_vals = (guint64 *) g_malloc(sizeof(guint64) * io->num_cols);
    io->max_frame = (guint32 *) g_malloc(sizeof(guint32) * io->num_cols);

    for (i=0; i<io->num_cols; i++) {
        io->max_vals[i] = 0;
        io->max_frame[i] = 0;
    }

    /
    if((!filters) || (filters[0]==0)) {
        register_io_tap(io, 0, NULL);
    } else {
        gchar *filter;
        i = 0;
        str = filters;
        do {
            pos = (gchar*) strchr(str, ',');
            if(pos==str){
                register_io_tap(io, i, NULL);
            } else if (pos==NULL) {
                str = (const char*) g_strstrip((gchar*)str);
                filter = g_strdup((gchar*) str);
                if (*filter)
                    register_io_tap(io, i, filter);
                else
                    register_io_tap(io, i, NULL);
            } else {
                filter = (gchar *)g_malloc((pos-str)+1);
                g_strlcpy( filter, str, (gsize) ((pos-str)+1));
                filter = g_strstrip(filter);
                register_io_tap(io, i, (char *) filter);
            }
            str = pos+1;
            i++;
        } while(pos);
    }
}
