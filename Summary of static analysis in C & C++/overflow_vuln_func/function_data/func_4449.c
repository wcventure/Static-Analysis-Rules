static int decode0(GetByteContext *gb, RangeCoder *rc, unsigned cumFreq, unsigned freq, unsigned total_freq)
{
    int t;

    if (total_freq == 0)
        return AVERROR_INVALIDDATA;

    t = rc->range * (uint64_t)cumFreq / total_freq;

    rc->code1 += t + 1;
    rc->range = rc->range * (uint64_t)(freq + cumFreq) / total_freq - (t + 1);

    while (rc->range < TOP && bytestream2_get_bytes_left(gb) > 0) {
        unsigned byte = bytestream2_get_byte(gb);
        rc->code = (rc->code << 8) | byte;
        rc->code1 <<= 8;
        rc->range <<= 8;
    }

    return 0;
}
