static int decode_bdlt(uint8_t *frame, int width, int height,
                       const uint8_t *src, const uint8_t *src_end)
{
    const uint8_t *frame_end = frame + width * height;
    uint8_t *line_ptr;
    int count, lines, segments;

    count = bytestream_get_le16(&src);
    if (count >= height || width * count < 0)
        return -1;
    frame += width * count;
    lines = bytestream_get_le16(&src);
    if (frame + lines * width > frame_end || src >= src_end)
        return -1;

    while (lines--) {
        line_ptr = frame;
        frame += width;
        segments = *src++;
        while (segments--) {
            if (src_end - src < 3)
                return -1;
            line_ptr += *src++;
            if (line_ptr >= frame)
                return -1;
            count = (int8_t)*src++;
            if (count >= 0) {
                if (line_ptr + count > frame || src_end - src < count)
                    return -1;
                bytestream_get_buffer(&src, line_ptr, count);
            } else {
                count = -count;
                if (line_ptr + count > frame || src >= src_end)
                    return -1;
                memset(line_ptr, *src++, count);
            }
            line_ptr += count;
        }
    }

    return 0;
}
