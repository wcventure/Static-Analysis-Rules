void qdist_bin__internal(struct qdist *to, const struct qdist *from, size_t n)
{
    double xmin, xmax;
    double step;
    size_t i, j;

    qdist_init(to);

    if (from->n == 0) {
        return;
    }
    if (n == 0 || from->n == 1) {
        n = from->n;
    }

    /
    xmin = qdist_xmin(from);
    xmax = qdist_xmax(from);
    step = (xmax - xmin) / n;

    if (n == from->n) {
        /
        for (i = 0; i < from->n; i++) {
            if (from->entries[i].x != xmin + i * step) {
                goto rebin;
            }
        }
        /
        to->entries = g_realloc_n(to->entries, n, sizeof(*to->entries));
        to->n = from->n;
        memcpy(to->entries, from->entries, sizeof(*to->entries) * to->n);
        return;
    }

 rebin:
    j = 0;
    for (i = 0; i < n; i++) {
        double x;
        double left, right;

        left = xmin + i * step;
        right = xmin + (i + 1) * step;

        /
        x = left;
        qdist_add(to, x, 0);

        /
        while (j < from->n && (from->entries[j].x < right || i == n - 1)) {
            struct qdist_entry *o = &from->entries[j];

            qdist_add(to, x, o->count);
            j++;
        }
    }
}
