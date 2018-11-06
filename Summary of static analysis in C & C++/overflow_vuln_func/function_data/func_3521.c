static const unsigned char *seq_unpack_rle_block(const unsigned char *src, unsigned char *dst, int dst_size)
{
    int i, len, sz;
    GetBitContext gb;
    int code_table[64];

    /
    init_get_bits(&gb, src, 64 * 8);
    for (i = 0, sz = 0; i < 64 && sz < dst_size; i++) {
        code_table[i] = get_sbits(&gb, 4);
        sz += FFABS(code_table[i]);
    }
    src += (get_bits_count(&gb) + 7) / 8;

    /
    for (i = 0; i < 64 && dst_size > 0; i++) {
        len = code_table[i];
        if (len < 0) {
            len = -len;
            memset(dst, *src++, FFMIN(len, dst_size));
        } else {
            memcpy(dst, src, FFMIN(len, dst_size));
            src += len;
        }
        dst += len;
        dst_size -= len;
    }
    return src;
}
