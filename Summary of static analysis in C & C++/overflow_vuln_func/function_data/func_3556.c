static void decode_delta_j(uint8_t *dst,
                           const uint8_t *buf, const uint8_t *buf_end,
                           int w, int h, int bpp, int dst_size)
{
    int32_t pitch;
    uint8_t *end = dst + dst_size, *ptr;
    uint32_t type, flag, cols, groups, rows, bytes;
    uint32_t offset;
    int planepitch_byte = (w + 7) / 8;
    int planepitch = ((w + 15) / 16) * 2;
    int kludge_j, b, g, r, d;
    GetByteContext gb;

    pitch = planepitch * bpp;
    kludge_j = w < 320 ? (320 - w) / 8 / 2 : 0;

    bytestream2_init(&gb, buf, buf_end - buf);

    while (bytestream2_get_bytes_left(&gb) >= 2) {
        type = bytestream2_get_be16(&gb);

        switch (type) {
        case 0:
            return;
        case 1:
            flag   = bytestream2_get_be16(&gb);
            cols   = bytestream2_get_be16(&gb);
            groups = bytestream2_get_be16(&gb);

            for (g = 0; g < groups; g++) {
                offset = bytestream2_get_be16(&gb);

                if (kludge_j)
                    offset = ((offset / (320 / 8)) * pitch) + (offset % (320 / 8)) - kludge_j;
                else
                    offset = ((offset / planepitch_byte) * pitch) + (offset % planepitch_byte);

                ptr = dst + offset;
                if (ptr >= end)
                    return;

                for (b = 0; b < cols; b++) {
                    for (d = 0; d < bpp; d++) {
                        uint8_t value = bytestream2_get_byte(&gb);

                        if (flag)
                            ptr[0] ^= value;
                        else
                            ptr[0]  = value;

                        ptr += planepitch;
                        if (ptr >= end)
                            return;
                    }
                }
                if ((cols * bpp) & 1)
                    bytestream2_skip(&gb, 1);
            }
            break;
        case 2:
            flag   = bytestream2_get_be16(&gb);
            rows   = bytestream2_get_be16(&gb);
            bytes  = bytestream2_get_be16(&gb);
            groups = bytestream2_get_be16(&gb);

            for (g = 0; g < groups; g++) {
                offset = bytestream2_get_be16(&gb);

                if (kludge_j)
                    offset = ((offset / (320 / 8)) * pitch) + (offset % (320/ 8)) - kludge_j;
                else
                    offset = ((offset / planepitch_byte) * pitch) + (offset % planepitch_byte);

                for (r = 0; r < rows; r++) {
                    for (d = 0; d < bpp; d++) {
                        ptr = dst + offset + (r * pitch) + d * planepitch;
                        if (ptr >= end)
                            return;

                        for (b = 0; b < bytes; b++) {
                            uint8_t value = bytestream2_get_byte(&gb);

                            if (flag)
                                ptr[0] ^= value;
                            else
                                ptr[0]  = value;

                            ptr++;
                            if (ptr >= end)
                                return;
                        }
                    }
                }
                if ((rows * bytes * bpp) & 1)
                    bytestream2_skip(&gb, 1);
            }
            break;
        default:
            return;
        }
    }
}
