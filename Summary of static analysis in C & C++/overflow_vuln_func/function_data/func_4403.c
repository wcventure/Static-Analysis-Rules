static int flac_probe(AVProbeData *p)
{
    uint8_t *bufptr = p->buf;
    uint8_t *end    = p->buf + p->buf_size;

    if(bufptr > end-4 || memcmp(bufptr, "fLaC", 4)) return 0;
    else                                            return AVPROBE_SCORE_MAX/2;
}
