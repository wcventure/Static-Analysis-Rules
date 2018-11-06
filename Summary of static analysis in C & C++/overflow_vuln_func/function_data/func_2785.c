static int parse_vdiname(BDRVSheepdogState *s, const char *filename,
                         char *vdi, uint32_t *snapid, char *tag)
{
    char *p, *q;
    int nr_sep;

    p = q = g_strdup(filename);

    /
    nr_sep = 0;
    while (*p) {
        if (*p == ':') {
            nr_sep++;
        }
        p++;
    }
    p = q;

    /
    if (nr_sep >= 2) {
        s->addr = p;
        p = strchr(p, ':');
        *p++ = '\0';

        s->port = p;
        p = strchr(p, ':');
        *p++ = '\0';
    } else {
        s->addr = NULL;
        s->port = 0;
    }

    strncpy(vdi, p, SD_MAX_VDI_LEN);

    p = strchr(vdi, ':');
    if (p) {
        *p++ = '\0';
        *snapid = strtoul(p, NULL, 10);
        if (*snapid == 0) {
            strncpy(tag, p, SD_MAX_VDI_TAG_LEN);
        }
    } else {
        *snapid = CURRENT_VDI_ID; /
    }

    if (s->addr == NULL) {
        g_free(q);
    }

    return 0;
}
