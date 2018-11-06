static void add_entry(TiffEncoderContext * s,
                      enum TiffTags tag, enum TiffTypes type, int count,
                      const void *ptr_val)
{
    uint8_t *entries_ptr = s->entries + 12 * s->num_entries;

    av_assert0(s->num_entries < TIFF_MAX_ENTRY);

    bytestream_put_le16(&entries_ptr, tag);
    bytestream_put_le16(&entries_ptr, type);
    bytestream_put_le32(&entries_ptr, count);

    if (type_sizes[type] * count <= 4) {
        tnput(&entries_ptr, count, ptr_val, type, 0);
    } else {
        bytestream_put_le32(&entries_ptr, *s->buf - s->buf_start);
        check_size(s, count * type_sizes2[type]);
        tnput(s->buf, count, ptr_val, type, 0);
    }

    s->num_entries++;
}
