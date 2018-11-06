static void xan_unpack(unsigned char *dest, const unsigned char *src, int dest_len)
{
    unsigned char opcode;
    int size;
    int offset;
    int byte1, byte2, byte3;
    unsigned char *dest_end = dest + dest_len;

    for (;;) {
        opcode = *src++;

        if ( (opcode & 0x80) == 0 ) {

            offset = *src++;

            size = opcode & 3;
            if (dest + size > dest_end)
                return;
            memcpy(dest, src, size);  dest += size;  src += size;

            size = ((opcode & 0x1c) >> 2) + 3;
            if (dest + size > dest_end)
                return;
            av_memcpy_backptr(dest, ((opcode & 0x60) << 3) + offset + 1, size);
            dest += size;

        } else if ( (opcode & 0x40) == 0 ) {

            byte1 = *src++;
            byte2 = *src++;

            size = byte1 >> 6;
            if (dest + size > dest_end)
                return;
            memcpy(dest, src, size);  dest += size;  src += size;

            size = (opcode & 0x3f) + 4;
            if (dest + size > dest_end)
                return;
            av_memcpy_backptr(dest, ((byte1 & 0x3f) << 8) + byte2 + 1, size);
            dest += size;

        } else if ( (opcode & 0x20) == 0 ) {

            byte1 = *src++;
            byte2 = *src++;
            byte3 = *src++;

            size = opcode & 3;
            if (dest + size > dest_end)
                return;
            memcpy(dest, src, size);  dest += size;  src += size;

            size = byte3 + 5 + ((opcode & 0xc) << 6);
            if (dest + size > dest_end)
                return;
            av_memcpy_backptr(dest,
                ((opcode & 0x10) << 12) + 1 + (byte1 << 8) + byte2,
                size);
            dest += size;
        } else {
            size = ((opcode & 0x1f) << 2) + 4;

            if (size > 0x70)
                break;

            if (dest + size > dest_end)
                return;
            memcpy(dest, src, size);  dest += size;  src += size;
        }
    }

    size = opcode & 3;
    memcpy(dest, src, size);  dest += size;  src += size;
}
