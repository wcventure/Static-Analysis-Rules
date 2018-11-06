static int mov_read_mac_string(MOVContext *c, AVIOContext *pb, int len,
                               char *dst, int dstlen)
{
    char *p = dst;
    char *end = dst+dstlen-1;
    int i;

    for (i = 0; i < len; i++) {
        uint8_t t, c = avio_r8(pb);
        if (c < 0x80 && p < end)
            *p++ = c;
        else
            PUT_UTF8(mac_to_unicode[c-0x80], t, if (p < end) *p++ = t;);
    }
    *p = 0;
    return p - dst;
}
