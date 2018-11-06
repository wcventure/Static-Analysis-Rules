static ssize_t
pipe_read_block(int pipe_fd, char *indicator, int len, char *msg,
                char **err_msg)
{
    int required;
    ssize_t newly;
    gchar header[4];

    /
    newly = pipe_read_bytes(pipe_fd, header, 4, err_msg);
    if(newly != 4) {
        if (newly == 0) {
            /
            g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG,
                  "read %d got an EOF", pipe_fd);
            return 0;
        }
        g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG,
              "read %d failed to read header: %lu", pipe_fd, (long)newly);
        if (newly != -1) {
            /
            *err_msg = g_strdup_printf("Premature EOF reading from sync pipe: got only %ld bytes",
                                       (long)newly);
        }
        return -1;
    }

    /
    pipe_convert_header((guchar*)header, 4, indicator, &required);

    /
    if(required == 0) {
        g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG,
              "read %d indicator: %c empty value", pipe_fd, *indicator);
        return 4;
    }

    /
    if(required > len) {
        g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG,
              "read %d length error, required %d > len %d, header: 0x%02x 0x%02x 0x%02x 0x%02x",
              pipe_fd, required, len,
              header[0], header[1], header[2], header[3]);

        /
        memcpy(msg, header, sizeof(header));
        newly = read(pipe_fd, &msg[sizeof(header)], len-sizeof(header));
	if (newly < 0) { /
	    g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG,
		  "read from pipe %d: error(%u): %s", pipe_fd, errno, g_strerror(errno));
	}
        *err_msg = g_strdup_printf("Unknown message from dumpcap, try to show it as a string: %s",
                                   msg);
        return -1;
    }
    len = required;

    /
    newly = pipe_read_bytes(pipe_fd, msg, required, err_msg);
    if(newly != required) {
        if (newly != -1) {
            *err_msg = g_strdup_printf("Unknown message from dumpcap, try to show it as a string: %s",
                                       msg);
        }
        return -1;
    }

    /
    g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG,
          "read %d ok indicator: %c len: %u msg: %s", pipe_fd, *indicator,
          len, msg);
    *err_msg = NULL;
    return newly + 4;
}
