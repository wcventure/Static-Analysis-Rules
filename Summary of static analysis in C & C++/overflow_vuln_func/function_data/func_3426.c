static int xan_unpack(uint8_t *dest, const int dest_len,
                      const uint8_t *src, const int src_len)
{
    uint8_t opcode;
    int size;
    uint8_t *orig_dest = dest;
    const uint8_t *src_end = src + src_len;
    const uint8_t *dest_end = dest + dest_len;

    while (dest < dest_end) {
        opcode = *src++;

        if (opcode < 0xe0) {
            int size2, back;
            if ((opcode & 0x80) == 0) {
                size  = opcode & 3;
                back  = ((opcode & 0x60) << 3) + *src++ + 1;
                size2 = ((opcode & 0x1c) >> 2) + 3;
            } else if ((opcode & 0x40) == 0) {
                size  = *src >> 6;
                back  = (bytestream_get_be16(&src) & 0x3fff) + 1;
                size2 = (opcode & 0x3f) + 4;
            } else {
                size  = opcode & 3;
                back  = ((opcode & 0x10) << 12) + bytestream_get_be16(&src) + 1;
                size2 = ((opcode & 0x0c) <<  6) + *src++ + 5;
                if (size + size2 > dest_end - dest)
                    break;
            }
            if (src + size > src_end || dest + size + size2 > dest_end)
                return -1;
            bytestream_get_buffer(&src, dest, size);
            dest += size;
            av_memcpy_backptr(dest, back, size2);
            dest += size2;
        } else {
            int finish = opcode >= 0xfc;

            size = finish ? opcode & 3 : ((opcode & 0x1f) << 2) + 4;
            if (src + size > src_end || dest + size > dest_end)
                return -1;
            bytestream_get_buffer(&src, dest, size);
            dest += size;
            if (finish)
                break;
        }
    }
    return dest - orig_dest;
}
