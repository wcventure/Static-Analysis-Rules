static void
decode_zcl_msg_start_time(gchar *s, guint32 value)
{
    if (value == ZBEE_ZCL_MSG_START_TIME_NOW)
        g_snprintf(s, ITEM_LABEL_LENGTH, "Now");
    else {
        gchar *start_time;
        value += ZBEE_ZCL_NSTIME_UTC_OFFSET;
        start_time = abs_time_secs_to_str (NULL, value, ABSOLUTE_TIME_LOCAL, TRUE);
        g_snprintf(s, ITEM_LABEL_LENGTH, "%s", start_time);
        wmem_free(NULL, start_time);
    }
} /
