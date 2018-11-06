static int register_interfaces(extcap_parameters * extcap_conf, const char *adb_server_ip, unsigned short *adb_server_tcp_port) {
    static char            packet[PACKET_LENGTH];
    static char            helpful_packet[PACKET_LENGTH];
    char                  *response;
    char                  *device_list;
    gssize                 data_length;
    gssize                 device_length;
    socket_handle_t        sock;
    const char            *adb_transport_serial_templace = "%04x""host:transport:%s";
    const char            *adb_check_port_templace       = "%04x""shell:cat /proc/%s/net/tcp";
    const char            *adb_devices            = "000C""host:devices";
    const char            *adb_api_level          = "0022""shell:getprop ro.build.version.sdk";
    const char            *adb_hcidump_version    = "0017""shell:hcidump --version";
    const char            *adb_ps_droid_bluetooth = "0018""shell:ps droid.bluetooth";
    const char            *adb_ps_bluetooth_app   = "001E""shell:ps com.android.bluetooth";
    const char            *adb_tcpdump_help       = "0010""shell:tcpdump -h";
    char                   serial_number[SERIAL_NUMER_LENGTH_MAX];
    size_t                 serial_number_length;
    int                    result;
    char                  *pos;
    char                  *prev_pos;
    int                    api_level;
    int                    disable_interface;

/

    sock = adb_connect(adb_server_ip, adb_server_tcp_port);
    if (sock == INVALID_SOCKET)
        return -1;

    device_list = adb_send_and_receive(sock, adb_devices, packet, sizeof(packet), &device_length);
    closesocket(sock);

    if (!device_list) {
        errmsg_print("ERROR: Cannot get list of interfaces from devices");

        return -2;
    }

    device_list[device_length] = '\0';
    pos = (char *) device_list;

    while (pos < (char *) (device_list + device_length)) {
        prev_pos = pos;
        pos = strchr(pos, '\t');
        result = (int) (pos - prev_pos);
        pos = strchr(pos, '\n') + 1;
        if (result >= (int) sizeof(serial_number)) {
            verbose_print("WARNING: Serial number too long, ignore device\n");
            continue;
        }
        memcpy(serial_number, prev_pos, result);
        serial_number[result] = '\0';
        serial_number_length = strlen(serial_number);

        verbose_print("VERBOSE: Processing device: \"%s\"\n" , serial_number);

        /

        sock = adb_connect(adb_server_ip, adb_server_tcp_port);
        if (sock == INVALID_SOCKET) continue;

        sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
        result = adb_send(sock, helpful_packet);
        if (result) {
            verbose_print("WARNING: Error while setting adb transport for <%s>\n", helpful_packet);
            closesocket(sock);
        } else {
            response = adb_send_and_read(sock, adb_tcpdump_help, helpful_packet, sizeof(helpful_packet), &data_length);
            closesocket(sock);

            if (response) {
                response[data_length] = '\0';

                /
                if (strstr(response,"tcpdump version")) {
                    new_interface(extcap_conf, INTERFACE_ANDROID_WIFI_TCPDUMP, serial_number, "Android WiFi");
                }
            } else {
                verbose_print("WARNING: Error on socket: <%s>\n", helpful_packet);
            }
        }

        sock = adb_connect(adb_server_ip, adb_server_tcp_port);
        if (sock == INVALID_SOCKET) continue;

        sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
        result = adb_send(sock, helpful_packet);
        if (result) {
            verbose_print("WARNING: Error while setting adb transport for <%s>\n", helpful_packet);
            closesocket(sock);
            continue;
        }

        response = adb_send_and_read(sock, adb_api_level, helpful_packet, sizeof(helpful_packet), &data_length);
        closesocket(sock);

        if (!response) {
            verbose_print("WARNING: Error on socket: <%s>\n", helpful_packet);
            continue;
        }

        response[data_length] = '\0';
        api_level = (int) g_ascii_strtoll(response, NULL, 10);
        verbose_print("VERBOSE: Android API Level for %s is %i\n", serial_number, api_level);

        if (api_level < 21) {
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_MAIN,   serial_number, "Android Logcat Main");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_SYSTEM, serial_number, "Android Logcat System");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_RADIO,  serial_number, "Android Logcat Radio");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_EVENTS, serial_number, "Android Logcat Events");
        } else {
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_MAIN,   serial_number, "Android Logcat Main");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_SYSTEM, serial_number, "Android Logcat System");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_RADIO,  serial_number, "Android Logcat Radio");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_EVENTS, serial_number, "Android Logcat Events");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_CRASH,  serial_number, "Android Logcat Crash");
        }

        if (api_level >= 5 && api_level < 17) {
            disable_interface = 0;

            sock = adb_connect(adb_server_ip, adb_server_tcp_port);
            if (sock == INVALID_SOCKET) continue;

            sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
            result = adb_send(sock, helpful_packet);
            if (result) {
                errmsg_print("ERROR: Error while setting adb transport for <%s>", helpful_packet);
                closesocket(sock);
                return 1;
            }

            response = adb_send_and_read(sock, adb_hcidump_version, helpful_packet, sizeof(helpful_packet), &data_length);
            closesocket(sock);
            if (!response || data_length < 1) {
                verbose_print("WARNING: Error while getting hcidump version by <%s> (%p len=%"G_GSSIZE_FORMAT")\n",
                    adb_hcidump_version, (void*)response, data_length);
                verbose_print("VERBOSE: Android hcidump version for %s is unknown\n", serial_number);
                disable_interface = 1;
            } else {
                response[data_length] = '\0';

                if (g_ascii_strtoull(response, NULL, 10) == 0) {
                    verbose_print("VERBOSE: Android hcidump version for %s is unknown\n", serial_number);
                    disable_interface = 1;
                } else {
                    verbose_print("VERBOSE: Android hcidump version for %s is %s\n", serial_number, response);
                }
            }

            if (!disable_interface) {
                new_interface(extcap_conf, INTERFACE_ANDROID_BLUETOOTH_HCIDUMP, serial_number, "Android Bluetooth Hcidump");
            }
        }

        if (api_level >= 17 && api_level < 21) {
            disable_interface = 0;
            sock = adb_connect(adb_server_ip, adb_server_tcp_port);
            if (sock == INVALID_SOCKET) continue;

            sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
            result = adb_send(sock, helpful_packet);
            if (result) {
                errmsg_print("ERROR: Error while setting adb transport for <%s>", helpful_packet);
                closesocket(sock);
                return 1;
            }

            response = adb_send_and_read(sock, adb_ps_droid_bluetooth, helpful_packet, sizeof(helpful_packet), &data_length);
            closesocket(sock);
            if (!response || data_length < 1) {
                verbose_print("WARNING: Error while getting Bluetooth application process id by <%s> "
                    "(%p len=%"G_GSSIZE_FORMAT")\n", adb_hcidump_version, (void*)response, data_length);
                verbose_print( "VERBOSE: Android Bluetooth application PID for %s is unknown\n", serial_number);
                disable_interface = 1;
            } else {
                char  *data_str;
                char   pid[16];

                memset(pid, 0, sizeof(pid));
                response[data_length] = '\0';

                data_str = strchr(response, '\n');
                if (data_str && sscanf(data_str, "%*s %s", pid) == 1) {
                    verbose_print("VERBOSE: Android Bluetooth application PID for %s is %s\n", serial_number, pid);

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

                    sprintf((char *) helpful_packet, adb_check_port_templace, strlen(adb_check_port_templace) - 6 + strlen(pid), pid);
                    response = adb_send_and_read(sock, helpful_packet, helpful_packet, sizeof(helpful_packet), &data_length);
                    closesocket(sock);

                    if (!response) {
                        disable_interface = 1;
                    } else {
                        response[data_length] = '\0';

                        data_str = strchr(response, '\n');
                        if (data_str && sscanf(data_str, "%*s %s", pid) == 1 && strcmp(pid + 9, "10EA") == 0) {
                            verbose_print("VERBOSE: Bluedroid External Parser Port for %s is %s\n", serial_number, pid + 9);
                        } else {
                            disable_interface = 1;
                            verbose_print("VERBOSE: Bluedroid External Parser Port for %s is unknown\n", serial_number);
                        }
                    }
                } else {
                    disable_interface = 1;
                    verbose_print("VERBOSE: Android Bluetooth application PID for %s is unknown\n", serial_number);
                }
            }

            if (!disable_interface) {
                new_interface(extcap_conf, INTERFACE_ANDROID_BLUETOOTH_EXTERNAL_PARSER, serial_number, "Android Bluetooth External Parser");
            }
        }

        if (api_level >= 21) {
            disable_interface = 0;
            sock = adb_connect(adb_server_ip, adb_server_tcp_port);
            if (sock == INVALID_SOCKET) continue;

            sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + serial_number_length, serial_number);
            result = adb_send(sock, helpful_packet);
            if (result) {
                errmsg_print("ERROR: Error while setting adb transport for <%s>", helpful_packet);
                closesocket(sock);
                return 1;
            }

            if (api_level >= 23) {
                response = adb_send_and_read(sock, adb_ps_bluetooth_app, helpful_packet, sizeof(helpful_packet), &data_length);
            }  else
                response = adb_send_and_read(sock, adb_ps_droid_bluetooth, helpful_packet, sizeof(helpful_packet), &data_length);
            closesocket(sock);
            if (!response || data_length < 1) {
                verbose_print("WARNING: Error while getting Bluetooth application process id by <%s> "
                    "(%p len=%"G_GSSIZE_FORMAT")\n", adb_hcidump_version, (void*)response, data_length);
                verbose_print("VERBOSE: Android Bluetooth application PID for %s is unknown\n", serial_number);
                disable_interface = 1;
            } else {
                char  *data_str;
                char   pid[16];

                memset(pid, 0, sizeof(pid));
                response[data_length] = '\0';

                data_str = strchr(response, '\n');
                if (data_str && sscanf(data_str, "%*s %s", pid) == 1) {
                    verbose_print("VERBOSE: Android Bluetooth application PID for %s is %s\n", serial_number, pid);

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

                    sprintf((char *) helpful_packet, adb_check_port_templace, strlen(adb_check_port_templace) - 6 + strlen(pid), pid);
                    response = adb_send_and_read(sock, helpful_packet, helpful_packet, sizeof(helpful_packet), &data_length);
                    closesocket(sock);

                    if (!response) {
                        disable_interface = 1;
                    } else {
                        response[data_length] = '\0';

                        data_str = strchr(response, '\n');
                        if (data_str && sscanf(data_str, "%*s %s", pid) == 1 && strcmp(pid + 9, "22A8") == 0) {
                            verbose_print("VERBOSE: Btsnoop Net Port for %s is %s\n", serial_number, pid + 9);
                        } else {
                            disable_interface = 1;
                            verbose_print("VERBOSE: Btsnoop Net Port for %s is unknown\n", serial_number);
                        }
                    }
                } else {
                    disable_interface = 1;
                    verbose_print("VERBOSE: Android Bluetooth application PID for %s is unknown\n", serial_number);
                }
            }

            if (!disable_interface) {
                new_interface(extcap_conf, INTERFACE_ANDROID_BLUETOOTH_BTSNOOP_NET, serial_number, "Android Bluetooth Btsnoop Net");
            }
        }
    }

    return 0;
}
