static void
iostat_draw(void *arg)
{
    guint32 num;
    guint64 interval, duration, t, invl_end;
    int i, j, k, num_cols, num_rows, dv, dur_secs, dur_mag, invl_mag, invl_prec, tabrow_w,
        borderlen, invl_col_w, numpad=1, namelen, len_filt, type, maxfltr_w, ftype;
    int fr_mag;    /
    int val_mag;   /
    char *spaces, *spaces_s, *filler_s=NULL, **fmts, *fmt=NULL;
    const char *filter;
    static gchar dur_mag_s[3], invl_mag_s[3], invl_prec_s[3], fr_mag_s[3], val_mag_s[3], *invl_fmt, *full_fmt;
    io_stat_item_t *mit, **stat_cols, *item, **item_in_column;
    gboolean last_row=FALSE;
    io_stat_t *iot;
    column_width *col_w;
    struct tm * tm_time;
    time_t the_time;

    mit = (io_stat_item_t *)arg;
    iot = mit->parent;
    num_cols = iot->num_cols;
    col_w = (column_width *)g_malloc(sizeof(column_width) * num_cols);
    fmts = (char **)g_malloc(sizeof(char *) * num_cols);
    duration = (guint64)((cfile.elapsed_time.secs*1000000) + ((cfile.elapsed_time.nsecs+500)/1000));

    /
    stat_cols = (io_stat_item_t **) g_malloc(sizeof(io_stat_item_t *) * num_cols);
    for (j=0; j<num_cols; j++)
        stat_cols[j] = &iot->items[j];

    /
    if (iot->interval > duration || iot->interval==G_MAXINT32) {
        interval = duration;
        iot->interval = G_MAXINT32;
    } else {
        interval = iot->interval;
    }

    /
    dur_secs = (int)duration/1000000;
    dur_mag = magnitude((guint64)dur_secs, 5);
    g_snprintf(dur_mag_s, 3, "%u", dur_mag);

    /
    invl_mag = magnitude((guint64)interval/1000000, 5);

    /
    if (interval==duration) {
        /
        if (dur_mag >= 2)
            invl_prec = 1;
        else if (dur_mag==1)
            invl_prec = 3;
        else
            invl_prec = 6;

        borderlen = 30 + dur_mag + (invl_prec==0 ? 0 : invl_prec+1);
    } else {
        invl_prec = iot->invl_prec;
        borderlen = 24 + invl_mag + (invl_prec==0 ? 0 : invl_prec+1);
    }

    /
    dv=1000000;
    for (i=0; i<invl_prec; i++)
        dv /= 10;
    duration = duration + (5*(dv/10));
    if (iot->interval==G_MAXINT32)
        interval = duration;

    /
    dur_secs = (int)duration/1000000;
    dur_mag = magnitude((guint64)dur_secs, 5);

    /
    if (invl_prec==0)
        invl_col_w = (2*dur_mag) + 8;
    else
        invl_col_w = (2*dur_mag) + (2*invl_prec) + 10;

    /
    if (timestamp_get_type()==TS_ABSOLUTE_WITH_DATE)
        invl_col_w = MAX(invl_col_w, 23);
    else
        invl_col_w = MAX(invl_col_w, 12);

    borderlen = MAX(borderlen, invl_col_w);

    /
    tabrow_w = invl_col_w;
    for (j=0; j<num_cols; j++) {
        type = iot->items[j].calc_type;
        if (type==CALC_TYPE_FRAMES_AND_BYTES) {
            namelen = 5;
        } else {
            namelen = (int) strlen(calc_type_table[type].func_name);
        }
        if(type==CALC_TYPE_FRAMES
        || type==CALC_TYPE_FRAMES_AND_BYTES) {

            fr_mag = magnitude(iot->max_frame[j], 15);
            fr_mag = MAX(6, fr_mag);
            col_w[j].fr = fr_mag;
            tabrow_w += col_w[j].fr + 3;
            g_snprintf(fr_mag_s, 3, "%u", fr_mag);

            if (type==CALC_TYPE_FRAMES) {
                fmt = g_strconcat(" %", fr_mag_s, "u |", NULL);
            } else {
                /
                val_mag = magnitude(iot->max_vals[j], 15);
                val_mag = MAX(5, val_mag);
                col_w[j].val = val_mag;
                tabrow_w += (col_w[j].val + 3);
                g_snprintf(val_mag_s, 3, "%u", val_mag);
                fmt = g_strconcat(" %", fr_mag_s, "u |", " %", val_mag_s, G_GINT64_MODIFIER, "u |", NULL);
            }
            if (fmt)
                fmts[j] = fmt;
            continue;
        }
        switch(type) {
        case CALC_TYPE_BYTES:
        case CALC_TYPE_COUNT:

            val_mag = magnitude(iot->max_vals[j], 15);
            val_mag = MAX(5, val_mag);
            col_w[j].val = val_mag;
            g_snprintf(val_mag_s, 3, "%u", val_mag);
            fmt = g_strconcat(" %", val_mag_s, G_GINT64_MODIFIER, "u |", NULL);
            break;

        default:
            ftype = proto_registrar_get_ftype(stat_cols[j]->hf_index);
            switch (ftype) {
                case FT_FLOAT:
                case FT_DOUBLE:
                    val_mag = magnitude(iot->max_vals[j], 15);
                    g_snprintf(val_mag_s, 3, "%u", val_mag);
                    fmt = g_strconcat(" %", val_mag_s, ".6f |", NULL);
                    col_w[j].val = val_mag + 7;
                    break;
                case FT_RELATIVE_TIME:
                    /
                    if (type==CALC_TYPE_LOAD) {
                        iot->max_vals[j] /= interval;
                    } else if (type != CALC_TYPE_AVG) {
                        iot->max_vals[j] = (iot->max_vals[j] + 500000000) / NANOSECS_PER_SEC;
                    }
                    val_mag = magnitude(iot->max_vals[j], 15);
                    g_snprintf(val_mag_s, 3, "%u", val_mag);
                    fmt = g_strconcat(" %", val_mag_s, "u.%06u |", NULL);
                    col_w[j].val = val_mag + 7;
                   break;

                default:
                    val_mag = magnitude(iot->max_vals[j], 15);
                    val_mag = MAX(namelen, val_mag);
                    col_w[j].val = val_mag;
                    g_snprintf(val_mag_s, 3, "%u", val_mag);

                    switch (ftype) {
                    case FT_UINT8:
                    case FT_UINT16:
                    case FT_UINT24:
                    case FT_UINT32:
                    case FT_UINT64:
                        fmt = g_strconcat(" %", val_mag_s, G_GINT64_MODIFIER, "u |", NULL);
                        break;
                    case FT_INT8:
                    case FT_INT16:
                    case FT_INT24:
                    case FT_INT32:
                    case FT_INT64:
                        fmt = g_strconcat(" %", val_mag_s, G_GINT64_MODIFIER, "d |", NULL);
                        break;
                    }
            } /
        } /
        tabrow_w += col_w[j].val + 3;
        if (fmt)
            fmts[j] = fmt;
    } /

    borderlen = MAX(borderlen, tabrow_w);

    /
    maxfltr_w = 0;
    for(j=0; j<num_cols; j++) {
        if (iot->filters[j]) {
            k = (int) (strlen(iot->filters[j]) + 11);
            maxfltr_w = MAX(maxfltr_w, k);
        } else {
            maxfltr_w = MAX(maxfltr_w, 26);
        }
    }
    /
    if (maxfltr_w > borderlen && borderlen < 102)
            borderlen = MIN(maxfltr_w, 102);

    /
    if (borderlen-tabrow_w==1)
        borderlen++;

    /
    printf("\n");
    for (i=0; i<borderlen; i++)
        printf("=");

    spaces = (char*) g_malloc(borderlen+1);
    for (i=0; i<borderlen; i++)
        spaces[i] = ' ';
    spaces[borderlen] = '\0';

    spaces_s = &spaces[16];
    printf("\n| IO Statistics%s|\n", spaces_s);
    spaces_s = &spaces[2];
    printf("|%s|\n", spaces_s);

    g_snprintf(invl_mag_s, 3, "%u", invl_mag);
    if (invl_prec > 0) {
        g_snprintf(invl_prec_s, 3, "%u", invl_prec);
        invl_fmt = g_strconcat("%", invl_mag_s, "u.%0", invl_prec_s, "u", NULL);
        if (interval==duration) {
            full_fmt = g_strconcat("| Interval size: ", invl_fmt, " secs (dur)%s", NULL);
            spaces_s = &spaces[30+invl_mag+invl_prec];
        } else {
            full_fmt = g_strconcat("| Interval size: ", invl_fmt, " secs%s", NULL);
            spaces_s = &spaces[24+invl_mag+invl_prec];
        }
        printf(full_fmt, (guint32)interval/1000000,
                            (guint32)((interval%1000000)/dv), spaces_s);
    } else {
        invl_fmt = g_strconcat("%", invl_mag_s, "u", NULL);
        full_fmt = g_strconcat("| Interval size: ", invl_fmt, " secs%s", NULL);
        spaces_s = &spaces[23 + invl_mag];
        printf(full_fmt, (guint32)interval/1000000, spaces_s);
    }
    g_free(invl_fmt);
    g_free(full_fmt);

    if (invl_prec > 0)
        invl_fmt = g_strconcat("%", dur_mag_s, "u.%0", invl_prec_s, "u", NULL);
    else
        invl_fmt = g_strconcat("%", dur_mag_s, "u", NULL);

    /
    printf("|\n| Col");
    for(j=0; j<num_cols; j++){
        printf((j==0 ? "%2u: " : "|    %2u: "), j+1);
        if (!iot->filters[j] || (iot->filters[j]==0)) {
            /
            spaces_s = &spaces[16 + 10];
            printf("Frames and bytes%s|\n", spaces_s);
        } else {
            filter = iot->filters[j];
            len_filt = (int) strlen(filter);

            /
            if (len_filt+11 <= borderlen) {
                printf("%s", filter);
                if (len_filt+11 <= borderlen) {
                    spaces_s = &spaces[len_filt + 10];
                    printf("%s", spaces_s);
                }
                printf("|\n");
            } else {
                gchar *sfilter1, *sfilter2;
                const gchar *pos;
                gsize len;
                int next_start, max_w=borderlen-11;

                do {
                    if (len_filt > max_w) {
                        sfilter1 = g_strndup( (gchar *) filter, (gsize) max_w);
                        /
                        pos = g_strrstr(sfilter1, " ");
                        if (pos) {
                            len = (gsize)(pos-sfilter1);
                            next_start = (int) len+1;
                        } else {
                            len = (gsize) strlen(sfilter1);
                            next_start = (int)len;
                        }
                        sfilter2 = g_strndup(sfilter1, len);
                        printf("%s%s|\n", sfilter2, &spaces[len+10]);
                        g_free(sfilter1);
                        g_free(sfilter2);

                        printf("|        ");
                        filter = &filter[next_start];
                        len_filt = (int) strlen(filter);
                    } else {
                        printf("%s%s|\n", filter, &spaces[((int)strlen(filter))+10]);
                        break;
                    }
                } while (1);
            }
        }
    }

    printf("|-");
    for(i=0;i<borderlen-3;i++){
        printf("-");
    }
    printf("|\n");

    /
    spaces_s = &spaces[borderlen-(invl_col_w-2)];
    printf("|%s|", spaces_s);

    /
    for(j=0; j<num_cols; j++) {
        item = stat_cols[j];
        if(item->calc_type==CALC_TYPE_FRAMES_AND_BYTES)
            spaces_s = &spaces[borderlen - (col_w[j].fr + col_w[j].val)] - 3;
        else if (item->calc_type==CALC_TYPE_FRAMES)
            spaces_s = &spaces[borderlen - col_w[j].fr];
        else
            spaces_s = &spaces[borderlen - col_w[j].val];

        printf("%-2u%s|", j+1, spaces_s);
    }
    if (tabrow_w < borderlen) {
        filler_s = &spaces[tabrow_w+1];
        printf("%s|", filler_s);
    }

    k = 11;
    switch (timestamp_get_type()) {
    case TS_ABSOLUTE:
        printf("\n| Time    ");
        break;
    case TS_ABSOLUTE_WITH_DATE:
        printf("\n| Date and time");
        k = 16;
        break;
    case TS_RELATIVE:
    case TS_NOT_SET:
        printf("\n| Interval");
        break;
    default:
        break;
    }

    spaces_s = &spaces[borderlen-(invl_col_w-k)];
    printf("%s|", spaces_s);

    /
    for(j=0; j<num_cols; j++) {
        type = stat_cols[j]->calc_type;
        if(type==CALC_TYPE_FRAMES) {
            printcenter (calc_type_table[type].func_name, col_w[j].fr, numpad);
        } else if (type==CALC_TYPE_FRAMES_AND_BYTES) {
            printcenter ("Frames", col_w[j].fr, numpad);
            printcenter ("Bytes", col_w[j].val, numpad);
        } else {
            printcenter (calc_type_table[type].func_name, col_w[j].val, numpad);
        }
    }
    if (filler_s)
        printf("%s|", filler_s);
    printf("\n|-");

    for(i=0; i<tabrow_w-3; i++)
        printf("-");
    printf("|");

    if (tabrow_w < borderlen)
        printf("%s|", &spaces[tabrow_w+1]);

    printf("\n");
    t=0;
    full_fmt = g_strconcat("| ", invl_fmt, " <> ", invl_fmt, " |", NULL);
    num_rows = (int)(duration/interval) + (((duration%interval+500000)/1000000) > 0 ? 1 : 0);

    /
    item_in_column = (io_stat_item_t **) g_malloc(sizeof(io_stat_item_t *) * num_cols);
    for (j=0; j<num_cols; j++) {
        item_in_column[j] = stat_cols[j];
    }

    /
    for (i=0; i<num_rows; i++) {

        if (i==num_rows-1)
            last_row = TRUE;

        /
        if (!last_row) {
            invl_end = t + interval;
        } else {
            invl_end = duration;
        }

        /
        the_time=iot->start_time+(guint32)(t/1000000);
        tm_time = localtime(&the_time);

        /
        switch (timestamp_get_type()) {
        case TS_ABSOLUTE:
          printf("| %02d:%02d:%02d |",
             tm_time->tm_hour,
             tm_time->tm_min,
             tm_time->tm_sec);
          break;

        case TS_ABSOLUTE_WITH_DATE:
          printf("| %04d-%02d-%02d %02d:%02d:%02d |",
             tm_time->tm_year + 1900,
             tm_time->tm_mon + 1,
             tm_time->tm_mday,
             tm_time->tm_hour,
             tm_time->tm_min,
             tm_time->tm_sec);
          break;

        case TS_RELATIVE:
        case TS_NOT_SET:

          if (invl_prec==0) {
              printf(full_fmt, (guint32)(t/1000000),
                               (guint32)(invl_end/1000000));
          } else {
              printf(full_fmt, (guint32)(t/1000000),
                               (guint32)(t%1000000) / dv,
                               (guint32) (invl_end/1000000),
                               (guint32)((invl_end%1000000) / dv));
          }
          break;
     /
        default:
          break;
        }

        /
        for (j=0; j<num_cols; j++) {
            fmt = fmts[j];
            item = item_in_column[j];

            if (item) {
                switch(item->calc_type) {
                case CALC_TYPE_FRAMES:
                    printf(fmt, item->frames);
                    break;
                case CALC_TYPE_BYTES:
                case CALC_TYPE_COUNT:
                    printf(fmt, item->counter);
                    break;
                case CALC_TYPE_FRAMES_AND_BYTES:
                    printf(fmt, item->frames, item->counter);
                    break;

                case CALC_TYPE_SUM:
                case CALC_TYPE_MIN:
                case CALC_TYPE_MAX:
                    ftype = proto_registrar_get_ftype(stat_cols[j]->hf_index);
                    switch(ftype){
                    case FT_FLOAT:
                        printf(fmt, item->float_counter);
                        break;
                    case FT_DOUBLE:
                        printf(fmt, item->double_counter);
                        break;
                    case FT_RELATIVE_TIME:
                        item->counter = (item->counter + 500) / 1000;
                        printf(fmt, (int)(item->counter/1000000), (int)(item->counter%1000000));
                        break;
                    default:
                        printf(fmt, item->counter);
                        break;
                    }
                    break;

                case CALC_TYPE_AVG:
                    num = item->num;
                    if(num==0)
                        num=1;
                    ftype = proto_registrar_get_ftype(stat_cols[j]->hf_index);
                    switch(ftype){
                    case FT_FLOAT:
                        printf(fmt, item->float_counter/num);
                        break;
                    case FT_DOUBLE:
                        printf(fmt, item->double_counter/num);
                        break;
                    case FT_RELATIVE_TIME:
                        item->counter = ((item->counter/num) + 500) / 1000;
                        printf(fmt,
                            (int)(item->counter/1000000), (int)(item->counter%1000000));
                        break;
                    default:
                        printf(fmt, item->counter/num);
                        break;
                    }
                    break;

                case CALC_TYPE_LOAD:
                    ftype = proto_registrar_get_ftype(stat_cols[j]->hf_index);
                    switch(ftype){
                    case FT_RELATIVE_TIME:
                        if (!last_row) {
                            printf(fmt,
                                (int) (item->counter/interval),
                                (int)((item->counter%interval)*1000000 / interval));
                        } else {
                            printf(fmt,
                                (int) (item->counter/(invl_end-t)),
                                (int)((item->counter%(invl_end-t))*1000000 / (invl_end-t)));
                        }
                        break;
                    }
                    break;
                }

                if (last_row) {
                    if (fmt)
                        g_free(fmt);
                } else {
                    item_in_column[j] = item_in_column[j]->next;
                }
            } else {
                printf(fmt, (guint64)0, (guint64)0);
            }
        }
        if (filler_s)
            printf("%s|", filler_s);
        printf("\n");
        t += interval;

    }
    for(i=0;i<borderlen;i++){
        printf("=");
    }
    printf("\n");
    g_free(iot->items);
    g_free(iot->max_vals);
    g_free(iot->max_frame);
    g_free(iot);
    g_free(col_w);
    g_free(invl_fmt);
    g_free(full_fmt);
    g_free(fmts);
    g_free(spaces);
    g_free(stat_cols);
    g_free(item_in_column);
}
