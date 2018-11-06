static int read_probe(AVProbeData *pd)
{
    if (pd->buf[0] == 'J' && pd->buf[1] == 'V' && strlen(MAGIC) <= pd->buf_size - 4 &&
        !memcmp(pd->buf + 4, MAGIC, strlen(MAGIC)))
        return AVPROBE_SCORE_MAX;
    return 0;
}
