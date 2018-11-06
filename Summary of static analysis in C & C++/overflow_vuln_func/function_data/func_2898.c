static void do_send_key(const char *string)
{
    char keybuf[16], *q;
    uint8_t keycodes[16];
    const char *p;
    int nb_keycodes, keycode, i;

    nb_keycodes = 0;
    p = string;
    while (*p != '\0') {
        q = keybuf;
        while (*p != '\0' && *p != '-') {
            if ((q - keybuf) < sizeof(keybuf) - 1) {
                *q++ = *p;
            }
            p++;
        }
        *q = '\0';
        keycode = get_keycode(keybuf);
        if (keycode < 0) {
            term_printf("unknown key: '%s'\n", keybuf);
            return;
        }
        keycodes[nb_keycodes++] = keycode;
        if (*p == '\0')
            break;
        p++;
    }
    /
    for(i = 0; i < nb_keycodes; i++) {
        keycode = keycodes[i];
        if (keycode & 0x80)
            kbd_put_keycode(0xe0);
        kbd_put_keycode(keycode & 0x7f);
    }
    /
    for(i = nb_keycodes - 1; i >= 0; i--) {
        keycode = keycodes[i];
        if (keycode & 0x80)
            kbd_put_keycode(0xe0);
        kbd_put_keycode(keycode | 0x80);
    }
}
