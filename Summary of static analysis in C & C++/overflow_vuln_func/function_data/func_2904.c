static direntry_t *create_short_filename(BDRVVVFATState *s,
                                         const char *filename,
                                         unsigned int directory_start)
{
    int i, j = 0;
    direntry_t *entry = array_get_next(&(s->directory));
    const gchar *p, *last_dot = NULL;
    gunichar c;
    bool lossy_conversion = false;
    char tail[11];

    if (!entry) {
        return NULL;
    }
    memset(entry->name, 0x20, sizeof(entry->name));

    /
    for (p = filename; ; p = g_utf8_next_char(p)) {
        c = g_utf8_get_char(p);
        if (c == '\0') {
            break;
        } else if (c == '.') {
            if (j == 0) {
                /
                lossy_conversion = true;
            } else {
                if (last_dot) {
                    lossy_conversion = true;
                }
                last_dot = p;
            }
        } else if (!last_dot) {
            /
            uint8_t v = to_valid_short_char(c);
            if (j < 8 && v) {
                entry->name[j++] = v;
            } else {
                lossy_conversion = true;
            }
        }
    }

    /
    if (last_dot) {
        j = 0;
        for (p = g_utf8_next_char(last_dot); ; p = g_utf8_next_char(p)) {
            c = g_utf8_get_char(p);
            if (c == '\0') {
                break;
            } else {
                /
                uint8_t v = to_valid_short_char(c);
                if (j < 3 && v) {
                    entry->name[8 + (j++)] = v;
                } else {
                    lossy_conversion = true;
                }
            }
        }
    }

    if (entry->name[0] == DIR_KANJI) {
        entry->name[0] = DIR_KANJI_FAKE;
    }

    /
    for (j = 0; j < 8; j++) {
        if (entry->name[j] == ' ') {
            break;
        }
    }
    for (i = lossy_conversion ? 1 : 0; i < 999999; i++) {
        direntry_t *entry1;
        if (i > 0) {
            int len = sprintf(tail, "~%d", i);
            memcpy(entry->name + MIN(j, 8 - len), tail, len);
        }
        for (entry1 = array_get(&(s->directory), directory_start);
             entry1 < entry; entry1++) {
            if (!is_long_name(entry1) &&
                !memcmp(entry1->name, entry->name, 11)) {
                break; /
            }
        }
        if (entry1 == entry) {
            /
            return entry;
        }
    }
    return NULL;
}
