static int capture_android_logcat_text(char *interface, char *fifo,
        const char *adb_server_ip, unsigned short *adb_server_tcp_port) {
    struct extcap_dumper        extcap_dumper;
    static char                 packet[PACKET_LENGTH];
    gssize                      length;
    size_t                      used_buffer_length = 0;
    socket_handle_t             sock;
    const char                 *protocol_name;
    size_t                      exported_pdu_headers_size = 0;
    struct exported_pdu_header  exported_pdu_header_protocol_normal;
    struct exported_pdu_header *exported_pdu_header_protocol;
    struct exported_pdu_header  exported_pdu_header_end = {0, 0};
    static const char          *wireshark_protocol_logcat_text = "logcat_text_threadtime";
    const char                 *adb_transport = "0012""host:transport-any";
    const char                 *adb_logcat_template = "%04x""shell:export ANDROID_LOG_TAGS=\"\" ; exec logcat -v threadtime%s%s";
    const char                 *adb_transport_serial_templace = "%04x""host:transport:%s";
    char                       *serial_number = NULL;
    size_t                      serial_number_length = 0;
    int                         result;
    char                       *pos;
    const char                 *logcat_buffer;

    extcap_dumper = extcap_dumper_open(fifo, EXTCAP_ENCAP_WIRESHARK_UPPER_PDU);

    exported_pdu_header_protocol_normal.tag = GUINT16_TO_BE(WIRESHARK_UPPER_PDU_TAG_DISSECTOR_NAME);
    exported_pdu_header_protocol_normal.length = GUINT16_TO_BE(strlen(wireshark_protocol_logcat_text) + 2);

    sock = adb_connect(adb_server_ip, adb_server_tcp_port);
    if (sock == INVALID_SOCKET)
        return -1;

    if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_MAIN) && strlen(interface) > strlen(INTERFACE_ANDROID_LOGCAT_MAIN) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_LOGCAT_MAIN) + 1;
    } else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_SYSTEM) && strlen(interface) > strlen(INTERFACE_ANDROID_LOGCAT_SYSTEM) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_LOGCAT_SYSTEM) + 1;
    } else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_RADIO) && strlen(interface) > strlen(INTERFACE_ANDROID_LOGCAT_RADIO) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_LOGCAT_RADIO) + 1;
    } else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_EVENTS) && strlen(interface) > strlen(INTERFACE_ANDROID_LOGCAT_EVENTS) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_LOGCAT_EVENTS) + 1;
    } else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_TEXT_MAIN) && strlen(interface) > strlen(INTERFACE_ANDROID_LOGCAT_TEXT_MAIN) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_LOGCAT_TEXT_MAIN) + 1;
    } else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_TEXT_SYSTEM) && strlen(interface) > strlen(INTERFACE_ANDROID_LOGCAT_TEXT_SYSTEM) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_LOGCAT_TEXT_SYSTEM) + 1;
    } else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_TEXT_RADIO) && strlen(interface) > strlen(INTERFACE_ANDROID_LOGCAT_TEXT_RADIO) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_LOGCAT_TEXT_RADIO) + 1;
    } else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_TEXT_EVENTS) && strlen(interface) > strlen(INTERFACE_ANDROID_LOGCAT_TEXT_EVENTS) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_LOGCAT_TEXT_EVENTS) + 1;
    } else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_TEXT_CRASH) && strlen(interface) > strlen(INTERFACE_ANDROID_LOGCAT_TEXT_CRASH) + 1) {
        serial_number = interface + strlen(INTERFACE_ANDROID_LOGCAT_TEXT_CRASH) + 1;
    }

    if (!serial_number) {
        result = adb_send(sock, adb_transport);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport for <%s>", adb_transport);
            closesocket(sock);
            return 1;
        }
    } else {
        serial_number_length = strlen(serial_number);

        sprintf((char *) packet, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
        result = adb_send(sock, packet);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport for <%s>", packet);
            closesocket(sock);
            return 1;
        }
    }

    if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_MAIN) || is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_TEXT_MAIN))
        logcat_buffer = " -b main";
    else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_SYSTEM) || is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_TEXT_SYSTEM))
        logcat_buffer = " -b system";
    else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_RADIO) || is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_TEXT_RADIO))
        logcat_buffer = " -b radio";
    else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_EVENTS) || is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_TEXT_EVENTS))
        logcat_buffer = " -b events";
    else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_TEXT_CRASH))
        logcat_buffer = " -b crash";
    else {
        errmsg_print("ERROR: Unknown interface: <%s>", interface);
        closesocket(sock);
        return -1;
    }


    g_snprintf((char *) packet, sizeof(packet), adb_logcat_template, strlen(adb_logcat_template) + -8 + strlen(logcat_buffer), logcat_buffer, "");
    result = adb_send(sock, packet);
    if (result) {
        errmsg_print("ERROR: Error while sending command <%s>", packet);
        closesocket(sock);
        return 1;
    }

    protocol_name = wireshark_protocol_logcat_text;
    exported_pdu_header_protocol = &exported_pdu_header_protocol_normal;

    memcpy(packet, exported_pdu_header_protocol, sizeof(struct exported_pdu_header));
    exported_pdu_headers_size += sizeof(struct exported_pdu_header);

    memcpy(packet + exported_pdu_headers_size, protocol_name, GUINT16_FROM_BE(exported_pdu_header_protocol->length) - 2);
    exported_pdu_headers_size += GUINT16_FROM_BE(exported_pdu_header_protocol->length);

    packet[exported_pdu_headers_size - 1] = 0;
    packet[exported_pdu_headers_size - 2] = 0;

    memcpy(packet + exported_pdu_headers_size, &exported_pdu_header_end, sizeof(struct exported_pdu_header));
    exported_pdu_headers_size += sizeof(struct exported_pdu_header) + GUINT16_FROM_BE(exported_pdu_header_end.length);

    used_buffer_length = 0;
    while (endless_loop) {
        errno = 0;
        length = recv(sock, packet + exported_pdu_headers_size + used_buffer_length,  (int)(PACKET_LENGTH - exported_pdu_headers_size - used_buffer_length), 0);
        if (errno == EAGAIN || (EWOULDBLOCK != EAGAIN && errno == EWOULDBLOCK)) continue;
        else if (errno != 0) {
            errmsg_print("ERROR capture: %s", strerror(errno));
            closesocket(sock);
            return 100;
        }

        if (length <= 0) {
            errmsg_print("ERROR: Broken socket connection. Try reconnect.");
            closesocket(sock);
            return 1;
        }

        used_buffer_length += length;

        while (used_buffer_length > 0 && (pos = (char *) memchr(packet + exported_pdu_headers_size, '\n', used_buffer_length))) {
            int        ms;
            struct tm  date;
            time_t     seconds;
            time_t     secs = 0;
            int        nsecs = 0;

            length = (gssize)(pos - packet) + 1;

            if (6 == sscanf(packet + exported_pdu_headers_size, "%d-%d %d:%d:%d.%d", &date.tm_mon, &date.tm_mday, &date.tm_hour,
                            &date.tm_min, &date.tm_sec, &ms)) {
                date.tm_year = 70;
                date.tm_mon -= 1;
                seconds = mktime(&date);
                secs = (time_t) seconds;
                nsecs = (int) (ms * 1e6);
            }

            endless_loop = extcap_dumper_dump(extcap_dumper, packet,
                    length,
                    length,
                    secs, nsecs);

            memmove(packet + exported_pdu_headers_size, packet + length, used_buffer_length + exported_pdu_headers_size - length);
            used_buffer_length -= length - exported_pdu_headers_size;
        }
    }

    closesocket(sock);
    return 0;
}
