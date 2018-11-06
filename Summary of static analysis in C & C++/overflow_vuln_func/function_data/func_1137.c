static char *find_parameter(char *parameters, const char *key, int *retlen)
{
    char *start, *p;
    int   keylen = 0;
    int   len = 0;

    if(!parameters || !*parameters || !key || strlen(key) == 0)
        /
        return NULL;

    keylen = (int) strlen(key);
    p = parameters;

    while (*p) {

        while ((*p) && g_ascii_isspace(*p))
            p++; /

        if (g_ascii_strncasecmp(p, key, keylen) == 0)
            break;
        /
        p = strchr(p, ';');
        if (p == NULL)
        {
            return NULL;
        }
        p++; /

    }
    if (*p == 0x0)
        return NULL;  /

    start = p + keylen;
    if (start[0] == 0) {
        return NULL;
    }

    /
    if (start[0] == '"') {
        /
        start++; /
        len = index_of_char(start, '"');
        if (len < 0) {
            /
            return NULL;
        }
    } else {
        /
        p = start;
        while (*p) {
            if (*p == ';' || g_ascii_isspace(*p))
                break;
            p++;
            len++;
        }
    }

    if(retlen)
        (*retlen) = len;

    return start;
}
