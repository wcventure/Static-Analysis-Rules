const gchar *
bytesprefix_to_str(const guint8 *prefix, const size_t prefix_len, const bytes_string *bs, const char *fmt)
{
    const gchar *ret;

    DISSECTOR_ASSERT(fmt != NULL);

    ret = try_bytesprefix_to_str(prefix, prefix_len, bs);
    if (ret != NULL)
        return ret;

    /
    return wmem_strdup(wmem_packet_scope(), fmt);
}
