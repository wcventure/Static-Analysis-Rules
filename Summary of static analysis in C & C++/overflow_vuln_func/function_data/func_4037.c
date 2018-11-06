static int decode_dsw1(uint8_t *frame, int width, int height,
                       const uint8_t *src, const uint8_t *src_end)
{
    const uint8_t *frame_start = frame;
    const uint8_t *frame_end   = frame + width * height;
    int mask = 0x10000, bitbuf = 0;
    int v, offset, count, segments;

    segments = bytestream_get_le16(&src);
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
            // can't use av_memcpy_backptr() since it can overwrite following pixels
            for (v = 0; v < count; v++)
                frame[v] = frame[v - offset];
            frame += count;
        } else if (bitbuf & (mask << 1)) {
            frame += bytestream_get_le16(&src);
        } else {
            *frame++ = *src++;
            *frame++ = *src++;
        }
        mask <<= 2;
    }

    return 0;
}
