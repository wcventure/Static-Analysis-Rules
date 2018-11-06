static int add_android_interfaces(struct interface_t **interface_list,
        const char *adb_server_ip, unsigned short *adb_server_tcp_port)
{
    static char            packet[PACKET_LENGTH];
    static char            helpful_packet[PACKET_LENGTH];
    char                  *response;
    char                  *device_list;
    ssize_t                data_length;
    ssize_t                device_length;
    socket_handle_t        sock;
    const char            *adb_transport_serial_templace = "%04x""host:transport:%s";
    const char            *adb_check_port_templace       = "%04x""shell:cat /proc/%s/net/tcp";
    const char            *adb_devices            = "000C""host:devices";
    const char            *adb_api_level          = "0022""shell:getprop ro.build.version.sdk";
    const char            *adb_hcidump_version    = "0017""shell:hcidump --version";
    const char            *adb_ps_droid_bluetooth = "0018""shell:ps droid.bluetooth";
    char                   serial_number[512];
    int                    result;
    char                  *interface_name;
    char                  *pos;
    char                  *prev_pos;
    struct interface_t    *i_interface_list;
    int                    api_level;
    int                    disable_interface;

/

    i_interface_list = *interface_list;

    sock = adb_connect(adb_server_ip, adb_server_tcp_port);
    if (sock == INVALID_SOCKET)
        return -1;

    device_list = adb_send_and_receive(sock, adb_devices, packet, sizeof(packet), &device_length);
    closesocket(sock);

    device_list[device_length] = '\0';
    pos = (char *) device_list;

    while (pos < (char *) (device_list + device_length)) {
        prev_pos = pos;
        pos = strchr(pos, '\t');
        result = (int) (pos - prev_pos);
        pos = strchr(pos, '\n') + 1;
        if (result > (int) sizeof(serial_number)) {
            fprintf(stderr, "WARNING: Serial number too long, ignore device\n");
            continue;
        }
        memcpy(serial_number, prev_pos, result);
        serial_number[result] = '\0';

        sock = adb_connect(adb_server_ip, adb_server_tcp_port);

        sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + strlen(serial_number), serial_number);
        result = adb_send(sock, helpful_packet);
        if (result) {
            fprintf(stderr, "ERROR: Error while setting adb transport for <%s>\n", helpful_packet);
            return 1;
        }

        response = adb_send_and_read(sock, adb_api_level, helpful_packet, sizeof(helpful_packet), &data_length);
        closesocket(sock);
        response[data_length] = '\0';
        api_level = (int) strtol(response, NULL, 10);
        fprintf(stderr, "VERBOSE: Android API Level for %s is %u\n", serial_number, api_level);

        if (api_level < 21) {
            interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_LOGCAT_MAIN) + 1 + strlen(serial_number) + 1);
            interface_name[0]= '\0';
            strcat(interface_name, INTERFACE_ANDROID_LOGCAT_MAIN);
            strcat(interface_name, "-");
            strcat(interface_name, serial_number);
            if (*interface_list == NULL) {
                i_interface_list = (struct interface_t *) malloc(sizeof(struct interface_t));
                *interface_list = i_interface_list;
            } else {
                i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
                i_interface_list = i_interface_list->next;
            }
            i_interface_list->display_name = "Android Logcat Main";
            i_interface_list->interface_name = interface_name;
            i_interface_list->next = NULL;


            interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_LOGCAT_SYSTEM) + 1 + strlen(serial_number) + 1);
            interface_name[0]= '\0';
            strcat(interface_name, INTERFACE_ANDROID_LOGCAT_SYSTEM);
            strcat(interface_name, "-");
            strcat(interface_name, serial_number);
            i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
            i_interface_list = i_interface_list->next;
            i_interface_list->display_name = "Android Logcat System";
            i_interface_list->interface_name = interface_name;
            i_interface_list->next = NULL;

            interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_LOGCAT_RADIO) + 1 + strlen(serial_number) + 1);
            interface_name[0]= '\0';
            strcat(interface_name, INTERFACE_ANDROID_LOGCAT_RADIO);
            strcat(interface_name, "-");
            strcat(interface_name, serial_number);
            i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
            i_interface_list = i_interface_list->next;
            i_interface_list->display_name = "Android Logcat Radio";
            i_interface_list->interface_name = interface_name;
            i_interface_list->next = NULL;

            interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_LOGCAT_EVENTS) + 1 + strlen(serial_number) + 1);
            interface_name[0]= '\0';
            strcat(interface_name, INTERFACE_ANDROID_LOGCAT_EVENTS);
            strcat(interface_name, "-");
            strcat(interface_name, serial_number);
            i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
            i_interface_list = i_interface_list->next;
            i_interface_list->display_name = "Android Logcat Events";
            i_interface_list->interface_name = interface_name;
            i_interface_list->next = NULL;
        } else {
            interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_LOGCAT_TEXT_MAIN) + 1 + strlen(serial_number) + 1);
            interface_name[0]= '\0';
            strcat(interface_name, INTERFACE_ANDROID_LOGCAT_TEXT_MAIN);
            strcat(interface_name, "-");
            strcat(interface_name, serial_number);
            if (*interface_list == NULL) {
                i_interface_list = (struct interface_t *) malloc(sizeof(struct interface_t));
                *interface_list = i_interface_list;
            } else {
                i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
                i_interface_list = i_interface_list->next;
            }
            i_interface_list->display_name = "Android Logcat Main";
            i_interface_list->interface_name = interface_name;
            i_interface_list->next = NULL;


            interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_LOGCAT_TEXT_SYSTEM) + 1 + strlen(serial_number) + 1);
            interface_name[0]= '\0';
            strcat(interface_name, INTERFACE_ANDROID_LOGCAT_TEXT_SYSTEM);
            strcat(interface_name, "-");
            strcat(interface_name, serial_number);
            i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
            i_interface_list = i_interface_list->next;
            i_interface_list->display_name = "Android Logcat System";
            i_interface_list->interface_name = interface_name;
            i_interface_list->next = NULL;

            interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_LOGCAT_TEXT_RADIO) + 1 + strlen(serial_number) + 1);
            interface_name[0]= '\0';
            strcat(interface_name, INTERFACE_ANDROID_LOGCAT_TEXT_RADIO);
            strcat(interface_name, "-");
            strcat(interface_name, serial_number);
            i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
            i_interface_list = i_interface_list->next;
            i_interface_list->display_name = "Android Logcat Radio";
            i_interface_list->interface_name = interface_name;
            i_interface_list->next = NULL;

            interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_LOGCAT_TEXT_EVENTS) + 1 + strlen(serial_number) + 1);
            interface_name[0]= '\0';
            strcat(interface_name, INTERFACE_ANDROID_LOGCAT_TEXT_EVENTS);
            strcat(interface_name, "-");
            strcat(interface_name, serial_number);
            i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
            i_interface_list = i_interface_list->next;
            i_interface_list->display_name = "Android Logcat Events";
            i_interface_list->interface_name = interface_name;
            i_interface_list->next = NULL;

            interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_LOGCAT_TEXT_CRASH) + 1 + strlen(serial_number) + 1);
            interface_name[0]= '\0';
            strcat(interface_name, INTERFACE_ANDROID_LOGCAT_TEXT_CRASH);
            strcat(interface_name, "-");
            strcat(interface_name, serial_number);
            i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
            i_interface_list = i_interface_list->next;
            i_interface_list->display_name = "Android Logcat Crash";
            i_interface_list->interface_name = interface_name;
            i_interface_list->next = NULL;
        }

        if (api_level >= 5 && api_level < 17) {
            disable_interface = 0;

            sock = adb_connect(adb_server_ip, adb_server_tcp_port);

            sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + strlen(serial_number), serial_number);
            result = adb_send(sock, helpful_packet);
            if (result) {
                fprintf(stderr, "ERROR: Error while setting adb transport for <%s>\n", helpful_packet);
                return 1;
            }

            response = adb_send_and_read(sock, adb_hcidump_version, helpful_packet, sizeof(helpful_packet), &data_length);
            closesocket(sock);
            if (!response || data_length < 1) {
                fprintf(stderr, "WARNING: Error while getting hcidump version by <%s> (%p len=%"G_GSSIZE_FORMAT")\n", adb_hcidump_version, response, data_length);
                fprintf(stderr, "VERBOSE: Android hcidump version for %s is unknown\n", serial_number);
                disable_interface = 1;
            } else {
                response[data_length] = '\0';

                if (strtoul(response, NULL, 10) == 0) {
                    fprintf(stderr, "VERBOSE: Android hcidump version for %s is unknown\n", serial_number);
                    disable_interface = 1;
                } else {
                    fprintf(stderr, "VERBOSE: Android hcidump version for %s is %s\n", serial_number, response);
                }
            }

            if (!disable_interface) {
                interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_BLUETOOTH_HCIDUMP) + 1 + strlen(serial_number) + 1);
                interface_name[0]= '\0';
                strcat(interface_name, INTERFACE_ANDROID_BLUETOOTH_HCIDUMP);
                strcat(interface_name, "-");
                strcat(interface_name, serial_number);
                i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
                i_interface_list = i_interface_list->next;
                i_interface_list->display_name = "Android Bluetooth Hcidump";
                i_interface_list->interface_name = interface_name;
                i_interface_list->next = NULL;
            }
        }

        if (api_level >= 17 && api_level < 21) {
            disable_interface = 0;
            sock = adb_connect(adb_server_ip, adb_server_tcp_port);

            sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + strlen(serial_number), serial_number);
            result = adb_send(sock, helpful_packet);
            if (result) {
                fprintf(stderr, "ERROR: Error while setting adb transport for <%s>\n", helpful_packet);
                return 1;
            }


            response = adb_send_and_read(sock, adb_ps_droid_bluetooth, helpful_packet, sizeof(helpful_packet), &data_length);
            closesocket(sock);
            if (!response || data_length < 1) {
                fprintf(stderr, "WARNING: Error while getting Bluetooth application process id by <%s> (%p len=%"G_GSSIZE_FORMAT")\n", adb_hcidump_version, response, data_length);
                fprintf(stderr, "VERBOSE: Android Bluetooth application PID for %s is unknown\n", serial_number);
                disable_interface = 1;
            } else {
                char  *data_str;
                char   pid[16];

                response[data_length] = '\0';

                data_str = strchr(response, '\n');
                if (data_str && sscanf(data_str, "%*s %s", pid) == 1) {
                    fprintf(stderr, "VERBOSE: Android Bluetooth application PID for %s is %s\n", serial_number, pid);

                    sock = adb_connect(adb_server_ip, adb_server_tcp_port);

                    sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + strlen(serial_number), serial_number);
                    result = adb_send(sock, helpful_packet);
                    if (result) {
                        fprintf(stderr, "ERROR: Error while setting adb transport for <%s>\n", helpful_packet);
                        return 1;
                    }

                    sprintf((char *) helpful_packet, adb_check_port_templace, strlen(adb_check_port_templace) - 6 + strlen(pid), pid);
                    response = adb_send_and_read(sock, helpful_packet, helpful_packet, sizeof(helpful_packet), &data_length);
                    response[data_length] = '\0';
                    closesocket(sock);

                    data_str = strchr(response, '\n');
                    if (data_str && sscanf(data_str, "%*s %s", pid) == 1 && strcmp(pid + 9, "10EA") == 0) {
                        fprintf(stderr, "VERBOSE: Bluedroid External Parser Port for %s is %s\n", serial_number, pid + 9);
                    } else {
                        disable_interface = 1;
                        fprintf(stderr, "VERBOSE: Bluedroid External Parser Port for %s is unknown\n", serial_number);
                    }
                } else {
                    disable_interface = 1;
                    fprintf(stderr, "VERBOSE: Android Bluetooth application PID for %s is unknown\n", serial_number);
                }
            }

            if (!disable_interface) {
                interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_BLUETOOTH_EXTERNAL_PARSER) + 1 + strlen(serial_number) + 1);
                interface_name[0]= '\0';
                strcat(interface_name, INTERFACE_ANDROID_BLUETOOTH_EXTERNAL_PARSER);
                strcat(interface_name, "-");
                strcat(interface_name, serial_number);
                i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
                i_interface_list = i_interface_list->next;
                i_interface_list->display_name = "Android Bluetooth External Parser";
                i_interface_list->interface_name = interface_name;
                i_interface_list->next = NULL;
            }
        }

        if (api_level >= 21) {
            disable_interface = 0;
            sock = adb_connect(adb_server_ip, adb_server_tcp_port);

            sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + strlen(serial_number), serial_number);
            result = adb_send(sock, helpful_packet);
            if (result) {
                fprintf(stderr, "ERROR: Error while setting adb transport for <%s>\n", helpful_packet);
                return 1;
            }


            response = adb_send_and_read(sock, adb_ps_droid_bluetooth, helpful_packet, sizeof(helpful_packet), &data_length);
            closesocket(sock);
            if (!response || data_length < 1) {
                fprintf(stderr, "WARNING: Error while getting Bluetooth application process id by <%s> (%p len=%"G_GSSIZE_FORMAT")\n", adb_hcidump_version, response, data_length);
                fprintf(stderr, "VERBOSE: Android Bluetooth application PID for %s is unknown\n", serial_number);
                disable_interface = 1;
            } else {
                char  *data_str;
                char   pid[16];

                response[data_length] = '\0';

                data_str = strchr(response, '\n');
                if (data_str && sscanf(data_str, "%*s %s", pid) == 1) {
                    fprintf(stderr, "VERBOSE: Android Bluetooth application PID for %s is %s\n", serial_number, pid);

                    sock = adb_connect(adb_server_ip, adb_server_tcp_port);

                    sprintf((char *) helpful_packet, adb_transport_serial_templace, 15 + strlen(serial_number), serial_number);
                    result = adb_send(sock, helpful_packet);
                    if (result) {
                        fprintf(stderr, "ERROR: Error while setting adb transport for <%s>\n", helpful_packet);
                        return 1;
                    }

                    sprintf((char *) helpful_packet, adb_check_port_templace, strlen(adb_check_port_templace) - 6 + strlen(pid), pid);
                    response = adb_send_and_read(sock, helpful_packet, helpful_packet, sizeof(helpful_packet), &data_length);
                    response[data_length] = '\0';
                    closesocket(sock);

                    data_str = strchr(response, '\n');
                    if (data_str && sscanf(data_str, "%*s %s", pid) == 1 && strcmp(pid + 9, "22A8") == 0) {
                        fprintf(stderr, "VERBOSE: Btsnoop Net Port for %s is %s\n", serial_number, pid + 9);
                    } else {
                        disable_interface = 1;
                        fprintf(stderr, "VERBOSE: Btsnoop Net Port for %s is unknown\n", serial_number);
                    }
                } else {
                    disable_interface = 1;
                    fprintf(stderr, "VERBOSE: Android Bluetooth application PID for %s is unknown\n", serial_number);
                }
            }

            if (!disable_interface) {
                interface_name = (char *) malloc(strlen(INTERFACE_ANDROID_BLUETOOTH_BTSNOOP_NET) + 1 + strlen(serial_number) + 1);
                interface_name[0]= '\0';
                strcat(interface_name, INTERFACE_ANDROID_BLUETOOTH_BTSNOOP_NET);
                strcat(interface_name, "-");
                strcat(interface_name, serial_number);
                i_interface_list->next = (struct interface_t *) malloc(sizeof(struct interface_t));
                i_interface_list = i_interface_list->next;
                i_interface_list->display_name = "Android Bluetooth Btsnoop Net";
                i_interface_list->interface_name = interface_name;
                i_interface_list->next = NULL;
            }
        }
    }

    return 0;
}
