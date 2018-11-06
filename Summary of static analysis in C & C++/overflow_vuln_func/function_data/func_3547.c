static int decode_tsw1(GetByteContext *gb, uint8_t *frame, int width, int height)
{
    const uint8_t *frame_start = frame;
    const uint8_t *frame_end   = frame + width * height;
    int mask = 0x10000, bitbuf = 0;
    int v, count, segments;
    unsigned offset;

    segments = bytestream2_get_le32(gb);
    offset   = bytestream2_get_le32(gb);
    if (segments == 0 && offset == frame_end - frame)
        return 0; // skip frame
    if (frame_end - frame <= offset)
        return AVERROR_INVALIDDATA;
    frame += offset;
    while (segments--) {
        if (bytestream2_get_bytes_left(gb) < 2)
            return AVERROR_INVALIDDATA;
        if (mask == 0x10000) {
            bitbuf = bytestream2_get_le16u(gb);
            mask = 1;
        }
        if (frame_end - frame < 2)
            return AVERROR_INVALIDDATA;
        if (bitbuf & mask) {
            v = bytestream2_get_le16(gb);
            offset = (v & 0x1FFF) << 1;
            count = ((v >> 13) + 2) << 1;
            if (frame - frame_start < offset || frame_end - frame < count)
                return AVERROR_INVALIDDATA;
            av_memcpy_backptr(frame, offset, count);
            frame += count;
        } else {
            *frame++ = bytestream2_get_byte(gb);
            *frame++ = bytestream2_get_byte(gb);
        }
        mask <<= 1;
    }

    return 0;
}
