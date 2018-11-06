static char **breakline(char *input, int *count)
{
    int c = 0;
    char *p;
    char **rval = g_malloc0(sizeof(char *));
    char **tmp;

    while (rval && (p = qemu_strsep(&input, " ")) != NULL) {
        if (!*p) {
            continue;
        }
        c++;
        tmp = g_realloc(rval, sizeof(*rval) * (c + 1));
        if (!tmp) {
            g_free(rval);
            rval = NULL;
            c = 0;
            break;
        } else {
            rval = tmp;
        }
        rval[c - 1] = p;
        rval[c] = NULL;
    }
    *count = c;
    return rval;
}
