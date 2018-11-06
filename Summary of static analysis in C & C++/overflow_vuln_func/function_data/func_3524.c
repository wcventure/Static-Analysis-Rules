static const unsigned char *seq_decode_op3(SeqVideoContext *seq, const unsigned char *src, unsigned char *dst)
{
    int pos, offset;

    do {
        pos = *src++;
        offset = ((pos >> 3) & 7) * seq->frame.linesize[0] + (pos & 7);
        dst[offset] = *src++;
    } while (!(pos & 0x80));

    return src;
}
