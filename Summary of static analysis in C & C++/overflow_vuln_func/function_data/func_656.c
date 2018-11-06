typedef struct _io_stat_t {
    guint64 interval;     /
    guint invl_prec;      /
    guint32 num_cols;     /
    struct _io_stat_item_t *items;  /
    time_t start_time;    /
    const char **filters; /
    guint64 *max_vals;    /
    guint32 *max_frame;   /
} io_stat_t;
