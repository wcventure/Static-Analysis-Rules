static int adb_forward(char *serial_number, const char *adb_server_ip, unsigned short *adb_server_tcp_port,
        unsigned short local_tcp_port, unsigned short server_tcp_port) {
    socket_handle_t       sock;
    int                   result;
    static char           helpful_packet[PACKET_LENGTH];
    static const char    *adb_forward_template = "%04x""%s%s:forward:tcp:%05u;tcp:%05u";

    sock = adb_connect(adb_server_ip, adb_server_tcp_port);
    if (sock == INVALID_SOCKET)
        return -1;

    g_snprintf(helpful_packet, PACKET_LENGTH, adb_forward_template, (serial_number) ? 5 + 7 + strlen(serial_number) + 28 : 4 + 28, (serial_number) ? "host-serial:" : "host", (serial_number) ?  serial_number: "", local_tcp_port, server_tcp_port);
    result = adb_send(sock, helpful_packet);
    closesocket(sock);

    return result;
}
