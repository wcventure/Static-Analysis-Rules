static int capture_android_bluetooth_hcidump(char *interface, char *fifo,
        const char *adb_server_ip, unsigned short *adb_server_tcp_port) {
    struct extcap_dumper           extcap_dumper;
    static char                    data[PACKET_LENGTH];
    static char                    packet[PACKET_LENGTH];
    static char                    helpful_packet[PACKET_LENGTH];
    gssize                         length;
    gssize                         used_buffer_length = 0;
    socket_handle_t                sock;
    const char                    *adb_transport  = "0012""host:transport-any";
    const char                    *adb_transport_serial_templace = "%04x""host:transport:%s";
    const char                    *adb_shell_hcidump = "0013""shell:hcidump -R -t";
    const char                    *adb_shell_su_hcidump = "0019""shell:su -c hcidump -R -t";
    int                            result;
    char                          *serial_number = NULL;
    size_t                         serial_number_length = 0;
    time_t                         ts = 0;
    unsigned int                   captured_length;
    gint64                         hex;
    char                          *hex_data;
    char                          *new_hex_data;
    own_pcap_bluetooth_h4_header  *h4_header;
    gint64                         raw_length = 0;
    gint64                         frame_length;
    int                            ms = 0;
    struct tm                      date;
    char                           direction_character;
    int                            try_next = 0;

    SET_DATA(h4_header, value_own_pcap_bluetooth_h4_header, packet);

    extcap_dumper = extcap_dumper_open(fifo, EXTCAP_ENCAP_BLUETOOTH_H4_WITH_PHDR);

    sock = adb_connect(adb_server_ip, adb_server_tcp_port);
    if (sock == INVALID_SOCKET)
        return -1;

    if (is_specified_interface(interface, INTERFACE_ANDROID_BLUETOOTH_HCIDUMP) &&
            strlen(interface) > strlen(INTERFACE_ANDROID_BLUETOOTH_HCIDUMP) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_BLUETOOTH_HCIDUMP) + 1;
        serial_number_length = strlen(serial_number);
    }

    if (!serial_number) {
        result = adb_send(sock, adb_transport);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport for <%s>", adb_transport);
            closesocket(sock);
            return 1;
        }
    } else {
        sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
        result = adb_send(sock, helpful_packet);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport for <%s>", helpful_packet);
            closesocket(sock);
            return 1;
        }
    }

    result = adb_send(sock, adb_shell_hcidump);
    if (result) {
        errmsg_print("ERROR: Error while starting capture by sending command: %s", adb_shell_hcidump);
        closesocket(sock);
        return 1;
    }

    while (endless_loop) {
        char  *i_position;

        errno = 0;
        length = recv(sock, data + used_buffer_length, (int)(PACKET_LENGTH - used_buffer_length), 0);
        if (errno == EAGAIN || (EWOULDBLOCK != EAGAIN && errno == EWOULDBLOCK)) continue;
        else if (errno != 0) {
            errmsg_print("ERROR capture: %s", strerror(errno));
            closesocket(sock);
            return 100;
        }

        if (length <= 0) {
            errmsg_print("ERROR: Broken socket connection.");
            closesocket(sock);
            return 101;
        }

        used_buffer_length += length;
        i_position =  (char *) memchr(data, '\n', used_buffer_length);
        if (i_position && i_position < data + used_buffer_length) {
            char *state_line_position = i_position + 1;

            if (!strncmp(data, "/system/bin/sh: hcidump: not found", 34)) {
                errmsg_print("ERROR: Command not found for <%s>", adb_shell_hcidump);
                closesocket(sock);
                return 2;
            }

            i_position =  (char *) memchr(i_position + 1, '\n', used_buffer_length);
            if (i_position) {
                i_position += 1;
                if (!strncmp(state_line_position, "Can't access device: Permission denied", 38)) {
                    verbose_print("WARNING: No permission for command <%s>\n", adb_shell_hcidump);
                    used_buffer_length = 0;
                    try_next += 1;
                    break;
                }
                memmove(data, i_position, used_buffer_length - (i_position - data));
                used_buffer_length = used_buffer_length - (gssize)(i_position - data);
                break;
            }
        }
    }

    if (try_next == 1) {
        sock = adb_connect(adb_server_ip, adb_server_tcp_port);
        if (sock == INVALID_SOCKET)
            return -1;

        sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
        result = adb_send(sock, helpful_packet);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport for <%s>", helpful_packet);
            closesocket(sock);
            return 1;
        }

        result = adb_send(sock, adb_shell_su_hcidump);
        if (result) {
            errmsg_print("ERROR: Error while starting capture by sending command: <%s>", adb_shell_su_hcidump);
            closesocket(sock);
            return 1;
        }

        used_buffer_length = 0;
        while (endless_loop) {
            char  *i_position;

            errno = 0;
            length = recv(sock, data + used_buffer_length, (int)(PACKET_LENGTH - used_buffer_length), 0);
            if (errno == EAGAIN || (EWOULDBLOCK != EAGAIN && errno == EWOULDBLOCK)) continue;
            else if (errno != 0) {
                errmsg_print("ERROR capture: %s", strerror(errno));
                closesocket(sock);
                return 100;
            }

            if (length <= 0) {
                errmsg_print("ERROR: Broken socket connection.");
                closesocket(sock);
                return 101;
            }

            used_buffer_length += length;
            i_position =  (char *) memchr(data, '\n', used_buffer_length);
            if (i_position && i_position < data + used_buffer_length) {
                if (!strncmp(data, "/system/bin/sh: su: not found", 29)) {
                    errmsg_print("ERROR: Command 'su' not found for <%s>", adb_shell_su_hcidump);
                    closesocket(sock);
                    return 2;
                }

                i_position =  (char *) memchr(i_position + 1, '\n', used_buffer_length);
                if (i_position) {
                    i_position += 1;
                    memmove(data, i_position, used_buffer_length - (i_position - data));
                    used_buffer_length = used_buffer_length - (gssize)(i_position - data);
                    break;
                }
            }
        }
    }

    while (endless_loop) {
        errno = 0;
        length = recv(sock, data + used_buffer_length,  (int)(PACKET_LENGTH - used_buffer_length), 0);
        if (errno == EAGAIN || (EWOULDBLOCK != EAGAIN && errno == EWOULDBLOCK)) continue;
        else if (errno != 0) {
            errmsg_print("ERROR capture: %s", strerror(errno));
            closesocket(sock);
            return 100;
        }

        if (length <= 0) {
            errmsg_print("ERROR: Broken socket connection.");
            closesocket(sock);
            return 101;
        }

        while (endless_loop) {
            if (used_buffer_length + length >= 1) {
                hex_data = data + 29;
                hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);

                if  ((hex == 0x01 && used_buffer_length + length >= 4) ||
                        (hex == 0x02 && used_buffer_length + length >= 5) ||
                        (hex == 0x04 && used_buffer_length + length >= 3)) {

                    if (hex == 0x01) {
                        hex_data = new_hex_data;
                        hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);
                        if (hex < 0 || hex >= 256 || hex_data == new_hex_data) {
                            errmsg_print("ERROR: data format error: %s", strerror(errno));
                            closesocket(sock);
                            return 101;
                        }

                        hex_data = new_hex_data;
                        hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);
                        if (hex < 0 || hex >= 256 || hex_data == new_hex_data) {
                            errmsg_print("ERROR: data format error: %s", strerror(errno));
                            closesocket(sock);
                            return 101;
                        }

                        hex_data = new_hex_data;
                        hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);

                        raw_length = hex + 4;
                    } else if (hex == 0x04) {
                        hex_data = new_hex_data;
                        hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);
                        if (hex < 0 || hex >= 256 || hex_data == new_hex_data) {
                            errmsg_print("ERROR: data format error: %s", strerror(errno));
                            closesocket(sock);
                            return 101;
                        }

                        hex_data = new_hex_data;
                        hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);

                        raw_length = hex + 3;
                    } else if (hex == 0x02) {
                        hex_data = new_hex_data;
                        hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);
                        if (hex < 0 || hex >= 256 || hex_data == new_hex_data) {
                            errmsg_print("ERROR: data format error: %s", strerror(errno));
                            closesocket(sock);
                            return 101;
                        }

                        hex_data = new_hex_data;
                        hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);
                        if (hex < 0 || hex >= 256 || hex_data == new_hex_data) {
                            errmsg_print("ERROR: data format error: %s", strerror(errno));
                            closesocket(sock);
                            return 101;
                        }

                        hex_data = new_hex_data;
                        hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);
                        raw_length = hex + 5;

                        hex_data = new_hex_data;
                        hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);
                        raw_length += hex << 8;
                    }

                } else {
                    errmsg_print("ERROR: bad raw stream");
                    closesocket(sock);
                    return 1;
                }
            } else {
                used_buffer_length += length;
                break;
            }

            frame_length = raw_length * 3 + (raw_length / 20) * 4 + ((raw_length % 20) ? 2 : -2) + 29;

            if (used_buffer_length + length < frame_length) {
                used_buffer_length += length;
                break;
            }

            if (8 == sscanf(data, "%04d-%02d-%02d %02d:%02d:%02d.%06d %c",
                    &date.tm_year, &date.tm_mon, &date.tm_mday, &date.tm_hour,
                    &date.tm_min, &date.tm_sec, &ms, &direction_character)) {

                verbose_print("time %04d-%02d-%02d %02d:%02d:%02d.%06d %c\n",
                            date.tm_year, date.tm_mon, date.tm_mday, date.tm_hour,
                            date.tm_min, date.tm_sec, ms, direction_character);
                date.tm_mon -= 1;
                date.tm_year -= 1900;
                ts = mktime(&date);

                new_hex_data = data + 29;
            }

            captured_length = 0;

            while ((long)(new_hex_data - data + sizeof(own_pcap_bluetooth_h4_header)) < frame_length) {
                hex_data = new_hex_data;
                hex = g_ascii_strtoll(hex_data, &new_hex_data, 16);

                packet[sizeof(own_pcap_bluetooth_h4_header) + captured_length] = (char) hex;
                captured_length += 1;
            }

            h4_header->direction = GINT32_TO_BE(direction_character == '>');

            endless_loop = extcap_dumper_dump(extcap_dumper, packet,
                    captured_length + sizeof(own_pcap_bluetooth_h4_header),
                    captured_length + sizeof(own_pcap_bluetooth_h4_header),
                    ts,
                    ms * 1000);

            if (used_buffer_length + length >= frame_length) {
                memmove(data, data + frame_length, (size_t)(used_buffer_length + length - frame_length));
                used_buffer_length = (gssize)(used_buffer_length + length - frame_length);
                length = 0;
                continue;
            }
            length = 0;
        }
    }

    closesocket(sock);
    return 0;
}
