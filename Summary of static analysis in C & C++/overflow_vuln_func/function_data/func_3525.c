static void seqvideo_decode(SeqVideoContext *seq, const unsigned char *data, int data_size)
{
    GetBitContext gb;
    int flags, i, j, x, y, op;
    unsigned char c[3];
    unsigned char *dst;
    uint32_t *palette;

    flags = *data++;

    if (flags & 1) {
        palette = (uint32_t *)seq->frame.data[1];
        for (i = 0; i < 256; i++) {
            for (j = 0; j < 3; j++, data++)
                c[j] = (*data << 2) | (*data >> 4);
            palette[i] = AV_RB24(c);
        }
        seq->frame.palette_has_changed = 1;
    }

    if (flags & 2) {
        init_get_bits(&gb, data, 128 * 8); data += 128;
        for (y = 0; y < 128; y += 8)
            for (x = 0; x < 256; x += 8) {
                dst = &seq->frame.data[0][y * seq->frame.linesize[0] + x];
                op = get_bits(&gb, 2);
                switch (op) {
                case 1:
                    data = seq_decode_op1(seq, data, dst);
                    break;
                case 2:
                    data = seq_decode_op2(seq, data, dst);
                    break;
                case 3:
                    data = seq_decode_op3(seq, data, dst);
                    break;
                }
            }
    }
}
