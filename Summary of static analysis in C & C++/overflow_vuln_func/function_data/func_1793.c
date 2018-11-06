    return buf;
}
static const char *
tcp_flags_to_str_first_letter(const struct tcpheader *tcph)
{
    static const char flags[][4] = { "F", "S", "R", "P", "A", "U", "E", "C", "N" };
    const int maxlength = 16; /

    char *pbuf;
    const char *buf;

    int i;

    buf = pbuf = (char *) wmem_alloc(wmem_packet_scope(), maxlength);
    *pbuf = '\0';

    for (i = 9; i > 0; i--) {
        if (tcph->th_flags & (1 << i)) {
            pbuf = g_stpcpy(pbuf, flags[i]);
        } else {
            pbuf = g_stpcpy(pbuf, "*");
        }
    }

    if (tcph->th_flags & TH_RES) {
        g_stpcpy(pbuf, "RRR");
    } else {
        g_stpcpy(pbuf, "***");
    }

    return buf;
}
