void ff_lzw_decode_tail(LZWState *p)
{
    struct LZWState *s = (struct LZWState *)p;

    if(s->mode == FF_LZW_GIF) {
        while (s->bs > 0) {
            if (s->pbuf + s->bs >= s->ebuf) {
                s->pbuf = s->ebuf;
                break;
            } else {
                s->pbuf += s->bs;
                s->bs = *s->pbuf++;
            }
        }
    }else
        s->pbuf= s->ebuf;
}
