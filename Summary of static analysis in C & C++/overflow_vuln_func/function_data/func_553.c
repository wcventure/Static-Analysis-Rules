static int capture_android_logcat(char *interface, char *fifo,
        const char *adb_server_ip, unsigned short *adb_server_tcp_port) {
    struct extcap_dumper        extcap_dumper;
    static char                 packet[PACKET_LENGTH];
    static char                 helper_packet[PACKET_LENGTH];
    gssize                      length;
    size_t                      used_buffer_length = 0;
    socket_handle_t             sock;
    const char                 *protocol_name;
    size_t                      exported_pdu_headers_size = 0;
    struct exported_pdu_header  exported_pdu_header_protocol_events;
    struct exported_pdu_header  exported_pdu_header_protocol_normal;
    struct exported_pdu_header *exported_pdu_header_protocol;
    struct exported_pdu_header  exported_pdu_header_end = {0, 0};
    static const char          *wireshark_protocol_logcat = "logcat";
    static const char          *wireshark_protocol_logcat_events = "logcat_events";
    const char                 *adb_transport  = "0012""host:transport-any";
    const char                 *adb_log_main   = "0008""log:main";
    const char                 *adb_log_system = "000A""log:system";
    const char                 *adb_log_radio  = "0009""log:radio";
    const char                 *adb_log_events = "000A""log:events";
    const char                 *adb_transport_serial_templace = "%04x""host:transport:%s";
    const char                 *adb_command;
    uint16_t                   *payload_length;
    uint16_t                   *try_header_size;
    uint32_t                   *timestamp_secs;
    uint32_t                   *timestamp_nsecs;
    uint16_t                    header_size;
    int                         result;
    char                       *serial_number = NULL;
    size_t                      serial_number_length = 0;

    extcap_dumper = extcap_dumper_open(fifo, EXTCAP_ENCAP_WIRESHARK_UPPER_PDU);

    exported_pdu_header_protocol_events.tag = GUINT16_TO_BE(WIRESHARK_UPPER_PDU_TAG_DISSECTOR_NAME);
    exported_pdu_header_protocol_events.length = GUINT16_TO_BE(strlen(wireshark_protocol_logcat_events) + 2);

    exported_pdu_header_protocol_normal.tag = GUINT16_TO_BE(WIRESHARK_UPPER_PDU_TAG_DISSECTOR_NAME);
    exported_pdu_header_protocol_normal.length = GUINT16_TO_BE(strlen(wireshark_protocol_logcat) + 2);

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
    }

    if (!serial_number) {
        result = adb_send(sock, adb_transport);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport");
            closesocket(sock);
            return 1;
        }
    } else {
        serial_number_length = strlen(serial_number);

        g_snprintf(packet, PACKET_LENGTH, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
        result = adb_send(sock, packet);
        if (result) {
            errmsg_print("ERROR: Error while setting adb transport");
            closesocket(sock);
            return 1;
        }
    }

    if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_MAIN))
        adb_command = adb_log_main;
    else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_SYSTEM))
        adb_command = adb_log_system;
    else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_RADIO))
        adb_command = adb_log_radio;
    else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_EVENTS))
        adb_command = adb_log_events;
    else {
        errmsg_print("ERROR: Unknown interface: <%s>", interface);
        closesocket(sock);
        return -1;
    }

    result = adb_send(sock, adb_command);
    if (result) {
        errmsg_print("ERROR: Error while sending command <%s>", adb_command);
        closesocket(sock);
        return 1;
    }

    if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_EVENTS))
    {
        protocol_name = wireshark_protocol_logcat_events;
        exported_pdu_header_protocol = &exported_pdu_header_protocol_events;
    } else {
        protocol_name = wireshark_protocol_logcat;
        exported_pdu_header_protocol = &exported_pdu_header_protocol_normal;
    }

    memcpy(packet, exported_pdu_header_protocol, sizeof(struct exported_pdu_header));
    exported_pdu_headers_size += sizeof(struct exported_pdu_header);

    memcpy(packet + exported_pdu_headers_size, protocol_name, GUINT16_FROM_BE(exported_pdu_header_protocol->length) - 2);
    exported_pdu_headers_size += GUINT16_FROM_BE(exported_pdu_header_protocol->length);

    packet[exported_pdu_headers_size - 1] = 0;
    packet[exported_pdu_headers_size - 2] = 0;

    memcpy(packet + exported_pdu_headers_size, &exported_pdu_header_end, sizeof(struct exported_pdu_header));
    exported_pdu_headers_size += sizeof(struct exported_pdu_header) + GUINT16_FROM_BE(exported_pdu_header_end.length);

    SET_DATA(payload_length,  value_u16, packet + exported_pdu_headers_size +  0);
    SET_DATA(try_header_size, value_u16, packet + exported_pdu_headers_size +  2);
    SET_DATA(timestamp_secs,  value_u32, packet + exported_pdu_headers_size + 12);
    SET_DATA(timestamp_nsecs, value_u32, packet + exported_pdu_headers_size + 16);

    while (endless_loop) {
        errno = 0;
        length = recv(sock, packet + exported_pdu_headers_size + used_buffer_length, (int)(PACKET_LENGTH - exported_pdu_headers_size - used_buffer_length), 0);
        if (errno == EAGAIN || (EWOULDBLOCK != EAGAIN && errno == EWOULDBLOCK)) continue;
        else if (errno != 0) {
            errmsg_print("ERROR capture: %s", strerror(errno));
            closesocket(sock);
            return 100;
        }

        if (length <= 0) {
            while (endless_loop) {
                verbose_print("WARNING: Broken socket connection. Try reconnect.\n");
                used_buffer_length = 0;
                closesocket(sock);

                sock = adb_connect(adb_server_ip, adb_server_tcp_port);
                if (sock == INVALID_SOCKET)
                    return -1;

                if (!serial_number) {
                    result = adb_send(sock, adb_transport);
                    if (result) {
                        errmsg_print("ERROR: Error while setting adb transport for <%s>", adb_transport);
                        closesocket(sock);
                        return 1;
                    }
                } else {
                    sprintf((char *) helper_packet, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
                    result = adb_send(sock, helper_packet);
                    if (result) {
                        errmsg_print("ERROR: Error while setting adb transport for <%s>", helper_packet);
                        closesocket(sock);
                        return 1;
                    }
                }

                if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_MAIN))
                    adb_command = adb_log_main;
                else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_SYSTEM))
                    adb_command = adb_log_system;
                else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_RADIO))
                    adb_command = adb_log_radio;
                else if (is_specified_interface(interface, INTERFACE_ANDROID_LOGCAT_EVENTS))
                    adb_command = adb_log_events;
                else {
                    errmsg_print("ERROR: Unknown interface: <%s>", interface);
                    closesocket(sock);
                    return 1;
                }

                result = adb_send(sock, adb_command);
                if (result) {
                    errmsg_print("ERROR: Error while sending command <%s>", adb_command);
                    continue;
                }

                break;
            }
        }

        used_buffer_length += length + exported_pdu_headers_size;

        if (*try_header_size != 24)
            header_size = 20;
        else
            header_size = *try_header_size;

        length = (*payload_length) + header_size + (gssize)exported_pdu_headers_size;

        while (used_buffer_length >= exported_pdu_headers_size + header_size && (size_t)length <= used_buffer_length) {
            endless_loop = extcap_dumper_dump(extcap_dumper, packet,
                    length,
                    length,
                    *timestamp_secs, *timestamp_nsecs);

            memmove(packet + exported_pdu_headers_size, packet + length, used_buffer_length - length);
            used_buffer_length -= length;
            used_buffer_length += exported_pdu_headers_size;


            length = (*payload_length) + header_size + (gssize)exported_pdu_headers_size;

            if (*try_header_size != 24)
                header_size = 20;
            else
                header_size = *try_header_size;
        }
        used_buffer_length -= exported_pdu_headers_size;
    }

    closesocket(sock);

    return 0;
}
