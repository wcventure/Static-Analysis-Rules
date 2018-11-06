static int cmp_pkt_sub_ts_pos(const void *a, const void *b)
{
    const AVPacket *s1 = a;
    const AVPacket *s2 = b;
    if (s1->pts == s2->pts) {
        if (s1->pos == s2->pos)
            return 0;
        return s1->pos > s2->pos ? 1 : -1;
    }
    return s1->pts > s2->pts ? 1 : -1;
}
