static const unsigned char *seq_decode_op1(SeqVideoContext *seq, const unsigned char *src, unsigned char *dst)
{
    const unsigned char *color_table;
    int b, i, len, bits;
    GetBitContext gb;
    unsigned char block[8 * 8];

    len = *src++;
    if (len & 0x80) {
        switch (len & 3) {
        case 1:
            src = seq_unpack_rle_block(src, block, sizeof(block));
            for (b = 0; b < 8; b++) {
                memcpy(dst, &block[b * 8], 8);
                dst += seq->frame.linesize[0];
            }
            break;
        case 2:
            src = seq_unpack_rle_block(src, block, sizeof(block));
            for (i = 0; i < 8; i++) {
                for (b = 0; b < 8; b++)
                    dst[b * seq->frame.linesize[0]] = block[i * 8 + b];
                ++dst;
            }
            break;
        }
    } else {
        color_table = src;
        src += len;
        bits = ff_log2_tab[len - 1] + 1;
        init_get_bits(&gb, src, bits * 8 * 8); src += bits * 8;
        for (b = 0; b < 8; b++) {
            for (i = 0; i < 8; i++)
                dst[i] = color_table[get_bits(&gb, bits)];
            dst += seq->frame.linesize[0];
        }
    }

    return src;
}
