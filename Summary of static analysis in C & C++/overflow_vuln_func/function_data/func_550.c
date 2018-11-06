static int capture_android_bluetooth_external_parser(char *interface,
        char *fifo, const char *adb_server_ip, unsigned short *adb_server_tcp_port,
        unsigned short *bt_server_tcp_port, unsigned int bt_forward_socket, const char *bt_local_ip,
        unsigned short *bt_local_tcp_port) {
    struct extcap_dumper           extcap_dumper;
    char                           buffer[PACKET_LENGTH];
    uint64_t                      *timestamp;
    char                          *packet = buffer + BLUEDROID_TIMESTAMP_SIZE - sizeof(own_pcap_bluetooth_h4_header); /
    own_pcap_bluetooth_h4_header  *h4_header;
    guint8                        *payload = packet + sizeof(own_pcap_bluetooth_h4_header);
    const char                    *adb_transport  = "0012""host:transport-any";
    const char                    *adb_transport_serial_templace              = "%04x""host:transport:%s";
    const char                    *adb_tcp_bluedroid_external_parser_template = "%04x""tcp:%05u";
    gssize                         length;
    gssize                         used_buffer_length = 0;
    uint64_t                       ts;
    socket_handle_t                sock;
    struct sockaddr_in             server;
    int                            captured_length;
    char                          *serial_number = NULL;
    size_t                         serial_number_length = 0;

    SET_DATA(timestamp, value_u64, buffer);
    SET_DATA(h4_header, value_own_pcap_bluetooth_h4_header, packet);

    extcap_dumper = extcap_dumper_open(fifo, EXTCAP_ENCAP_BLUETOOTH_H4_WITH_PHDR);

    if (is_specified_interface(interface, INTERFACE_ANDROID_BLUETOOTH_EXTERNAL_PARSER) &&
            strlen(interface) > strlen(INTERFACE_ANDROID_BLUETOOTH_EXTERNAL_PARSER) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_BLUETOOTH_EXTERNAL_PARSER) + 1;
        serial_number_length = strlen(serial_number);
    }

    if (bt_forward_socket) {
        if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
            errmsg_print("ERROR: Cannot open system TCP socket: %s", strerror(errno));
            return 1;
        }

        verbose_print("Using config: Server TCP Port=%u, Local IP=%s, Local TCP Port=%u\n",
                    *bt_server_tcp_port, bt_local_ip, *bt_local_tcp_port);

        if (*bt_local_tcp_port != 0) {
            int result;

            result = adb_forward(serial_number, adb_server_ip, adb_server_tcp_port, *bt_local_tcp_port, *bt_server_tcp_port);
            verbose_print("DO: adb forward tcp:%u (local) tcp:%u (remote) result=%i\n",
                        *bt_local_tcp_port, *bt_server_tcp_port, result);
        }

        memset(&server, 0 , sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = GINT16_TO_BE(*bt_local_tcp_port);
        server.sin_addr.s_addr = inet_addr(bt_local_ip);

        if (connect(sock, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
            errmsg_print("ERROR: <%s> Please check that adb daemon is running.", strerror(errno));
            closesocket(sock);
            return 2;
        }

        if (verbose) {
            struct sockaddr_in  client;

            length = sizeof(client);
            if (getsockname(sock, (struct sockaddr *) &client, (socklen_t *) &length)) {
                errmsg_print("ERROR getsockname: %s", strerror(errno));
                closesocket(sock);
                return 3;
            }

            if (length != sizeof(client)) {
                errmsg_print("ERROR: incorrect length");
                closesocket(sock);
                return 4;
            }

            verbose_print("VERBOSE: Client port %u\n", GUINT16_FROM_BE(client.sin_port));
        }
    } else {
        int  result;

        sock = adb_connect(adb_server_ip, adb_server_tcp_port);
        if (sock == INVALID_SOCKET)
            return -1;

        if (!serial_number) {
            result = adb_send(sock, adb_transport);
            if (result) {
                errmsg_print("ERROR: Error while setting adb transport");
                closesocket(sock);
                return 1;
            }
        } else {
            g_snprintf((char *) buffer, PACKET_LENGTH, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
            result = adb_send(sock, buffer);
            if (result) {
                errmsg_print("ERROR: Error while setting adb transport");
                closesocket(sock);
                return 1;
            }
        }

        sprintf((char *) buffer, adb_tcp_bluedroid_external_parser_template, 4 + 5, *bt_server_tcp_port);
        result = adb_send(sock, buffer);
        if (result) {
            errmsg_print("ERROR: Error while forwarding adb port");
            closesocket(sock);
            return 1;
        }
    }

    while (endless_loop) {
        errno = 0;
        length = recv(sock, buffer + used_buffer_length,  (int)(PACKET_LENGTH - used_buffer_length), 0);
        if (errno == EAGAIN || (EWOULDBLOCK != EAGAIN && errno == EWOULDBLOCK)) continue;
        else if (errno != 0) {
            errmsg_print("ERROR capture: %s", strerror(errno));
            closesocket(sock);
            return 100;
        }

        if (length <= 0) {
            if (bt_forward_socket) {
                /
                verbose_print("WARNING: Broken socket connection. Try reconnect.\n");
                closesocket(sock);

                if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
                    errmsg_print("ERROR: %s", strerror(errno));
                    return 1;
                }

                server.sin_family = AF_INET;
                server.sin_port = GINT16_TO_BE(*bt_local_tcp_port);
                server.sin_addr.s_addr = inet_addr(bt_local_ip);

                if (connect(sock, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
                    errmsg_print("ERROR reconnect: <%s> Please check that adb daemon is running.", strerror(errno));
                    closesocket(sock);
                    return 2;
                }
            } else {
                errmsg_print("ERROR: Broken socket connection.");
                closesocket(sock);
                return 1;
            }

            continue;
        }

        used_buffer_length += length;

        verbose_print("Received: length=%"G_GSSIZE_FORMAT"\n", length);

        while (((payload[BLUEDROID_H4_PACKET_TYPE] == BLUEDROID_H4_PACKET_TYPE_HCI_CMD || payload[BLUEDROID_H4_PACKET_TYPE] == BLUEDROID_H4_PACKET_TYPE_SCO) &&
                    used_buffer_length >= BLUEDROID_TIMESTAMP_SIZE + BLUEDROID_H4_SIZE + 2 + 1 &&
                    BLUEDROID_TIMESTAMP_SIZE + BLUEDROID_H4_SIZE + 2 + payload[BLUEDROID_H4_SIZE + 2] + 1 <= used_buffer_length) ||
                (payload[BLUEDROID_H4_PACKET_TYPE] == BLUEDROID_H4_PACKET_TYPE_ACL &&
                    used_buffer_length >= BLUEDROID_TIMESTAMP_SIZE + BLUEDROID_H4_SIZE + 2 + 2 &&
                    BLUEDROID_TIMESTAMP_SIZE + BLUEDROID_H4_SIZE + 2 + payload[BLUEDROID_H4_SIZE + 2] + (payload[BLUEDROID_H4_SIZE + 2 + 1] << 8) + 2 <= used_buffer_length) ||
                (payload[BLUEDROID_H4_PACKET_TYPE] == BLUEDROID_H4_PACKET_TYPE_SCO &&
                    used_buffer_length >= BLUEDROID_TIMESTAMP_SIZE + BLUEDROID_H4_SIZE + 2 + 1 &&
                    BLUEDROID_TIMESTAMP_SIZE + BLUEDROID_H4_SIZE + 2 + payload[BLUEDROID_H4_SIZE + 2] + 1 <= used_buffer_length) ||
                (payload[BLUEDROID_H4_PACKET_TYPE] == BLUEDROID_H4_PACKET_TYPE_HCI_EVT &&
                    used_buffer_length >= BLUEDROID_TIMESTAMP_SIZE + BLUEDROID_H4_SIZE + 1 + 1 &&
                    BLUEDROID_TIMESTAMP_SIZE + BLUEDROID_H4_SIZE + 1 + payload[BLUEDROID_H4_SIZE + 1] + 1 <= used_buffer_length)) {

            ts = GINT64_FROM_BE(*timestamp);

            switch (payload[BLUEDROID_H4_PACKET_TYPE]) {
            case BLUEDROID_H4_PACKET_TYPE_HCI_CMD:
                h4_header->direction = GINT32_TO_BE(BLUEDROID_DIRECTION_SENT);

                captured_length = (unsigned int)sizeof(own_pcap_bluetooth_h4_header) + payload[3] + 4;

                length = sizeof(own_pcap_bluetooth_h4_header) + BLUEDROID_H4_SIZE + 2 + 1 + payload[3];

                break;
            case BLUEDROID_H4_PACKET_TYPE_ACL:
                h4_header->direction = (payload[2] & 0x80) ? GINT32_TO_BE(BLUEDROID_DIRECTION_RECV) : GINT32_TO_BE(BLUEDROID_DIRECTION_SENT);

                captured_length = (unsigned int)sizeof(own_pcap_bluetooth_h4_header) + payload[3] + (payload[3 + 1] << 8) + 5;

                length = sizeof(own_pcap_bluetooth_h4_header) + BLUEDROID_H4_SIZE + 2 + 2 + payload[3] + (payload[3 + 1] << 8);

                break;
            case BLUEDROID_H4_PACKET_TYPE_SCO:
                h4_header->direction = (payload[2] & 0x80) ? GINT32_TO_BE(BLUEDROID_DIRECTION_RECV) : GINT32_TO_BE(BLUEDROID_DIRECTION_SENT);

                captured_length = (unsigned int)sizeof(own_pcap_bluetooth_h4_header) + payload[3] + 4;

                length = sizeof(own_pcap_bluetooth_h4_header) + BLUEDROID_H4_SIZE + 2 + 1 + payload[3];

                break;
            case BLUEDROID_H4_PACKET_TYPE_HCI_EVT:
                h4_header->direction = GINT32_TO_BE(BLUEDROID_DIRECTION_RECV);

                captured_length = (unsigned int)sizeof(own_pcap_bluetooth_h4_header) + payload[2] + 3;

                length = sizeof(own_pcap_bluetooth_h4_header) + BLUEDROID_H4_SIZE + 1 + 1 + payload[2];

                break;
            default:
                errmsg_print("ERROR: Invalid stream");
                closesocket(sock);
                return 1;
            }

            if (verbose) {
                static unsigned int id = 1;
                verbose_print("\t Packet %u: used_buffer_length=%"G_GSSIZE_FORMAT" length=%"G_GSSIZE_FORMAT" captured_length=%i type=0x%02x\n", id, used_buffer_length, length, captured_length, payload[BLUEDROID_H4_PACKET_TYPE]);
                if (payload[BLUEDROID_H4_PACKET_TYPE] == BLUEDROID_H4_PACKET_TYPE_HCI_EVT)
                    verbose_print("\t Packet: %02x %02x %02x\n", (unsigned int) payload[0], (unsigned int) payload[1], (unsigned int)payload[2]);
                id +=1;
            }

            ts -= BLUEDROID_TIMESTAMP_BASE;

            endless_loop = extcap_dumper_dump(extcap_dumper, packet,
                    captured_length,
                    captured_length,
                    (uint32_t)(ts / 1000000),
                    ((uint32_t)(ts % 1000000)) * 1000);

            used_buffer_length -= length - sizeof(own_pcap_bluetooth_h4_header) + BLUEDROID_TIMESTAMP_SIZE;
            if (used_buffer_length < 0) {
                errmsg_print("ERROR: Internal error: Negative used buffer length.");
                closesocket(sock);
                return 1;
            }
            memmove(buffer, packet + length, used_buffer_length);
        }
    }

    closesocket(sock);
    return 0;
}
