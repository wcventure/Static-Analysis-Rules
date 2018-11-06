static gint detect_version(wtap *wth, int *err, gchar **err_info)
{
    gint                     bytes_read;
    guint16                  payload_length;
    guint16                  hdr_size;
    guint16                  read_sofar;
    guint16                  entry_len;
    gint                     version;
    struct logger_entry     *log_entry;
    struct logger_entry_v2  *log_entry_v2;
    guint8                  *buffer;
    guint16                  tmp;
    guint8                  *msg_payload, *msg_part, *msg_end;
    guint16                  msg_len;

    /
    bytes_read = file_read(&tmp, 2, wth->fh);
    if (bytes_read != 2) {
        *err = file_error(wth->fh, err_info);
        if (*err == 0 && bytes_read != 0)
            *err = WTAP_ERR_SHORT_READ;
        return -1;
    }
    payload_length = pletoh16(&tmp);

    /
    bytes_read = file_read(&tmp, 2, wth->fh);
    if (bytes_read != 2) {
        *err = file_error(wth->fh, err_info);
        if (*err == 0 && bytes_read != 0)
            *err = WTAP_ERR_SHORT_READ;
        return -1;
    }
    hdr_size = pletoh16(&tmp);
    read_sofar = 4;

    /
    if (payload_length < 3)
        return -1;
    /
    if (payload_length > LOGGER_ENTRY_MAX_PAYLOAD)
        return -1;

    /
    buffer = (guint8 *) g_malloc(sizeof(*log_entry_v2) + payload_length);
    log_entry_v2 = (struct logger_entry_v2 *) buffer;
    log_entry = (struct logger_entry *) buffer;

    /
    for (version = 1; version <= 2; ++version) {
        if (version == 1) {
            msg_payload = log_entry->msg;
            entry_len = sizeof(*log_entry) + payload_length;
        } else if (version == 2) {
            /
            msg_payload = log_entry_v2->msg;
            entry_len = sizeof(*log_entry_v2) + payload_length;
            if (hdr_size != sizeof(*log_entry_v2))
                continue;
        }

        bytes_read = file_read(buffer + read_sofar, entry_len - read_sofar,
                wth->fh);
        if (bytes_read != entry_len - read_sofar) {
            *err = file_error(wth->fh, err_info);
            if (*err == 0 && bytes_read != 0)
                *err = WTAP_ERR_SHORT_READ;
            /
            break;
        }
        read_sofar += bytes_read;

        /
        if (get_priority(msg_payload[0]) == '?')
            continue;

        /
        msg_part = (guint8 *) memchr(msg_payload, '\0', payload_length - 1);
        if (msg_part == NULL)
            continue;

        /
        ++msg_part;
        msg_len = (guint16)(payload_length - (msg_part - msg_payload));
        msg_end = (guint8 *) memchr(msg_part, '\0', msg_len);
        /
        if (msg_end && (msg_payload + payload_length - 1 != msg_end))
            continue;

        g_free(buffer);
        return version;
    }

    g_free(buffer);
    return -1;
}
