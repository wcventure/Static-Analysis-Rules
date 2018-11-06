static kbd_layout_t *parse_keyboard_layout(const name2keysym_t *table,
                                           const char *language,
                                           kbd_layout_t *k)
{
    FILE *f;
    char * filename;
    char line[1024];
    int len;

    filename = qemu_find_file(QEMU_FILE_TYPE_KEYMAP, language);
    f = filename ? fopen(filename, "r") : NULL;
    g_free(filename);
    if (!f) {
        fprintf(stderr, "Could not read keymap file: '%s'\n", language);
        return NULL;
    }

    if (!k) {
        k = g_malloc0(sizeof(kbd_layout_t));
    }

    for(;;) {
        if (fgets(line, 1024, f) == NULL) {
            break;
        }
        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        if (line[0] == '#') {
            continue;
        }
        if (!strncmp(line, "map ", 4)) {
            continue;
        }
        if (!strncmp(line, "include ", 8)) {
            parse_keyboard_layout(table, line + 8, k);
        } else {
            char *end_of_keysym = line;
            while (*end_of_keysym != 0 && *end_of_keysym != ' ') {
                end_of_keysym++;
            }
            if (*end_of_keysym) {
                int keysym;
                *end_of_keysym = 0;
                keysym = get_keysym(table, line);
                if (keysym == 0) {
                    /
                } else {
                    const char *rest = end_of_keysym + 1;
                    int keycode = strtol(rest, NULL, 0);

                    if (strstr(rest, "numlock")) {
                        add_to_key_range(&k->keypad_range, keycode);
                        add_to_key_range(&k->numlock_range, keysym);
                        /
                    }

                    if (strstr(rest, "shift")) {
                        keycode |= SCANCODE_SHIFT;
                    }
                    if (strstr(rest, "altgr")) {
                        keycode |= SCANCODE_ALTGR;
                    }
                    if (strstr(rest, "ctrl")) {
                        keycode |= SCANCODE_CTRL;
                    }

                    add_keysym(line, keysym, keycode, k);

                    if (strstr(rest, "addupper")) {
                        char *c;
                        for (c = line; *c; c++) {
                            *c = qemu_toupper(*c);
                        }
                        keysym = get_keysym(table, line);
                        if (keysym) {
                            add_keysym(line, keysym,
                                       keycode | SCANCODE_SHIFT, k);
                        }
                    }
                }
            }
        }
    }
    fclose(f);
    return k;
}
