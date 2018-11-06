int ff_lzw_decode(LZWState *p, uint8_t *buf, int len){
    int l, c, code, oc, fc;
    uint8_t *sp;
    struct LZWState *s = (struct LZWState *)p;

    if (s->end_code < 0)
        return 0;

    l = len;
    sp = s->sp;
    oc = s->oc;
    fc = s->fc;

    for (;;) {
        while (sp > s->stack) {
            *buf++ = *(--sp);
            if ((--l) == 0)
                goto the_end;
        }
        c = lzw_get_code(s);
        if (c == s->end_code) {
            s->end_code = -1;
            break;
        } else if (c == s->clear_code) {
            s->cursize = s->codesize + 1;
            s->curmask = mask[s->cursize];
            s->slot = s->newcodes;
            s->top_slot = 1 << s->cursize;
            fc= oc= -1;
        } else {
            code = c;
            if (code >= s->slot) {
                *sp++ = fc;
                code = oc;
            }
            while (code >= s->newcodes) {
                *sp++ = s->suffix[code];
                code = s->prefix[code];
            }
            *sp++ = code;
            if (s->slot < s->top_slot && oc>=0) {
                s->suffix[s->slot] = code;
                s->prefix[s->slot++] = oc;
            }
            fc = code;
            oc = c;
            if (s->slot >= s->top_slot - s->extra_slot) {
                if (s->cursize < LZW_MAXBITS) {
                    s->top_slot <<= 1;
                    s->curmask = mask[++s->cursize];
                }
            }
        }
    }
  the_end:
    s->sp = sp;
    s->oc = oc;
    s->fc = fc;
    return len - l;
}
