static void print_type_size(Visitor *v, uint64_t *obj, const char *name,
                           Error **errp)
{
    StringOutputVisitor *sov = DO_UPCAST(StringOutputVisitor, visitor, v);
    static const char suffixes[] = { 'B', 'K', 'M', 'G', 'T' };
    uint64_t div, val;
    char *out;
    int i;

    if (!sov->human) {
        out = g_strdup_printf("%llu", (long long) *obj);
        string_output_set(sov, out);
        return;
    }

    val = *obj;

    /
    i = 64 - clz64(val);

    /
    i /= 10;
    if (i >= ARRAY_SIZE(suffixes)) {
        i = ARRAY_SIZE(suffixes) - 1;
    }
    div = 1ULL << (i * 10);

    out = g_strdup_printf("%0.03f%c", (double)val/div, suffixes[i]);
    string_output_set(sov, out);
}
