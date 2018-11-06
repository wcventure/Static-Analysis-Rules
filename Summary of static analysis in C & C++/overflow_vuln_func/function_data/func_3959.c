static int decode_wdlt(uint8_t *frame, int width, int height,
                       const uint8_t *src, const uint8_t *src_end)
{
    const uint8_t *frame_end   = frame + width * height;
    uint8_t *line_ptr;
    int count, i, v, lines, segments;

    lines = bytestream_get_le16(&src);
    if (frame + lines * width > frame_end || src >= src_end)
        return -1;

    while (lines--) {
        segments = bytestream_get_le16(&src);
        while ((segments & 0xC000) == 0xC000) {
            frame    -= (int16_t)segments * width;
            if (frame >= frame_end)
                return -1;
            segments = bytestream_get_le16(&src);
        }
        if (segments & 0x8000) {
            frame[width - 1] = segments & 0xFF;
            segments = bytestream_get_le16(&src);
        }
        line_ptr = frame;
        frame += width;
        while (segments--) {
            if (src_end - src < 2)
                return -1;
            line_ptr += *src++;
            if (line_ptr >= frame)
                return -1;
            count = (int8_t)*src++;
            if (count >= 0) {
                if (line_ptr + count*2 > frame || src_end - src < count*2)
                    return -1;
                bytestream_get_buffer(&src, line_ptr, count*2);
                line_ptr += count * 2;
            } else {
                count = -count;
                if (line_ptr + count*2 > frame || src_end - src < 2)
                    return -1;
                v = bytestream_get_le16(&src);
                for (i = 0; i < count; i++)
                    bytestream_put_le16(&line_ptr, v);
            }
        }
    }

    return 0;
}
