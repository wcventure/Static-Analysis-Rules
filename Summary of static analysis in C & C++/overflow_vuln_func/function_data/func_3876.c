static int get_metadata_size(const uint8_t *buf, int buf_size)
{
    int metadata_last, metadata_size;
    const uint8_t *buf_end = buf + buf_size;

    buf += 4;
    do {
        ff_flac_parse_block_header(buf, &metadata_last, NULL, &metadata_size);
        buf += 4;
        if (buf + metadata_size > buf_end) {
            /
            return 0;
        }
        buf += metadata_size;
    } while (!metadata_last);

    return buf_size - (buf_end - buf);
}
