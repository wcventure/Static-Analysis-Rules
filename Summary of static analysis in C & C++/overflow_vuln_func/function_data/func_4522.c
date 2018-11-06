static int xan_huffman_decode(unsigned char *dest, const unsigned char *src,
    int dest_len)
{
    unsigned char byte = *src++;
    unsigned char ival = byte + 0x16;
    const unsigned char * ptr = src + byte*2;
    unsigned char val = ival;
    unsigned char *dest_end = dest + dest_len;
    GetBitContext gb;

    init_get_bits(&gb, ptr, 0); // FIXME: no src size available

    while ( val != 0x16 ) {
        val = src[val - 0x17 + get_bits1(&gb) * byte];

        if ( val < 0x16 ) {
            if (dest + 1 > dest_end)
                return 0;
            *dest++ = val;
            val = ival;
        }
    }

    return 0;
}
