gboolean parse_line(gint length, gint *seconds, gint *useconds,
                    long *before_time_offset, long *after_time_offset,
                    long *data_offset, gint *data_chars,
                    packet_direction_t *direction,
                    int *encap,
                    gboolean seek_read)
{
    int  n = 0;
    int  port_digits = 0;
    char port_number_string[MAX_PORT_DIGITS+1];
    int  protocol_chars = 0;

    char seconds_buff[MAX_SECONDS_CHARS+1];
    int  seconds_chars;
    char subsecond_decimals_buff[MAX_SUBSECOND_DECIMALS+1];
    int  subsecond_decimals_chars;

    gboolean atm_header_present = FALSE;

    /
    for (n=0; linebuff[n] != '.' && (n < MAX_CONTEXT_NAME); n++)
    {
        if (!isalnum(linebuff[n]) && (linebuff[n] != '_'))
        {
            return FALSE;
        }
        context_name[n] = linebuff[n];
    }

    /
    if (linebuff[n] != '.')
    {
        return FALSE;
    }
    context_name[n] = '\0';
    /
    n++;


    /
    for (port_digits = 0;
         (linebuff[n] != '/') && (port_digits <= MAX_PORT_DIGITS);
         n++, port_digits++)
    {
        if (!isdigit(linebuff[n]))
        {
            return FALSE;
        }
        port_number_string[port_digits] = linebuff[n];
    }

    /
    if (linebuff[n] != '/')
    {
        return FALSE;
    }
    port_number_string[port_digits] = '\0';
    context_port = atoi(port_number_string);
    /
    n++;


    /
    for (protocol_chars = 0;
         (linebuff[n] != '/') && (protocol_chars < MAX_PROTOCOL_NAME) &&
         (n < MAX_LINE_LENGTH);
         n++, protocol_chars++)
    {
        if (!isalnum(linebuff[n]) && linebuff[n] != '_')
        {
            return FALSE;
        }
        protocol_name[protocol_chars] = linebuff[n];
    }
    if (protocol_chars == MAX_PROTOCOL_NAME)
    {
        /
        return FALSE;
    }
    protocol_name[protocol_chars] = '\0';

    /
    if (linebuff[n] != '/')
    {
        return FALSE;
    }


    /
    /

    if ((strcmp(protocol_name, "ip") == 0)  || (strcmp(protocol_name, "sctp") == 0))
    {
        *encap = WTAP_ENCAP_RAW_IP;
    }
    else

    /
    if ((strcmp(protocol_name, "fp") == 0) ||
        (strcmp(protocol_name, "fp_r4") == 0) ||
        (strcmp(protocol_name, "fp_r5") == 0) ||
        (strcmp(protocol_name, "fp_r6") == 0))
    {
        *encap = WTAP_ENCAP_ATM_PDUS_UNTRUNCATED;
        atm_header_present = TRUE;
    }

    else
    if (strcmp(protocol_name, "ppp") == 0)
    {
        *encap = WTAP_ENCAP_PPP;
    }
    else
    if (strcmp(protocol_name, "isdn_l3") == 0)
    {
        /
        *encap = WTAP_ENCAP_ISDN;
    }
    else
    if (strcmp(protocol_name, "ethernet") == 0)
    {
        *encap = WTAP_ENCAP_ETHERNET;
    }
    else
    if ((strcmp(protocol_name, "saalnni_sscop") == 0) ||
        (strcmp(protocol_name, "saaluni_sscop") == 0))
    {
        *encap = DCT2000_ENCAP_SSCOP;
    }
    else
    if (strcmp(protocol_name, "frelay_l2") == 0)
    {
        *encap = WTAP_ENCAP_FRELAY;
    }
    else
    if (strcmp(protocol_name, "ss7_mtp2") == 0)
    {
        *encap = DCT2000_ENCAP_MTP2;
    }
    else
    {
        /
        if (catapult_dct2000_board_ports_only && !seek_read)
        {
            return FALSE;
        }
        else
        {
            /
            *encap = DCT2000_ENCAP_UNHANDLED;
        }
    }


    /
    if (atm_header_present)
    {
        int header_chars_seen = 0;

        /
        for (; (linebuff[n] != '$') && (n < MAX_LINE_LENGTH); n++);
        /
        n++;

        /
        for (;
             (isalnum(linebuff[n]) &&
              (n < MAX_LINE_LENGTH) &&
              (header_chars_seen < AAL_HEADER_CHARS));
             n++, header_chars_seen++)
        {
            aal_header_chars[header_chars_seen] = linebuff[n];
        }

        if (header_chars_seen != AAL_HEADER_CHARS)
        {
            return FALSE;
        }
    }


    /
    for (; (linebuff[n] != ' ') && (n < MAX_LINE_LENGTH); n++);
    /
    n++;

    /
    if (linebuff[n] == 's')
    {
        *direction = sent;
    }
    else
    if (linebuff[n] == 'r')
    {
        *direction = received;
    }
    else
    {
        return FALSE;
    }


    /
    /

    /
    for (; !isdigit(linebuff[n]) && (n < MAX_LINE_LENGTH); n++);

    *before_time_offset = n;

    /
    for (seconds_chars = 0;
         (linebuff[n] != '.') &&
         (seconds_chars <= MAX_SECONDS_CHARS) &&
         (n < MAX_LINE_LENGTH);
         n++, seconds_chars++)
    {
        if (!isdigit(linebuff[n]))
        {
            /
            return FALSE;
        }
        seconds_buff[seconds_chars] = linebuff[n];
    }
    if (seconds_chars > MAX_SECONDS_CHARS)
    {
        /
        return FALSE;
    }
    
    /
    seconds_buff[seconds_chars] = '\0';
    *seconds = atoi(seconds_buff);

    /
    if (linebuff[n] != '.')
    {
        return FALSE;
    }
    /
    n++;

    /
    for (subsecond_decimals_chars = 0;
         (linebuff[n] != ' ') &&
         (subsecond_decimals_chars <= MAX_SUBSECOND_DECIMALS) &&
         (n < MAX_LINE_LENGTH);
         n++, subsecond_decimals_chars++)
    {
        if (!isdigit(linebuff[n]))
        {
            return FALSE;
        }
        subsecond_decimals_buff[subsecond_decimals_chars] = linebuff[n];
    }
    if (subsecond_decimals_chars > MAX_SUBSECOND_DECIMALS)
    {
        /
        return FALSE;
    }
    /
    subsecond_decimals_buff[subsecond_decimals_chars] = '\0';
    *useconds = atoi(subsecond_decimals_buff) * 100;

    /
    if (linebuff[n] != ' ')
    {
        return FALSE;
    }

    *after_time_offset = n;

    /
    for (; (linebuff[n] != '$') && (n < MAX_LINE_LENGTH); n++);
    /
    n++;

    /
    *data_offset = n;

    /
    *data_chars = length - n;

    /
    if (*encap == WTAP_ENCAP_ISDN)
    {
        *data_offset += 2;
        *data_chars -= 2;
    }

    return TRUE;
}
