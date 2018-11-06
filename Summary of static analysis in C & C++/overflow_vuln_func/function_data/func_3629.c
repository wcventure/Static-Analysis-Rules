static int decode_wdlt(GetByteContext *gb, uint8_t *frame, int width, int height)
{
    const uint8_t *frame_end   = frame + width * height;
    uint8_t *line_ptr;
    int count, i, v, lines, segments;
    int y = 0;

    lines = bytestream2_get_le16(gb);
    if (lines > height)
        return AVERROR_INVALIDDATA;

    while (lines--) {
        if (bytestream2_get_bytes_left(gb) < 2)
            return AVERROR_INVALIDDATA;
        segments = bytestream2_get_le16u(gb);
        while ((segments & 0xC000) == 0xC000) {
            unsigned skip_lines = -(int16_t)segments;
            unsigned delta = -((int16_t)segments * width);
            if (frame_end - frame <= delta || y + lines + skip_lines > height)
                return AVERROR_INVALIDDATA;
            frame    += delta;
            y        += skip_lines;
            segments = bytestream2_get_le16(gb);
        }

        if (frame_end <= frame)
            return AVERROR_INVALIDDATA;
        if (segments & 0x8000) {
            frame[width - 1] = segments & 0xFF;
            segments = bytestream2_get_le16(gb);
        }
        line_ptr = frame;
        if (frame_end - frame < width)
            return AVERROR_INVALIDDATA;
        frame += width;
        y++;
        while (segments--) {
            if (frame - line_ptr <= bytestream2_peek_byte(gb))
                return AVERROR_INVALIDDATA;
            line_ptr += bytestream2_get_byte(gb);
            count = (int8_t)bytestream2_get_byte(gb);
            if (count >= 0) {
                if (frame - line_ptr < count * 2)
                    return AVERROR_INVALIDDATA;
                if (bytestream2_get_buffer(gb, line_ptr, count * 2) != count * 2)
                    return AVERROR_INVALIDDATA;
                line_ptr += count * 2;
            } else {
                count = -count;
                if (frame - line_ptr < count * 2)
                    return AVERROR_INVALIDDATA;
                v = bytestream2_get_le16(gb);
                for (i = 0; i < count; i++)
                    bytestream_put_le16(&line_ptr, v);
            }
        }
    }

    return 0;
}
