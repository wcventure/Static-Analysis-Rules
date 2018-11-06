static int register_interfaces(extcap_parameters * extcap_conf, const char *adb_server_ip, unsigned short *adb_server_tcp_port) {
    static char            packet[PACKET_LENGTH];
    static char            helpful_packet[PACKET_LENGTH];
    char                   check_port_buf[80];
    char                  *response;
    char                  *device_list;
    gssize                 data_length;
    size_t                 device_length;
    socket_handle_t        sock;
    const char            *adb_check_port_templace       = "shell:cat /proc/%s/net/tcp";
    const char            *adb_devices            = "host:devices-l";
    const char            *adb_api_level          = "shell:getprop ro.build.version.sdk";
    const char            *adb_hcidump_version    = "shell:hcidump --version";
    const char            *adb_ps_droid_bluetooth = "shell:ps droid.bluetooth";
    const char            *adb_ps_bluetooth_app   = "shell:ps com.android.bluetooth";
    const char            *adb_ps_with_grep       = "shell:ps | grep com.android.bluetooth";
    char                   serial_number[SERIAL_NUMBER_LENGTH_MAX];
    char                   model_name[MODEL_NAME_LENGTH_MAX];
    int                    result;
    char                  *pos;
    char                  *i_pos;
    char                  *model_pos;
    char                  *device_pos;
    char                  *prev_pos;
    int                    api_level;
    int                    disable_interface;

/

    sock = adb_connect(adb_server_ip, adb_server_tcp_port);
    if (sock == INVALID_SOCKET)
        return EXIT_CODE_INVALID_SOCKET_INTERFACES_LIST;

    device_list = adb_send_and_receive(sock, adb_devices, packet, sizeof(packet), &device_length);
    closesocket(sock);

    if (!device_list) {
        g_warning("Cannot get list of interfaces from devices");

        return EXIT_CODE_CANNOT_GET_INTERFACES_LIST;
    }

    device_list[device_length] = '\0';
    pos = (char *) device_list;

    while (pos < (char *) (device_list + device_length)) {
        prev_pos = pos;
        pos = strchr(pos, ' ');
        i_pos = pos;
        result = (int) (pos - prev_pos);
        pos = strchr(pos, '\n') + 1;
        if (result >= (int) sizeof(serial_number)) {
            g_warning("Serial number too long, ignore device");
            continue;
        }
        memcpy(serial_number, prev_pos, result);
        serial_number[result] = '\0';

        model_name[0] = '\0';
        model_pos = g_strstr_len(i_pos, pos - i_pos, "model:");
        if (model_pos) {
            device_pos = g_strstr_len(i_pos, pos - i_pos, "device:");
            if (device_pos && device_pos - model_pos - 6 - 1 < MODEL_NAME_LENGTH_MAX) {
                memcpy(model_name, model_pos + 6, device_pos - model_pos - 6 - 1);
                model_name[device_pos - model_pos - 6 - 1] = '\0';
            }
        }

        if (model_name[0] == '\0')
            strcpy(model_name, "unknown");

        g_debug("Processing device: \"%s\" <%s>" , serial_number, model_name);

        /
        result = add_tcpdump_interfaces(extcap_conf, adb_server_ip, adb_server_tcp_port, serial_number  );
        if (result) {
            g_warning("Error while adding tcpdump interfaces");
        }

        sock = adb_connect_transport(adb_server_ip, adb_server_tcp_port, serial_number);
        if (sock == INVALID_SOCKET) continue;

        response = adb_send_and_read(sock, adb_api_level, helpful_packet, sizeof(helpful_packet), &data_length);
        closesocket(sock);

        if (!response) {
            g_warning("Error on socket: <%s>", helpful_packet);
            continue;
        }

        response[data_length] = '\0';
        api_level = (int) g_ascii_strtoll(response, NULL, 10);
        g_debug("Android API Level for %s is %i", serial_number, api_level);

        if (api_level < 21) {
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_MAIN,   model_name, serial_number, "Android Logcat Main");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_SYSTEM, model_name, serial_number, "Android Logcat System");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_RADIO,  model_name, serial_number, "Android Logcat Radio");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_EVENTS, model_name, serial_number, "Android Logcat Events");

            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_MAIN,   model_name, serial_number, "Android Logcat Main");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_SYSTEM, model_name, serial_number, "Android Logcat System");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_RADIO,  model_name, serial_number, "Android Logcat Radio");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_EVENTS, model_name, serial_number, "Android Logcat Events");
        } else {
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_MAIN,   model_name, serial_number, "Android Logcat Main");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_SYSTEM, model_name, serial_number, "Android Logcat System");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_RADIO,  model_name, serial_number, "Android Logcat Radio");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_EVENTS, model_name, serial_number, "Android Logcat Events");
            new_interface(extcap_conf, INTERFACE_ANDROID_LOGCAT_TEXT_CRASH,  model_name, serial_number, "Android Logcat Crash");
        }

        if (api_level >= 5 && api_level < 17) {
            disable_interface = 0;

            sock = adb_connect_transport(adb_server_ip, adb_server_tcp_port, serial_number);
            if (sock == INVALID_SOCKET) continue;

            response = adb_send_and_read(sock, adb_hcidump_version, helpful_packet, sizeof(helpful_packet), &data_length);
            closesocket(sock);

            if (!response || data_length < 1) {
                g_warning("Error while getting hcidump version by <%s> (%p len=%"G_GSSIZE_FORMAT")",
                    adb_hcidump_version, (void*)response, data_length);
                g_debug("Android hcidump version for %s is unknown", serial_number);
                disable_interface = 1;
            } else {
                response[data_length] = '\0';

                if (g_ascii_strtoull(response, NULL, 10) == 0) {
                    g_debug("Android hcidump version for %s is unknown", serial_number);
                    disable_interface = 1;
                } else {
                    g_debug("Android hcidump version for %s is %s", serial_number, response);
                }
            }

            if (!disable_interface) {
                new_interface(extcap_conf, INTERFACE_ANDROID_BLUETOOTH_HCIDUMP, model_name, serial_number, "Android Bluetooth Hcidump");
            }
        }

        if (api_level >= 17 && api_level < 21) {
            disable_interface = 0;
            sock = adb_connect_transport(adb_server_ip, adb_server_tcp_port, serial_number);
            if (sock == INVALID_SOCKET) continue;

            response = adb_send_and_read(sock, adb_ps_droid_bluetooth, helpful_packet, sizeof(helpful_packet), &data_length);
            closesocket(sock);
            if (!response || data_length < 1) {
                g_warning("Error while getting Bluetooth application process id by <%s> "
                    "(%p len=%"G_GSSIZE_FORMAT")", adb_hcidump_version, (void*)response, data_length);
                g_debug( "Android Bluetooth application PID for %s is unknown", serial_number);
                disable_interface = 1;
            } else {
                char  *data_str;
                char   pid[16];

                memset(pid, 0, sizeof(pid));
                response[data_length] = '\0';

                data_str = strchr(response, '\n');
                if (data_str && sscanf(data_str, "%*s %s", pid) == 1) {
                    g_debug("Android Bluetooth application PID for %s is %s", serial_number, pid);

                    result = g_snprintf(check_port_buf, sizeof(check_port_buf), adb_check_port_templace, pid);
                    if (result <= 0 || result > (int)sizeof(check_port_buf)) {
                        g_warning("Error while completing adb packet");
                        return EXIT_CODE_BAD_SIZE_OF_ASSEMBLED_ADB_PACKET_6;
                    }

                    sock = adb_connect_transport(adb_server_ip, adb_server_tcp_port, serial_number);
                    if (sock == INVALID_SOCKET) continue;

                    response = adb_send_and_read(sock, check_port_buf, helpful_packet, sizeof(helpful_packet), &data_length);
                    closesocket(sock);

                    if (!response) {
                        disable_interface = 1;
                    } else {
                        response[data_length] = '\0';

                        data_str = strchr(response, '\n');
                        if (data_str && sscanf(data_str, "%*s %s", pid) == 1 && strcmp(pid + 9, "10EA") == 0) {
                            g_debug("Bluedroid External Parser Port for %s is %s", serial_number, pid + 9);
                        } else {
                            disable_interface = 1;
                            g_debug("Bluedroid External Parser Port for %s is unknown", serial_number);
                        }
                    }
                } else {
                    disable_interface = 1;
                    g_debug("Android Bluetooth application PID for %s is unknown", serial_number);
                }
            }

            if (!disable_interface) {
                new_interface(extcap_conf, INTERFACE_ANDROID_BLUETOOTH_EXTERNAL_PARSER, model_name, serial_number, "Android Bluetooth External Parser");
            }
        }

        if (api_level >= 21) {
            const char* ps_cmd;
            disable_interface = 0;

            if (api_level >= 24) {
                ps_cmd = adb_ps_with_grep;
            } else if (api_level >= 23) {
                ps_cmd = adb_ps_bluetooth_app;
            }  else {
                ps_cmd = adb_ps_droid_bluetooth;
            }
            sock = adb_connect_transport(adb_server_ip, adb_server_tcp_port, serial_number);
            if (sock == INVALID_SOCKET) continue;

            response = adb_send_and_read(sock, ps_cmd, helpful_packet, sizeof(helpful_packet), &data_length);
            closesocket(sock);

            if (!response || data_length < 1) {
                g_warning("Error while getting Bluetooth application process id by <%s> "
                    "(%p len=%"G_GSSIZE_FORMAT")", adb_hcidump_version, (void*)response, data_length);
                g_debug("Android Bluetooth application PID for %s is unknown", serial_number);
                disable_interface = 1;
            } else {
                char  *data_str;
                char   pid[16];

                memset(pid, 0, sizeof(pid));
                response[data_length] = '\0';

                if (api_level >= 24)
                    data_str = response;
                else
                    data_str = strchr(response, '\n');

                if (data_str && sscanf(data_str, "%*s %s", pid) == 1) {
                    g_debug("Android Bluetooth application PID for %s is %s", serial_number, pid);

                    result = g_snprintf(check_port_buf, sizeof(check_port_buf), adb_check_port_templace, pid);
                    if (result <= 0 || result > (int)sizeof(check_port_buf)) {
                        g_warning("Error while completing adb packet");
                        return EXIT_CODE_BAD_SIZE_OF_ASSEMBLED_ADB_PACKET_9;
                    }

                    sock = adb_connect_transport(adb_server_ip, adb_server_tcp_port, serial_number);
                    if (sock == INVALID_SOCKET) continue;

                    response = adb_send_and_read(sock, check_port_buf, helpful_packet, sizeof(helpful_packet), &data_length);
                    closesocket(sock);

                    if (!response) {
                        disable_interface = 1;
                    } else {
                        response[data_length] = '\0';

                        data_str = strchr(response, '\n');
                        if (data_str && sscanf(data_str, "%*s %s", pid) == 1 && strcmp(pid + 9, "22A8") == 0) {
                            g_debug("Btsnoop Net Port for %s is %s", serial_number, pid + 9);
                        } else {
                            disable_interface = 1;
                            g_debug("Btsnoop Net Port for %s is unknown", serial_number);
                        }
                    }
                } else {
                    disable_interface = 1;
                    g_debug("Android Bluetooth application PID for %s is unknown", serial_number);
                }
            }

            if (!disable_interface) {
                new_interface(extcap_conf, INTERFACE_ANDROID_BLUETOOTH_BTSNOOP_NET, model_name, serial_number, "Android Bluetooth Btsnoop Net");
            }
        }
    }

    return EXIT_CODE_SUCCESS;
}
