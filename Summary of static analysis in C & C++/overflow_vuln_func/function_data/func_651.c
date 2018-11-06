static gint buffered_detect_version(const guint8 *pd)
{
    struct logger_entry     *log_entry;
    struct logger_entry_v2  *log_entry_v2;
    gint                     version;
    guint8                  *msg_payload = NULL;
    guint8                  *msg_part;
    guint8                  *msg_end;
    guint16                  msg_len;

    log_entry_v2 = (struct logger_entry_v2 *) pd;
    log_entry = (struct logger_entry *) pd;

    /
    if (log_entry->len < 3)
        return -1;

    /
    if (log_entry->len > LOGGER_ENTRY_MAX_PAYLOAD)
        return -1;

    /
    for (version = 1; version <= 2; ++version) {
        if (version == 1) {
            msg_payload = log_entry->msg;
        } else if (version == 2) {
            /
            msg_payload = log_entry_v2->msg;
            if (log_entry_v2->hdr_size != sizeof(*log_entry_v2))
                continue;
        }

        /
        if (get_priority(msg_payload[0]) == '?')
            continue;

        /
        msg_part = (guint8 *) memchr(msg_payload, '\0', log_entry->len - 1);
        if (msg_part == NULL)
            continue;

        /
        ++msg_part;
        msg_len = (guint16)(log_entry->len - (msg_part - msg_payload));
        msg_end = (guint8 *) memchr(msg_part, '\0', msg_len);
        /
        if (msg_end && (msg_payload + log_entry->len - 1 != msg_end))
            continue;

        return version;
    }

    return -1;
}
