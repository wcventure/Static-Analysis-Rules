static gboolean
parse_line(gchar *linebuff, gint line_length,
           gint *seconds, gint *useconds,
           long *before_time_offset, long *after_time_offset,
           long *data_offset, gint *data_chars,
           packet_direction_t *direction,
           int *encap, int *is_comment, int *is_sprint,
           gchar *aal_header_chars,
           gchar *context_name, guint8 *context_portp,
           gchar *protocol_name, gchar *variant_name,
           gchar *outhdr_name)
{
    int  n = 0;
    int  port_digits;
    char port_number_string[MAX_PORT_DIGITS+1];
    int  variant_digits = 0;
    int  variant = 1;
    int  protocol_chars = 0;
    int  outhdr_chars = 0;

    char seconds_buff[MAX_SECONDS_CHARS+1];
    int  seconds_chars;
    char subsecond_decimals_buff[MAX_SUBSECOND_DECIMALS+1];
    int  subsecond_decimals_chars;
    int  skip_first_byte = FALSE;
    gboolean atm_header_present = FALSE;

    *is_comment = FALSE;
    *is_sprint = FALSE;

    /
    for (n=0; (linebuff[n] != '.') && (n < MAX_CONTEXT_NAME) && (n+1 < line_length); n++) {
        if (linebuff[n] == '/') {
            context_name[n] = '\0';

            /
            if (strncmp(linebuff+n, "/////", 5) != 0) {
                return FALSE;
            }

            /
            g_strlcpy(protocol_name, "comment", MAX_PROTOCOL_NAME);
            *is_comment = TRUE;
            break;
        }
        if (!isalnum((guchar)linebuff[n]) && (linebuff[n] != '_') && (linebuff[n] != '-')) {
            return FALSE;
        }
        context_name[n] = linebuff[n];
    }
    if (n == MAX_CONTEXT_NAME || (n+1 >= line_length)) {
        return FALSE;
    }

    /
    variant_name[0] = '\0';
    outhdr_name[0] = '\0';
    port_number_string[0] = '\0';

    if (!(*is_comment)) {
        /
        if (linebuff[n] != '.') {
            return FALSE;
        }
        context_name[n] = '\0';
        /
        n++;

        /
        for (port_digits = 0;
             (linebuff[n] != '/') && (port_digits <= MAX_PORT_DIGITS) && (n+1 < line_length);
             n++, port_digits++) {

            if (!isdigit((guchar)linebuff[n])) {
                return FALSE;
            }
            port_number_string[port_digits] = linebuff[n];
        }
        if (port_digits > MAX_PORT_DIGITS || (n+1 >= line_length)) {
            return FALSE;
        }

        /
        if (linebuff[n] != '/')
        {
            return FALSE;
        }
        port_number_string[port_digits] = '\0';
        if (port_digits == 1) {
            *context_portp = port_number_string[0] - '0';
        }
        else {
            *context_portp = atoi(port_number_string);
        }
        /
        n++;

        /
        for (protocol_chars = 0;
             (linebuff[n] != '/') && (protocol_chars < MAX_PROTOCOL_NAME) && (n < line_length);
             n++, protocol_chars++) {

            if (!isalnum((guchar)linebuff[n]) && linebuff[n] != '_') {
                return FALSE;
            }
            protocol_name[protocol_chars] = linebuff[n];
        }
        if (protocol_chars == MAX_PROTOCOL_NAME || n >= line_length) {
            /
            return FALSE;
        }
        protocol_name[protocol_chars] = '\0';

        /
        if (linebuff[n] != '/') {
            return FALSE;
        }
        /
        n++;


        /
        for (variant_digits = 0;
             (isdigit((guchar)linebuff[n])) && (variant_digits <= MAX_VARIANT_DIGITS) && (n+1 < line_length);
             n++, variant_digits++) {

            if (!isdigit((guchar)linebuff[n])) {
                return FALSE;
            }
            variant_name[variant_digits] = linebuff[n];
        }
        if (variant_digits > MAX_VARIANT_DIGITS || (n+1 >= line_length)) {
            return FALSE;
        }

        if (variant_digits > 0) {
            variant_name[variant_digits] = '\0';
            if (variant_digits == 1) {
                variant = variant_name[0] - '0';
            }
            else {
                variant = atoi(variant_name);
            }
        }
        else {
            variant_name[0] = '1';
            variant_name[1] = '\0';
        }


        /
        outhdr_name[0] = '\0';
        if (linebuff[n] == ',') {
            /
            n++;

            for (outhdr_chars = 0;
                 (isdigit((guchar)linebuff[n]) || linebuff[n] == ',') &&
                 (outhdr_chars <= MAX_OUTHDR_NAME) && (n+1 < line_length);
                 n++, outhdr_chars++) {

                if (!isdigit((guchar)linebuff[n]) && (linebuff[n] != ',')) {
                    return FALSE;
                }
                outhdr_name[outhdr_chars] = linebuff[n];
            }
            if (outhdr_chars > MAX_OUTHDR_NAME || (n+1 >= line_length)) {
                return FALSE;
            }
            /
            outhdr_name[outhdr_chars] = '\0';
        }
    }


    /
    /

    if ((strcmp(protocol_name, "ip") == 0) ||
        (strcmp(protocol_name, "sctp") == 0) ||
        (strcmp(protocol_name, "gre") == 0) ||
        (strcmp(protocol_name, "mipv6") == 0) ||
        (strcmp(protocol_name, "igmp") == 0)) {

        *encap = WTAP_ENCAP_RAW_IP;
    }
    else

    /
    if ((strcmp(protocol_name, "fp") == 0) ||
        (strncmp(protocol_name, "fp_r", 4) == 0)) {

        if ((variant > 256) && (variant % 256 == 3)) {
            /
            *encap = 0;
        }
        else {
            /
            *encap = WTAP_ENCAP_ATM_PDUS_UNTRUNCATED;
            atm_header_present = TRUE;
        }
    }
    else if (strcmp(protocol_name, "fpiur_r5") == 0) {
        /
        *encap = WTAP_ENCAP_ATM_PDUS_UNTRUNCATED;
        atm_header_present = TRUE;
    }

    else
    if (strcmp(protocol_name, "ppp") == 0) {
        *encap = WTAP_ENCAP_PPP;
    }
    else
    if (strcmp(protocol_name, "isdn_l3") == 0) {
       /
        skip_first_byte = TRUE;
        *encap = WTAP_ENCAP_ISDN;
    }
    else
    if (strcmp(protocol_name, "isdn_l2") == 0) {
        *encap = WTAP_ENCAP_ISDN;
    }
    else
    if (strcmp(protocol_name, "ethernet") == 0) {
        *encap = WTAP_ENCAP_ETHERNET;
    }
    else
    if ((strcmp(protocol_name, "saalnni_sscop") == 0) ||
        (strcmp(protocol_name, "saaluni_sscop") == 0)) {

        *encap = DCT2000_ENCAP_SSCOP;
    }
    else
    if (strcmp(protocol_name, "frelay_l2") == 0) {
        *encap = WTAP_ENCAP_FRELAY;
    }
    else
    if (strcmp(protocol_name, "ss7_mtp2") == 0) {
        *encap = DCT2000_ENCAP_MTP2;
    }
    else
    if ((strcmp(protocol_name, "nbap") == 0) ||
        (strcmp(protocol_name, "nbap_r4") == 0) ||
        (strncmp(protocol_name, "nbap_sscfuni", strlen("nbap_sscfuni")) == 0)) {

        /
        *encap = DCT2000_ENCAP_NBAP;
    }
    else {
        /
        *encap = DCT2000_ENCAP_UNHANDLED;
    }


    /
    if (atm_header_present) {
        int header_chars_seen = 0;

        /
        for (; (linebuff[n] != '$') && (n+1 < line_length); n++);
        /
        n++;
        if (n+1 >= line_length) {
            return FALSE;
        }

        /
        for (;
             ((linebuff[n] >= '0') && (linebuff[n] <= '?') &&
              (n < line_length) &&
              (header_chars_seen < AAL_HEADER_CHARS));
             n++, header_chars_seen++) {

            aal_header_chars[header_chars_seen] = linebuff[n];
            /
            if (!isdigit((guchar)linebuff[n])) {
                aal_header_chars[header_chars_seen] = 'a' + (linebuff[n] - '9') -1;
            }
        }

        if (header_chars_seen != AAL_HEADER_CHARS || n >= line_length) {
            return FALSE;
        }
    }

    /
    n++;

    /
    if (isdigit(linebuff[n])) {
        while ((n+1 < line_length) && linebuff[n] != '/') {
            n++;
        }
    }

    /
    while ((n+1 < line_length) && linebuff[n] == '/') {
        n++;
    }

    /
    if ((n+1 < line_length) && linebuff[n] == ' ') {
        n++;
    }

    /
    if (!(*is_comment)) {
        if (linebuff[n] == 's') {
            *direction = sent;
        }
        else
        if (linebuff[n] == 'r') {
            *direction = received;
        }
        else {
            return FALSE;
        }
        /
        n++;
    }
    else {
        *direction = sent;
    }


    /
    /

    /
    /

    for (; ((linebuff[n] != 't') || (linebuff[n+1] != 'm')) && (n+1 < line_length); n++);
    if (n >= line_length) {
        return FALSE;
    }

    for (; !isdigit((guchar)linebuff[n]) && (n < line_length); n++);
    if (n >= line_length) {
        return FALSE;
    }

    *before_time_offset = n;

    /
    for (seconds_chars = 0;
         (linebuff[n] != '.') &&
         (seconds_chars <= MAX_SECONDS_CHARS) &&
         (n < line_length);
         n++, seconds_chars++) {

        if (!isdigit((guchar)linebuff[n])) {
            /
            return FALSE;
        }
        seconds_buff[seconds_chars] = linebuff[n];
    }
    if (seconds_chars > MAX_SECONDS_CHARS || n >= line_length) {
        /
        return FALSE;
    }

    /
    seconds_buff[seconds_chars] = '\0';
    *seconds = atoi(seconds_buff);

    /
    if (linebuff[n] != '.') {
        return FALSE;
    }
    /
    n++;

    /
    for (subsecond_decimals_chars = 0;
         (linebuff[n] != ' ') &&
         (subsecond_decimals_chars <= MAX_SUBSECOND_DECIMALS) &&
         (n < line_length);
         n++, subsecond_decimals_chars++) {

        if (!isdigit((guchar)linebuff[n])) {
            return FALSE;
        }
        subsecond_decimals_buff[subsecond_decimals_chars] = linebuff[n];
    }
    if (subsecond_decimals_chars > MAX_SUBSECOND_DECIMALS || n >= line_length) {
        /
        return FALSE;
    }
    /
    subsecond_decimals_buff[subsecond_decimals_chars] = '\0';
    *useconds = atoi(subsecond_decimals_buff) * 100;

    /
    if (linebuff[n] != ' ') {
        return FALSE;
    }

    *after_time_offset = n++;

    /
    if (*is_comment) {
        if (strncmp(linebuff+n, "l $", 3) != 0) {
            *is_sprint = TRUE;
            g_strlcpy(protocol_name, "sprint", MAX_PROTOCOL_NAME);
        }
    }

    if (!(*is_sprint)) {
        /
        for (; (linebuff[n] != '$') && (linebuff[n] != '\'') && (n+1 < line_length); n++);
        if ((linebuff[n] == '\'') || (n+1 >= line_length)) {
            return FALSE;
        }
        /
        n++;
    }

    /
    *data_offset = n;

    /
    *data_chars = line_length - n;

    /
    if (skip_first_byte) {
        *data_offset += 2;
        *data_chars -= 2;
    }

    return TRUE;
}
