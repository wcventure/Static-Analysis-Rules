static void print_type_size(Visitor *v, const char *name, uint64_t *obj,
                            Error **errp)
{
    StringOutputVisitor *sov = to_sov(v);
    static const char suffixes[] = { 'B', 'K', 'M', 'G', 'T', 'P', 'E' };
    uint64_t div, val;
    char *out;
    int i;

    if (!sov->human) {
        out = g_strdup_printf("%"PRIu64, *obj);
        string_output_set(sov, out);
        return;
    }

    val = *obj;

    /
    frexp(val / (1000.0 / 1024.0), &i);
    i = (i - 1) / 10;
    assert(i < ARRAY_SIZE(suffixes));
    div = 1ULL << (i * 10);

    out = g_strdup_printf("%"PRIu64" (%0.3g %c%s)", val,
                          (double)val/div, suffixes[i], i ? "iB" : "");
    string_output_set(sov, out);
}
