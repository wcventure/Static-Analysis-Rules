gboolean catapult_dct2000_read(wtap *wth, int *err, gchar **err_info _U_,
                               long *data_offset)
{
    long offset = wth->data_offset;
    long dollar_offset, before_time_offset, after_time_offset;
    packet_direction_t direction;
    int encap;

    /
    dct2000_file_externals_t *file_externals =
        (dct2000_file_externals_t*)g_hash_table_lookup(file_externals_table, wth);

    /
    if (!file_externals)
    {
        return FALSE;
    }

    /
    while (1)
    {
        int length, seconds, useconds, data_chars;
        long this_offset = offset;

        /
        if (wth->data_offset == 0)
        {
            this_offset += (file_externals->firstline_length+1+
                            file_externals->secondline_length+1);
        }

        /
        errno = 0;

        /
        if (read_new_line(wth->fh, &offset, &length) == FALSE)
        {
            /
            break;
        }

        /
        if (parse_line(length, &seconds, &useconds,
                       &before_time_offset, &after_time_offset,
                       &dollar_offset,
                       &data_chars, &direction, &encap, FALSE))
        {
            guchar *frame_buffer;
            int n;
            int stub_offset = 0;
            line_prefix_info_t *line_prefix_info;
            char timestamp_string[32];
            sprintf(timestamp_string, "%d.%04d", seconds, useconds/100);

            /
            wth->phdr.pkt_encap = WTAP_ENCAP_CATAPULT_DCT2000;

            /
            *data_offset = this_offset;

            /
            wth->data_offset = this_offset + length + 1;

            /
            wth->phdr.ts.secs = wth->capture.catapult_dct2000->start_secs + seconds;
            if ((wth->capture.catapult_dct2000->start_usecs + useconds) >= 1000000)
            {
                wth->phdr.ts.secs++;
            }
            wth->phdr.ts.nsecs =
                ((wth->capture.catapult_dct2000->start_usecs + useconds) % 1000000) *1000;

            /
            buffer_assure_space(wth->frame_buffer,
                                strlen(context_name)+1 +  /
                                1 +                       /
                                strlen(protocol_name)+1 + /
                                1 +                       /
                                1 +                       /
                                (data_chars/2));
            frame_buffer = buffer_start_ptr(wth->frame_buffer);


            /
            /

            /
            strcpy((char*)frame_buffer, context_name);
            stub_offset += (strlen(context_name) + 1);

            /
            frame_buffer[stub_offset] = context_port;
            stub_offset++;

            /
            strcpy((char*)&frame_buffer[stub_offset], timestamp_string);
            stub_offset += (strlen(timestamp_string) + 1);

            /
            strcpy((char*)&frame_buffer[stub_offset], protocol_name);
            stub_offset += (strlen(protocol_name) + 1);

            /
            frame_buffer[stub_offset] = direction;
            stub_offset++;

            /
            frame_buffer[stub_offset] = (guint8)encap;
            stub_offset++;

            /
            wth->phdr.len = data_chars/2 + stub_offset;
            wth->phdr.caplen = data_chars/2 + stub_offset;


            /
            /
            for (n=0; n <= data_chars; n+=2)
            {
                frame_buffer[stub_offset + n/2] =
                    (hex_from_char(linebuff[dollar_offset+n]) << 4) |
                     hex_from_char(linebuff[dollar_offset+n+1]);
            }
            
            /
            line_prefix_info = g_malloc(sizeof(line_prefix_info_t));

            line_prefix_info->before_time = g_malloc(before_time_offset+1);
            strncpy(line_prefix_info->before_time, linebuff, before_time_offset);
            line_prefix_info->before_time[before_time_offset] = '\0';

            line_prefix_info->after_time = g_malloc(dollar_offset - after_time_offset);
            strncpy(line_prefix_info->after_time, linebuff+after_time_offset,
                   dollar_offset - after_time_offset);
            line_prefix_info->after_time[dollar_offset - after_time_offset-1] = '\0';

            /
            g_hash_table_insert(file_externals->line_header_prefixes_table,
                                (void*)this_offset, line_prefix_info);


            /
            set_pseudo_header_info(wth, encap, this_offset, &wth->pseudo_header,
                                   data_chars/2, direction);

            /
            *err = errno;
            return TRUE;
        }
    }

    /
    *err = errno;
    return FALSE;
}
