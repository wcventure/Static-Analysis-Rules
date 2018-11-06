static const unsigned char *seq_decode_op2(SeqVideoContext *seq, const unsigned char *src, unsigned char *dst)
{
    int i;

    for (i = 0; i < 8; i++) {
        memcpy(dst, src, 8);
        src += 8;
        dst += seq->frame.linesize[0];
    }

    return src;
}
