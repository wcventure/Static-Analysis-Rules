static int capture_android_wifi_tcpdump(char *interface, char *fifo,
        const char *adb_server_ip, unsigned short *adb_server_tcp_port) {
    struct extcap_dumper                     extcap_dumper;
    static char                              data[PACKET_LENGTH];
    static char                              helpful_packet[PACKET_LENGTH];
    gssize                                   length;
    gssize                                   used_buffer_length =  0;
    gssize                                   filter_buffer_length = 0;
    gssize                                   frame_length=0;
    socket_handle_t                          sock;
    const char                               *adb_transport = "0012" "host:transport-any";
    const char                               *adb_transport_serial_templace = "%04x" "host:transport:%s";
    const char                               *adb_shell_tcpdump = "001D" "shell:tcpdump -n -s 0 -u -w -";
    gint                                     result;
    char                                     *serial_number = NULL;
    char                                     filter_buffer[PACKET_LENGTH];
    gint                                     device_endiness = G_LITTLE_ENDIAN;
    gboolean                                 global_header_skipped=FALSE;
    pcaprec_hdr_t                            p_header;

    /
    sock = adb_connect(adb_server_ip, adb_server_tcp_port);
    if (sock == INVALID_SOCKET)
        return -1;

    if (is_specified_interface(interface, INTERFACE_ANDROID_WIFI_TCPDUMP)
            && strlen(interface) > strlen(INTERFACE_ANDROID_WIFI_TCPDUMP) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_WIFI_TCPDUMP) + 1;
    }
    if (!serial_number) {
        result = adb_send(sock, adb_transport);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport");
            fflush(stdout);

            errmsg_print("ERROR: Error while setting adb transport for <%s>",
                adb_transport);
            closesocket(sock);
            return 1;
        }
    } else {
        sprintf((char *) helpful_packet, adb_transport_serial_templace,
            15 + strlen(serial_number), serial_number);
        result = adb_send(sock, helpful_packet);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport");
            closesocket(sock);
            return 1;
        }
    }
    result = adb_send(sock, adb_shell_tcpdump);
    if (result) {
        errmsg_print("ERROR: Error while setting adb transport");
        closesocket(sock);
        return 1;
    }

    extcap_dumper = extcap_dumper_open(fifo, EXTCAP_ENCAP_ETHERNET);
    while (endless_loop) {
        char *i_position;
        errno = 0;
        length = recv(sock, data + used_buffer_length, (int)(PACKET_LENGTH - used_buffer_length), 0);
        if (errno == EAGAIN || (EWOULDBLOCK != EAGAIN && errno == EWOULDBLOCK))
            continue;
        else if (errno != 0) {
            errmsg_print("ERROR capture: %s", strerror(errno));
            return 100;
        }

        if (length <= 0) {
            errmsg_print("ERROR: Broken socket connection.");
            closesocket(sock);
            return 101;
        }

        used_buffer_length += length;

        /
        if (used_buffer_length > 4) {
            guint * magic_number;
            magic_number= (guint *)data;
            if (*magic_number == 0xd4c3b2a1 || *magic_number == 0xa1b2c3d4) {
                if (data[0] == (char)0xd4){
                    device_endiness = G_LITTLE_ENDIAN;
                }
                else {
                    device_endiness = G_BIG_ENDIAN;
                }
                break;
            }
        }

        i_position = (char *) memchr(data, '\n', used_buffer_length);
        if (i_position && i_position < data + used_buffer_length) {
            memmove(data, i_position + 1 , used_buffer_length - (i_position + 1 - data));
            used_buffer_length = used_buffer_length - (gssize) (i_position + 1 - data);
        }
    }
    /
    filter_buffer_length=0;
    while (endless_loop) {
        gssize i = 0,read_offset,j=0;
       /
        for (i = 0; i < (used_buffer_length - 1); i++) {
            if (data[i] == 0x0d && data[i + 1] == 0x0a) {
                i++;
            }
            filter_buffer[filter_buffer_length++] = data[i];
        }

        /
        for (j=0; i < used_buffer_length; i++,j++) {
            data[j] = data[i];
        }
        used_buffer_length = j;
        if (global_header_skipped==FALSE && filter_buffer_length >= PCAP_GLOBAL_HEADER_LENGTH) {
            /
            filter_buffer_length -= PCAP_GLOBAL_HEADER_LENGTH;

            /
            memmove(filter_buffer , filter_buffer + PCAP_GLOBAL_HEADER_LENGTH , filter_buffer_length);
            global_header_skipped = TRUE;
        }
        else if (global_header_skipped && filter_buffer_length > PCAP_RECORD_HEADER_LENGTH) {
            read_offset=0;
            while (filter_buffer_length > PCAP_RECORD_HEADER_LENGTH) {
                gchar *packet;
                packet = filter_buffer + read_offset;
                 /
                if (device_endiness == G_BYTE_ORDER) {
                    p_header = *((pcaprec_hdr_t*)packet);
                }
                else {
                    p_header.ts_sec = GUINT32_SWAP_LE_BE(*((guint32*)packet));
                    p_header.ts_usec = GUINT32_SWAP_LE_BE(*(guint32*)(packet +4));
                    p_header.incl_len = GUINT32_SWAP_LE_BE(*(guint32*)(packet +8));
                    p_header.orig_len = GUINT32_SWAP_LE_BE(*(guint32*)(packet +12));
                }

                if ((gssize)(p_header.incl_len + PCAP_RECORD_HEADER_LENGTH) <= filter_buffer_length) {

                    /
                    if (p_header.incl_len > 0) {
                        endless_loop = extcap_dumper_dump(extcap_dumper , filter_buffer + read_offset+ PCAP_RECORD_HEADER_LENGTH,
                        p_header.incl_len , p_header.orig_len , p_header.ts_sec , p_header.ts_usec);
                    }
                    frame_length = p_header.incl_len + PCAP_RECORD_HEADER_LENGTH;

                    /
                    filter_buffer_length -= frame_length;
                    read_offset += frame_length;
                }
                else {
                    /
                    break;
                }
            }
            if (read_offset!=0) {
                /
                memmove(filter_buffer, filter_buffer + read_offset , filter_buffer_length);
            }
        }

        /
        while (endless_loop) {
            errno = 0;
            length = recv(sock, data + used_buffer_length, (int)(PACKET_LENGTH -(used_buffer_length + filter_buffer_length)), 0);
            if (errno == EAGAIN || (EWOULDBLOCK != EAGAIN && errno == EWOULDBLOCK)) {
                continue;
            }
            else if (errno != 0) {
                errmsg_print("ERROR capture: %s", strerror(errno));
                return 100;
            }

            if (length <= 0) {
                errmsg_print("ERROR: Broken socket connection.");
                closesocket(sock);
                return 101;
            }

            if (used_buffer_length += length > 1) {
                break;
            }
        }
    }

    closesocket(sock);
    return 0;
}
