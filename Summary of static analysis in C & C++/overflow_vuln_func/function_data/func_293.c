const gchar *
try_bytesprefix_to_str(const guint8 *prefix, const size_t prefix_len, const bytes_string *bs)
{
    guint i = 0;

    if (bs) {
        while (bs[i].strptr) {
            if (prefix_len >= bs[i].value_length && !memcmp(bs[i].value, prefix, prefix_len)) {
                return bs[i].strptr;
            }
            i++;
        }
    }

    return NULL;
}
