static gboolean
sync_pipe_input_cb(gint source, gpointer user_data)
{
    capture_session *cap_session = (capture_session *)user_data;
    int  ret;
    char buffer[SP_MAX_MSG_LEN+1];
    ssize_t nread;
    char indicator;
    int  primary_len;
    char *primary_msg;
    int  secondary_len;
    char *secondary_msg;
    char *wait_msg, *combined_msg;
    int npackets;

    nread = pipe_read_block(source, &indicator, SP_MAX_MSG_LEN, buffer,
                            &primary_msg);
    if(nread <= 0) {
        /
        ret = sync_pipe_wait_for_child(cap_session->fork_child, &wait_msg);
        if(nread == 0) {
            /
            if (ret == -1)
                primary_msg = wait_msg;
        } else {
            /
            if (ret == -1) {
                combined_msg = g_strdup_printf("%s\n\n%s", primary_msg, wait_msg);
                g_free(primary_msg);
                g_free(wait_msg);
                primary_msg = combined_msg;
            }
        }

        /
        cap_session->fork_child = -1;
        cap_session->fork_child_status = ret;

#ifdef _WIN32
        ws_close(cap_session->signal_pipe_write_fd);
#endif
        capture_input_closed(cap_session, primary_msg);
        g_free(primary_msg);
        return FALSE;
    }

    /
    switch(indicator) {
    case SP_FILE:
        if(!capture_input_new_file(cap_session, buffer)) {
            g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG, "sync_pipe_input_cb: file failed, closing capture");

            /
            ws_close(source);

            /
            sync_pipe_stop(cap_session);
            capture_input_closed(cap_session, NULL);
            return FALSE;
        }
        break;
    case SP_PACKET_COUNT:
        npackets = atoi(buffer);
        g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG, "sync_pipe_input_cb: new packets %u", npackets);
        capture_input_new_packets(cap_session, npackets);
        break;
    case SP_ERROR_MSG:
        /
        pipe_convert_header((guchar*)buffer, 4, &indicator, &primary_len);
        primary_msg = buffer+4;
        /
        pipe_convert_header((guchar*)primary_msg + primary_len, 4, &indicator, &secondary_len);
        secondary_msg = primary_msg + primary_len + 4;
        /
        capture_input_error_message(cap_session, primary_msg, secondary_msg);
        /
        /
        break;
    case SP_BAD_FILTER: {
        char *ch;
        int indx;

        ch = strtok(buffer, ":");
        indx = (int)strtol(ch, NULL, 10);
        ch = strtok(NULL, ":");
        capture_input_cfilter_error_message(cap_session, indx, ch);
         /
         break;
        }
    case SP_DROPS:
        capture_input_drops(cap_session, (guint32)strtoul(buffer, NULL, 10));
        break;
    default:
        g_assert_not_reached();
    }

    return TRUE;
}
