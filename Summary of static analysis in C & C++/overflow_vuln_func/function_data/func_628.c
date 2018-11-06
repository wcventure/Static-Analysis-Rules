gint
dissect_uleb128(tvbuff_t *tvb, gint offset, guint64 *value)
{
    guint  start_offset = offset;
    guint  shift = 0;
    guint8 byte;

    *value = 0;

    do {
        byte = tvb_get_guint8(tvb, offset);
        offset += 1;

        *value |= (byte & 0x7F) << shift;
        shift += 7;
    } while (byte & 0x80);

    return offset - start_offset;
}
