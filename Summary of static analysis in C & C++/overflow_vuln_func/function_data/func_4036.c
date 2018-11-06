static int decode_tsw1(uint8_t *frame, int width, int height,
                       const uint8_t *src, const uint8_t *src_end)
{
    const uint8_t *frame_start = frame;
    const uint8_t *frame_end   = frame + width * height;
    int mask = 0x10000, bitbuf = 0;
    int v, offset, count, segments;

    segments = bytestream_get_le32(&src);
    frame   += bytestream_get_le32(&src);
    if (frame < frame_start || frame > frame_end)
        return -1;
    while (segments--) {
        if (mask == 0x10000) {
            if (src >= src_end)
                return -1;
            bitbuf = bytestream_get_le16(&src);
            mask = 1;
        }
        if (src_end - src < 2 || frame_end - frame < 2)
            return -1;
        if (bitbuf & mask) {
            v = bytestream_get_le16(&src);
            offset = (v & 0x1FFF) << 1;
            count = ((v >> 13) + 2) << 1;
            if (frame - offset < frame_start || frame_end - frame < count)
                return -1;
            av_memcpy_backptr(frame, offset, count);
            frame += count;
        } else {
            *frame++ = *src++;
            *frame++ = *src++;
        }
        mask <<= 1;
    }

    return 0;
}
