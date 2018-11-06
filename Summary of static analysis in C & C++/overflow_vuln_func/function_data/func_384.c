static guint get_write_list_size(tvbuff_t *tvb, guint max_offset, guint offset)
{
    guint32 value_follows;
    guint start = offset;

    while (1) {
        value_follows = tvb_get_ntohl(tvb, offset);
        offset += 4;
        if (offset > max_offset)
            return 0;
        if (!value_follows)
            break;

        offset += get_write_chunk_size(tvb, offset);
        if (offset > max_offset)
            return 0;
    }

    return offset - start;
}
