static int capture_android_bluetooth_btsnoop_net(char *interface, char *fifo,
        const char *adb_server_ip, unsigned short *adb_server_tcp_port) {
    struct extcap_dumper           extcap_dumper;
    static char                    packet[PACKET_LENGTH];
    gssize                         length;
    gssize                         used_buffer_length = 0;
    socket_handle_t                sock;
    const char                    *adb_transport  = "0012""host:transport-any";
    const char                    *adb_transport_serial_templace = "%04x""host:transport:%s";
    const char                    *adb_tcp_btsnoop_net   = "0008""tcp:8872";
    int                            result;
    char                          *serial_number = NULL;
    size_t                         serial_number_length;
    uint64_t                       ts;
    static const uint64_t          BTSNOOP_TIMESTAMP_BASE = G_GUINT64_CONSTANT(0x00dcddb30f2f8000);
    uint32_t                      *reported_length;
    uint32_t                      *captured_length;
    uint32_t                      *flags;
/
    uint64_t                      *timestamp;
    char                          *payload                     =  packet + sizeof(own_pcap_bluetooth_h4_header) + 24;
    own_pcap_bluetooth_h4_header  *h4_header;

    SET_DATA(reported_length, value_u32, packet + sizeof(own_pcap_bluetooth_h4_header) + 0);
    SET_DATA(captured_length, value_u32, packet + sizeof(own_pcap_bluetooth_h4_header) + 4);
    SET_DATA(flags, value_u32, packet + sizeof(own_pcap_bluetooth_h4_header) + 8);
/
    SET_DATA(timestamp, value_u64, packet + sizeof(own_pcap_bluetooth_h4_header) + 16);
    SET_DATA(h4_header, value_own_pcap_bluetooth_h4_header, payload - sizeof(own_pcap_bluetooth_h4_header));

    extcap_dumper = extcap_dumper_open(fifo, EXTCAP_ENCAP_BLUETOOTH_H4_WITH_PHDR);

    sock = adb_connect(adb_server_ip, adb_server_tcp_port);
    if (sock == INVALID_SOCKET)
        return -1;

    if (is_specified_interface(interface, INTERFACE_ANDROID_BLUETOOTH_BTSNOOP_NET) &&
            strlen(interface) > strlen(INTERFACE_ANDROID_BLUETOOTH_BTSNOOP_NET) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_BLUETOOTH_BTSNOOP_NET) + 1;
        serial_number_length = strlen(serial_number);
    }

    if (!serial_number) {
        result = adb_send(sock, adb_transport);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport for <%s>", adb_transport);
            return 1;
        }
    } else {
        sprintf((char *) packet, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
        result = adb_send(sock, packet);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport for <%s>", packet);
            closesocket(sock);
            return 1;
        }
    }

    result = adb_send(sock, adb_tcp_btsnoop_net);
    if (result) {
        errmsg_print("ERROR: Error while sending command <%s>", adb_tcp_btsnoop_net);
        closesocket(sock);
        return 1;
    }

    /
    while (used_buffer_length < 16) {
        length = recv(sock, packet + used_buffer_length,  (int)(PACKET_LENGTH - used_buffer_length), 0);

        if (length <= 0) {
            errmsg_print("ERROR: Broken socket connection.");
            closesocket(sock);
            return 101;
        }

        used_buffer_length += length;
    }
    if (used_buffer_length > 16)
        memmove(packet, packet + 16, used_buffer_length - 16);

    used_buffer_length = 0;

    while (endless_loop) {
        errno = 0;
        length = recv(sock, packet + used_buffer_length + sizeof(own_pcap_bluetooth_h4_header),
                (int)(PACKET_LENGTH - sizeof(own_pcap_bluetooth_h4_header) - used_buffer_length), 0);
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

        while (used_buffer_length >= 24 &&
                used_buffer_length >= (int) (24 + GINT32_FROM_BE(*captured_length))) {
            gint32 direction;

            ts = GINT64_FROM_BE(*timestamp);
            ts -= BTSNOOP_TIMESTAMP_BASE;

            direction = GINT32_FROM_BE(*flags) & 0x01;
            h4_header->direction = GINT32_TO_BE(direction);

            endless_loop = extcap_dumper_dump(extcap_dumper, payload - sizeof(own_pcap_bluetooth_h4_header),
                    GINT32_FROM_BE(*captured_length) + sizeof(own_pcap_bluetooth_h4_header),
                    GINT32_FROM_BE(*reported_length) + sizeof(own_pcap_bluetooth_h4_header),
                    (uint32_t)(ts / 1000000),
                    ((uint32_t)(ts % 1000000)) * 1000);

            used_buffer_length -= 24 + GINT32_FROM_BE(*captured_length);
            if (used_buffer_length < 0) {
                errmsg_print("ERROR: Internal error: Negative used buffer length.");
                closesocket(sock);
                return 1;
            }

            if  (used_buffer_length > 0)
                memmove(packet + sizeof(own_pcap_bluetooth_h4_header), payload + GINT32_FROM_BE(*captured_length), used_buffer_length);
        }
    }

    closesocket(sock);
    return 0;
}
