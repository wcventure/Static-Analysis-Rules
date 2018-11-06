static int csrhci_write(struct CharDriverState *chr,
                const uint8_t *buf, int len)
{
    struct csrhci_s *s = (struct csrhci_s *) chr->opaque;
    int plen = s->in_len;

    if (!s->enable)
        return 0;

    s->in_len += len;
    memcpy(s->inpkt + plen, buf, len);

    while (1) {
        if (s->in_len >= 2 && plen < 2)
            s->in_hdr = csrhci_header_len(s->inpkt) + 1;

        if (s->in_len >= s->in_hdr && plen < s->in_hdr)
            s->in_data = csrhci_data_len(s->inpkt) + s->in_hdr;

        if (s->in_len >= s->in_data) {
            csrhci_in_packet(s, s->inpkt);

            memmove(s->inpkt, s->inpkt + s->in_len, s->in_len - s->in_data);
            s->in_len -= s->in_data;
            s->in_hdr = INT_MAX;
            s->in_data = INT_MAX;
            plen = 0;
        } else
            break;
    }

    return len;
}
