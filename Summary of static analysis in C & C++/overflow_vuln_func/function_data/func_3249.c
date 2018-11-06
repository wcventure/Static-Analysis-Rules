static int
matroska_probe (AVProbeData *p)
{
    uint64_t total = 0;
    int len_mask = 0x80, size = 1, n = 1;
    uint8_t probe_data[] = { 'm', 'a', 't', 'r', 'o', 's', 'k', 'a' };

    if (p->buf_size < 5)
        return 0;

    /
    if ((p->buf[0] << 24 | p->buf[1] << 16 |
         p->buf[2] << 8 | p->buf[3]) != EBML_ID_HEADER)
        return 0;

    /
    total = p->buf[4];
    while (size <= 8 && !(total & len_mask)) {
        size++;
        len_mask >>= 1;
    }
    if (size > 8)
      return 0;
    total &= (len_mask - 1);
    while (n < size)
        total = (total << 8) | p->buf[4 + n++];

    /
    if (p->buf_size < 4 + size + total)
      return 0;

    /
    for (n = 4 + size; n < 4 + size + total - sizeof(probe_data); n++)
        if (!memcmp (&p->buf[n], probe_data, sizeof(probe_data)))
            return AVPROBE_SCORE_MAX;

    return 0;
}
