static gint
index_of_char(const char *str, const char c)
{
    gint len = 0;
    const char *p = str;

    while (*p && *p != c) {
        p++;
        len++;
    }

    if (*p)
        return len;
    return -1;
}
