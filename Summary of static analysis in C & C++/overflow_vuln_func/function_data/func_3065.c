static int mpc8_probe(AVProbeData *p)
{
    const uint8_t *bs = p->buf + 4;
    const uint8_t *bs_end = bs + p->buf_size;
    int64_t size;

    if (p->buf_size < 16)
        return 0;
    if (AV_RL32(p->buf) != TAG_MPCK)
        return 0;
    while (bs < bs_end + 3) {
        int header_found = (bs[0] == 'S' && bs[1] == 'H');
        if (bs[0] < 'A' || bs[0] > 'Z' || bs[1] < 'A' || bs[1] > 'Z')
            return 0;
        bs += 2;
        size = bs_get_v(&bs);
        if (size < 2)
            return 0;
        if (bs + size - 2 >= bs_end)
            return AVPROBE_SCORE_EXTENSION - 1; // seems to be valid MPC but no header yet
        if (header_found) {
            if (size < 11 || size > 28)
                return 0;
            if (!AV_RL32(bs)) //zero CRC is invalid
                return 0;
            return AVPROBE_SCORE_MAX;
        } else {
            bs += size - 2;
        }
    }
    return 0;
}
