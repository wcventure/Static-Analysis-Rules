static int
dissect_capwap_message_element_type(tvbuff_t *tvb, proto_tree *msg_element_type_tree, guint offset, packet_info *pinfo)
{
    guint optlen, offset_end, number_encrypt, i, msg_element_type = 0;
    proto_item *msg_element_type_item, *msg_element_type_item_flag, *ti_len, *ti_type;
    proto_tree *sub_msg_element_type_tree, *sub_msg_element_type_flag_tree;

    msg_element_type = tvb_get_ntohs(tvb, offset);
    optlen = tvb_get_ntohs(tvb, offset+2);
    msg_element_type_item = proto_tree_add_item(msg_element_type_tree, hf_capwap_msg_element, tvb, offset, 2+2+optlen, ENC_NA );

    proto_item_append_text(msg_element_type_item, ": (t=%d,l=%d) %s", msg_element_type, optlen, val_to_str(msg_element_type,message_element_type_vals,"Unknown Message Element Type (%02d)") );

    sub_msg_element_type_tree = proto_item_add_subtree(msg_element_type_item, ett_capwap_message_element_type);

    ti_type = proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type, tvb, offset, 2, ENC_BIG_ENDIAN);

    ti_len = proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_length, tvb, offset+2, 2, ENC_BIG_ENDIAN);

    proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_value, tvb, offset+4, optlen, ENC_NA);

    switch (msg_element_type) {
    case TYPE_AC_DESCRIPTOR: /
        if (optlen < 12) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "AC Descriptor length %u wrong, must be >= 12", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_descriptor_stations, tvb, offset+4, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_descriptor_limit, tvb, offset+6, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_descriptor_active_wtp, tvb, offset+8, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_descriptor_max_wtp, tvb, offset+10, 2, ENC_BIG_ENDIAN);

        /
        proto_tree_add_bitmask_with_flags(sub_msg_element_type_tree, tvb, offset+12,
hf_capwap_msg_element_type_ac_descriptor_security, ett_capwap_ac_descriptor_security_flags, capwap_ac_descriptor_security_flags, ENC_BIG_ENDIAN, BMT_NO_APPEND);

        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_descriptor_rmac_field, tvb, offset+13, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_descriptor_reserved, tvb, offset+14, 1, ENC_BIG_ENDIAN);

        /
        proto_tree_add_bitmask_with_flags(sub_msg_element_type_tree, tvb, offset+15,
hf_capwap_msg_element_type_ac_descriptor_dtls_policy, ett_capwap_ac_descriptor_dtls_flags,  capwap_ac_descriptor_dtls_flags, ENC_BIG_ENDIAN, BMT_NO_APPEND);

        offset_end = offset + optlen -4;
        offset += 4 + 12;
        while (offset < offset_end) {
            offset += dissect_capwap_ac_information(tvb, sub_msg_element_type_tree, offset);
        }
        break;

    case TYPE_AC_IPV4_LIST: /
        if (optlen < 4) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "AC IPv4 List length %u wrong, must be >= 4", optlen);
        break;
        }
        offset_end = offset + 4 + optlen;
        offset += 4;

        if (optlen%4 == 0)
        {
            while (offset_end-offset > 0)
            {
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_ipv4_list, tvb, offset, 4, ENC_BIG_ENDIAN);
                offset += 4;
            }

        }
        break;
    case TYPE_AC_IPV6_LIST: /
        if (optlen < 16) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "AC IPv6 List length %u wrong, must be >= 4", optlen);
        break;
        }
        offset_end = offset + 4 + optlen;
        offset += 4;

        if (optlen%16 == 0)
        {
            while (offset_end-offset > 0)
            {
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_ipv6_list, tvb, offset, 16, ENC_NA);
                offset += 16;
            }

        }
        break;
    case TYPE_AC_NAME: /
        if (optlen < 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "AC Name length %u wrong, must be >= 1", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_name, tvb, offset+4, optlen, ENC_ASCII|ENC_NA);
        break;

    case TYPE_AC_NAME_W_PRIORITY: /
        if (optlen < 2) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "AC Name with Priority length %u wrong, must be >= 2", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_name_with_priority, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_name, tvb, offset+5, optlen-1, ENC_ASCII|ENC_NA);
        break;

    case TYPE_AC_TIMESTAMP: /
        if (optlen != 4) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "AC Timestamp length %u wrong, must be = 4", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ac_timestamp, tvb, offset + 4, 4, ENC_TIME_NTP|ENC_BIG_ENDIAN);
        break;

    case TYPE_ADD_STATION:{ /
        guint8 maclength;
        if (optlen < 8) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Add Station length %u wrong, must be >= 8", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_add_station_radio_id, tvb, offset + 4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_add_station_length, tvb, offset + 5, 1, ENC_BIG_ENDIAN);
        maclength = tvb_get_guint8(tvb, offset+5);
        switch(maclength){
            case 6:
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_add_station_mac_eui48, tvb, offset+6, maclength, ENC_NA);
            break;
            case 8:
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_add_station_mac_eui64, tvb, offset+6, maclength, ENC_BIG_ENDIAN);
            break;
            default:
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_add_station_mac_data, tvb, offset+6, maclength, ENC_NA);
            break;
        }

        if(optlen -(2 + maclength)) {
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_add_station_vlan_name, tvb, offset+6+maclength, optlen -(2 + maclength), ENC_ASCII|ENC_NA);
        }
        }
        break;

    case TYPE_CAPWAP_CONTROL_IPV4_ADDRESS: /
        if (optlen != 6) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "CAPWAP Control IPv4 Address length %u wrong, must be = 6", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_capwap_control_ipv4, tvb, offset+4, 4, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_capwap_control_wtp_count, tvb, offset+8, 2, ENC_BIG_ENDIAN);
        break;

    case TYPE_CAPWAP_CONTROL_IPV6_ADDRESS: /
        if (optlen != 18) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "CAPWAP Control IPv6 Address length %u wrong, must be = 18", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_capwap_control_ipv6, tvb, offset+4, 16, ENC_NA);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_capwap_control_wtp_count, tvb, offset+20, 2, ENC_BIG_ENDIAN);
        break;

    case TYPE_CAPWAP_TIMERS: /
        if (optlen != 2) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "CAPWAP Timers length %u wrong, must be = 2", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_capwap_timers_discovery, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_capwap_timers_echo_request, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        break;

    case TYPE_DECRYPTION_ERROR_REPORT_PERIOD: /
        if (optlen != 3) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Decryption Error Report Period length %u wrong, must be = 3", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_decryption_error_report_period_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree,hf_capwap_msg_element_type_decryption_error_report_period_interval, tvb, offset+5, 2, ENC_BIG_ENDIAN);
        break;

    case TYPE_DELETE_STATION:{ /
        guint8 maclength;
        if (optlen < 8) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Delete Station length %u wrong, must be >= 8", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_delete_station_radio_id, tvb, offset + 4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_delete_station_length, tvb, offset + 5, 1, ENC_BIG_ENDIAN);
        maclength = tvb_get_guint8(tvb, offset+5);
        switch(maclength){
            case 6:
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_delete_station_mac_eui48, tvb, offset+6, maclength, ENC_NA);
            break;
            case 8:
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_delete_station_mac_eui64, tvb, offset+6, maclength, ENC_BIG_ENDIAN);
            break;
            default:
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_delete_station_mac_data, tvb, offset+6, maclength, ENC_NA);
            break;
        }

        }
        break;

    case TYPE_DISCOVERY_TYPE: /
        if (optlen != 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Discovery Type length %u wrong, must be = 1", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_discovery_type, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        break;
    case TYPE_IDLE_TIMEOUT: /
        if (optlen != 4) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Idle Timeout length %u wrong, must be = 4", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_idle_timeout, tvb, offset+4, 4, ENC_BIG_ENDIAN);
        break;

    case TYPE_LOCATION_DATA: /
        if (optlen < 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Location Data length %u wrong, must be >= 1", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_location_data, tvb, offset+4, optlen, ENC_ASCII|ENC_NA);
        break;

    case TYPE_MAXIMUM_MESSAGE_LENGTH: /
        if (optlen != 2) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Maximum Message length %u wrong, must be = 2", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_maximum_message_length, tvb, offset+4, 2, ENC_BIG_ENDIAN);
        break;

    case TYPE_CAPWAP_LOCAL_IPV4_ADDRESS: /
        if (optlen != 4) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "CAPWAP Local IPv4 Address length %u wrong, must be = 4", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_capwap_local_ipv4_address, tvb, offset+4, 4, ENC_BIG_ENDIAN);
        break;


    case TYPE_RADIO_ADMINISTRATIVE_STATE: /
        if (optlen != 2) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Radio Administrative State length %u wrong, must be = 2", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_radio_admin_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_radio_admin_state, tvb, offset+5, 1, ENC_BIG_ENDIAN);

        break;

    case TYPE_RADIO_OPERATIONAL_STATE: /
        if (optlen != 3) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Radio Operational State length %u wrong, must be = 3", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_radio_op_state_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_radio_op_state_radio_state, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_radio_op_state_radio_cause, tvb, offset+6, 1, ENC_BIG_ENDIAN);
        break;

    case TYPE_RESULT_CODE: /
        if (optlen != 4) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Result Code length %u wrong, must be = 4", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_result_code, tvb, offset+4, 4, ENC_BIG_ENDIAN);

        break;

    case TYPE_SESSION_ID: /
        if (optlen != 16) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Session ID length %u wrong, must be = 16", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_session_id, tvb, offset+4, 16, ENC_NA);
        break;

    case TYPE_STATISTICS_TIMER: /
        if (optlen != 2) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Statistics Timer length %u wrong, must be = 2", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_statistics_timer, tvb, offset+4, 2, ENC_BIG_ENDIAN);
        break;

    case TYPE_VENDOR_SPECIFIC_PAYLOAD:{ /
        guint32 vendor_id;
        if (optlen < 7) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "Vendor Specific Payload length %u wrong, must be >= 7", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_vsp_vendor_identifier, tvb, offset+4, 4, ENC_BIG_ENDIAN);
        vendor_id = tvb_get_ntohl(tvb, offset+4);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_vsp_vendor_element_id, tvb, offset+8, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_vsp_vendor_data, tvb, offset+10, optlen-6, ENC_NA);
        switch(vendor_id){
            case VENDOR_FORTINET:
                dissect_capwap_message_element_vendor_fortinet_type(tvb, sub_msg_element_type_tree, offset+8, pinfo, optlen, msg_element_type_item);
            break;
            case VENDOR_CISCO_WIFI:
                dissect_capwap_message_element_vendor_cisco_type(tvb, sub_msg_element_type_tree, offset+8, pinfo, optlen, msg_element_type_item);
            break;
            default:
                /
            break;
        }
        }
        break;

    case TYPE_WTP_BOARD_DATA: /
        if (optlen < 14) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "WTP Board Data length %u wrong, must be >= 14", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_board_data_vendor, tvb, offset+4, 4, ENC_BIG_ENDIAN);
        offset += 8;
        offset_end = offset + optlen -4;
        while (offset < offset_end) {
            offset += dissect_capwap_board_data(tvb, sub_msg_element_type_tree, offset);
        }
        break;

    case TYPE_WTP_DESCRIPTOR: /
        if (optlen < 33) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "WTP Descriptor length %u wrong, must be >= 33", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_descriptor_max_radios, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_descriptor_radio_in_use, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        if (global_capwap_draft_8_cisco == 0)
        {
            number_encrypt = tvb_get_guint8(tvb,offset+6);
            msg_element_type_item_flag = proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_descriptor_number_encrypt, tvb, offset+6, 1, ENC_BIG_ENDIAN);
            sub_msg_element_type_flag_tree = proto_item_add_subtree(msg_element_type_item_flag, ett_capwap_encryption_capabilities);
            for (i=0; i < number_encrypt; i++) {
                dissect_capwap_encryption_capabilities(tvb, sub_msg_element_type_flag_tree, offset+4+3+i*3);
            }
            offset_end = offset + optlen -4;
            offset += 4 + 3 + number_encrypt * 3;
        }
        else
        {
            /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_descriptor_encrypt_capabilities, tvb, offset+6, 2, ENC_BIG_ENDIAN);
            offset_end = offset + optlen -4;
            offset += 6 + 2;
        }
        while (offset < offset_end) {
            offset += dissect_capwap_wtp_descriptor(tvb, sub_msg_element_type_tree, offset);
        }
        break;

    case TYPE_WTP_FALLBACK: /
        if (optlen != 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "WTP Fallback length %u wrong, must be = 1", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_fallback, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        break;

    case TYPE_WTP_FRAME_TUNNEL_MODE: /
        if (optlen != 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "WTP Frame Tunnel Mode length %u wrong, must be = 1", optlen);
        break;
        }

        proto_tree_add_bitmask_with_flags(sub_msg_element_type_tree, tvb, offset+4,
hf_capwap_msg_element_type_wtp_frame_tunnel_mode, ett_capwap_wtp_frame_tunnel_mode, capwap_wtp_frame_tunnel_mode_flags, ENC_BIG_ENDIAN, BMT_NO_APPEND);
        break;
    case TYPE_WTP_MAC_TYPE: /
        if (optlen != 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "WTP MAC Type length %u wrong, must be = 1", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_mac_type, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        break;

    case TYPE_WTP_NAME: /
        if (optlen < 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "WTP Name length %u wrong, must be >= 1", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_name, tvb, offset+4, optlen, ENC_ASCII|ENC_NA);
        break;

    case TYPE_WTP_REBOOT_STATISTICS: /
        if (optlen != 15) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "WTP Reboot Statistics length %u wrong, must be = 15", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_reboot_statistics_reboot_count, tvb, offset+4, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_reboot_statistics_ac_initiated_count, tvb, offset+6, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_reboot_statistics_link_failure_count, tvb, offset+8, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_reboot_statistics_sw_failure_count, tvb, offset+10, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_reboot_statistics_hw_failure_count, tvb, offset+12, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_reboot_statistics_other_failure_count, tvb, offset+14, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_reboot_statistics_unknown_failure_count, tvb, offset+16, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_wtp_reboot_statistics_last_failure_type, tvb, offset+18, 1, ENC_BIG_ENDIAN);
        break;

    case TYPE_CAPWAP_LOCAL_IPV6_ADDRESS: /
        if (optlen != 16) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "CAPWAP Local IPv6 Address length %u wrong, must be = 16", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_capwap_local_ipv6_address, tvb, offset+4, 16, ENC_NA);
        break;

    case TYPE_CAPWAP_TRANSPORT_PROTOCOL: /
        if (optlen != 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "CAPWAP Transport Protocol length %u wrong, must be = 1", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_capwap_transport_protocol, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        break;

    case TYPE_MTU_DISCOVERY_PADDING: /
        if (optlen < 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "MTU Discovery Padding length %u wrong, must be >= 1", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_mtu_discovery_padding, tvb, offset+4, optlen, ENC_NA);
        break;


    case TYPE_ECN_SUPPORT: /
        if (optlen != 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "ECN Support length %u wrong, must be = 1", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ecn_support, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        break;

    case IEEE80211_ADD_WLAN:{ /
        guint16 key_length;
        if (optlen < 20) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Add Wlan length %u wrong, must be >= 20", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_wlan_id, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_bitmask_with_flags(sub_msg_element_type_tree, tvb, offset+6,
hf_capwap_msg_element_type_ieee80211_add_wlan_capability, ett_capwap_ieee80211_add_wlan_capability, ieee80211_add_wlan_capability_flags, ENC_BIG_ENDIAN, BMT_NO_APPEND);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_key_index, tvb, offset+8, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_key_status, tvb, offset+9, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_key_length, tvb, offset+10, 2, ENC_BIG_ENDIAN);
        key_length = tvb_get_ntohs(tvb, offset+10);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_key, tvb, offset+12, key_length, ENC_NA);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_group_tsc, tvb, offset+key_length+12, 6, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_qos, tvb, offset+key_length+18, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_auth_type, tvb, offset+key_length+19, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_mac_mode, tvb, offset+key_length+20, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_tunnel_mode, tvb, offset+key_length+21, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_suppress_ssid, tvb, offset+key_length+22, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_add_wlan_ssid, tvb, offset+key_length+23, optlen-(key_length+23-4), ENC_ASCII|ENC_NA);
        }
        break;

    case IEEE80211_ANTENNA:{ /
        guint8 antenna_count, antenna = 0;
        if (optlen < 5) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Antenna length %u wrong, must be >= 5", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_antenna_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_antenna_diversity, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_antenna_combiner, tvb, offset+6, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_antenna_count, tvb, offset+7, 1, ENC_BIG_ENDIAN);
        antenna_count = tvb_get_guint8(tvb, offset+7);
        while(antenna < antenna_count){
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_antenna_selection, tvb, offset+8+antenna, 1, ENC_BIG_ENDIAN);
            antenna += 1;
        }
    }
    break;

    case IEEE80211_ASSIGNED_WTP_BSSID: /
        if (optlen != 8) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Assigned WTP BSSID length %u wrong, must be = 8", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_assigned_wtp_bssid_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_assigned_wtp_bssid_wlan_id, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_assigned_wtp_bssid_bssid, tvb, offset+6, 6, ENC_NA);
        break;

    case IEEE80211_DELETE_WLAN: /
        if (optlen != 2) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Delete Wlan length %u wrong, must be = 2", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_delete_wlan_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_delete_wlan_wlan_id, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        break;

    case IEEE80211_DIRECT_SEQUENCE_CONTROL: /
        if (optlen != 8) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Direct Sequence Control length %u wrong, must be = 8", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_direct_sequence_control_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_direct_sequence_control_reserved, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_direct_sequence_control_current_channel, tvb, offset+6, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_direct_sequence_control_current_cca, tvb, offset+7, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_direct_sequence_control_energy_detect_threshold, tvb, offset+8, 4, ENC_BIG_ENDIAN);
        break;

    case IEEE80211_INFORMATION_ELEMENT: /
        if (optlen < 4) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Information Element length %u wrong, must be >= 4", optlen);
        break;
        }
        offset += 4;
        offset_end = offset + optlen;

        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_ie_radio_id, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_ie_wlan_id, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

        proto_tree_add_bitmask_with_flags(sub_msg_element_type_tree, tvb, offset,
hf_capwap_msg_element_type_ieee80211_ie_flags, ett_capwap_ieee80211_ie_flags, ieee80211_ie_flags, ENC_BIG_ENDIAN, BMT_NO_APPEND);
        offset += 1;

        while (offset < offset_end) {
            offset += add_tagged_field(pinfo, sub_msg_element_type_tree, tvb, offset, 0);
        }

        break;

    case IEEE80211_MAC_OPERATION: /
        if (optlen != 16) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 MAC Operation length %u wrong, must be = 16", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mac_operation_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mac_operation_reserved, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mac_operation_rts_threshold, tvb, offset+6, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mac_operation_short_retry, tvb, offset+8, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mac_operation_long_retry, tvb, offset+9, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mac_operation_fragmentation_threshold, tvb, offset+10, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mac_operation_tx_msdu_lifetime, tvb, offset+12, 4, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mac_operation_rx_msdu_lifetime, tvb, offset+16, 4, ENC_BIG_ENDIAN);
        break;

    case IEEE80211_MIC_COUNTERMEASURES: /
        if (optlen != 8) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 MIC Countermeasures length %u wrong, must be = 8", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mic_countermeasures_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mic_countermeasures_wlan_id, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mic_countermeasures_mac_address, tvb, offset+6, 6, ENC_NA);
        break;

    case IEEE80211_MULTI_DOMAIN_CAPABILITY: /
        if (optlen != 8) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Multi-Domain Capability length %u wrong, must be = 8", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_multi_domain_capability_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_multi_domain_capability_reserved, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_multi_domain_capability_first_channel, tvb, offset+6, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_multi_domain_capability_number_of_channels, tvb, offset+8, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_multi_domain_capability_max_tx_power_level, tvb, offset+10, 2, ENC_BIG_ENDIAN);
        break;

    case IEEE80211_OFDM_CONTROL: /
        if (optlen != 8) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 OFDM Control length %u wrong, must be = 8", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_ofdm_control_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_ofdm_control_reserved, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_ofdm_control_current_channel, tvb, offset+6, 1, ENC_BIG_ENDIAN);
        proto_tree_add_bitmask_with_flags(sub_msg_element_type_tree, tvb, offset+7, hf_capwap_msg_element_type_ieee80211_ofdm_control_band_support, ett_capwap_ieee80211_ofdm_control_band_support, ieee80211_ofdm_control_band_support_flags, ENC_BIG_ENDIAN, BMT_NO_APPEND);

        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_ofdm_control_ti_threshold, tvb, offset+8, 4, ENC_BIG_ENDIAN);
        break;

    case IEEE80211_RATE_SET: /
        if (optlen < 3) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Rate Set length %u wrong, must be >= 3", optlen);
        break;
        }
        offset += 4;
        offset_end = offset + optlen;

        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_rate_set_radio_id, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

        while (offset < offset_end) {
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_rate_set_rate_set, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        }


        break;
    case IEEE80211_STATION: /
        if (optlen < 14) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Station length %u wrong, must be >= 14", optlen);
        break;
        }
        offset_end = offset + 4 + optlen;
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_association_id, tvb, offset+5, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_flags, tvb, offset+7, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_mac_address, tvb, offset+8, 6, ENC_NA);
        proto_tree_add_bitmask_with_flags(sub_msg_element_type_tree, tvb, offset+14,
hf_capwap_msg_element_type_ieee80211_station_capabilities, ett_capwap_ieee80211_station_capabilities, ieee80211_station_capabilities_flags, ENC_BIG_ENDIAN, BMT_NO_APPEND);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_wlan_id, tvb, offset+16, 1, ENC_BIG_ENDIAN);

        offset += 17;
        while (offset < offset_end) {
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_supported_rates, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        }

        break;

    case IEEE80211_STATION_SESSION_KEY: /
        if (optlen < 25) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Station Session Key length %u wrong, must be >= 25", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_session_key_mac, tvb, offset+4, 6, ENC_NA);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_session_key_flags, tvb, offset+10, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_session_key_flags_a, tvb, offset+10, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_session_key_flags_c, tvb, offset+10, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_session_key_pairwire_tsc, tvb, offset+12, 6, ENC_NA);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_session_key_pairwire_rsc, tvb, offset+18, 6, ENC_NA);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_station_session_key_key, tvb, offset+24, optlen-24, ENC_NA);
        break;

    case IEEE80211_SUPPORTED_RATES: /
        if (optlen < 3) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Supported Rates length %u wrong, must be >= 3", optlen);
        break;
        }
        offset += 4;
        offset_end = offset + optlen;

        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_supported_rates_radio_id, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

        while (offset < offset_end) {
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_supported_rates_rate, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        }
        break;

    case IEEE80211_TX_POWER: /
        if (optlen != 4) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Tx Power length %u wrong, must be = 4", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_tx_power_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_tx_power_reserved, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_tx_power_current_tx_power, tvb, offset+6, 2, ENC_BIG_ENDIAN);
        break;

    case IEEE80211_TX_POWER_LEVEL:{ /
        guint8 num_levels, level = 0;
        if (optlen < 3) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Antenna length %u wrong, must be >= 3", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_tx_power_level_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_tx_power_level_num_levels, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        num_levels = tvb_get_guint8(tvb, offset+5);
        while(level < num_levels){
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_tx_power_level_power_level, tvb, offset+6+(level*2), 2, ENC_BIG_ENDIAN);
            level += 1;
        }
        }
        break;

    case IEEE80211_UPDATE_WLAN:{ /
        guint16 key_length;
        if (optlen < 8) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Update Wlan length %u wrong, must be >= 8", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_update_wlan_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_update_wlan_wlan_id, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_bitmask_with_flags(sub_msg_element_type_tree, tvb, offset+6,
hf_capwap_msg_element_type_ieee80211_update_wlan_capability, ett_capwap_ieee80211_update_wlan_capability, ieee80211_update_wlan_capability_flags, ENC_BIG_ENDIAN, BMT_NO_APPEND);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_update_wlan_key_index, tvb, offset+8, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_update_wlan_key_status, tvb, offset+9, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_update_wlan_key_length, tvb, offset+10, 2, ENC_BIG_ENDIAN);
        key_length = tvb_get_ntohs(tvb, offset+10);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_update_wlan_key, tvb, offset+12, key_length, ENC_NA);
        }
        break;

    case IEEE80211_WTP_RADIO_CONFIGURATION: /
        if (optlen != 16) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 WTP Radio Configuration length %u wrong, must be = 16", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_cfg_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_cfg_short_preamble, tvb, offset+5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_cfg_num_of_bssids, tvb, offset+6, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_cfg_dtim_period, tvb, offset+7, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_cfg_bssid, tvb, offset+8, 6, ENC_NA);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_cfg_beacon_period, tvb, offset+14, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_cfg_country_string, tvb, offset+16, 4, ENC_ASCII|ENC_NA);
        break;

    case IEEE80211_WTP_RADIO_INFORMATION: /
        if (optlen != 5) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 WTP Radio Information length %u wrong, must be = 5", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_info_radio_id, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_info_radio_type_reserved, tvb, offset+5, 3, ENC_NA);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_info_radio_type_n, tvb, offset+8, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_info_radio_type_g, tvb, offset+8, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_info_radio_type_a, tvb, offset+8, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_wtp_radio_info_radio_type_b, tvb, offset+8, 1, ENC_BIG_ENDIAN);
        break;



    case IEEE80211_SUPPORTED_MAC_PROFILES:{ /
        guint8 num_profiles;
        if (optlen < 2) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 Supported MAC Profiles length %u wrong, must be >= 2", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_supported_mac_profiles_numbers, tvb, offset+4, 1, ENC_BIG_ENDIAN);
        num_profiles = tvb_get_guint8(tvb ,offset);
        while(num_profiles){
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_supported_mac_profiles_profile, tvb, offset+5, 1, ENC_BIG_ENDIAN);
            offset += 1;
            num_profiles--;
        }
        }
        break;

    case IEEE80211_MAC_PROFILE: /
        if (optlen != 1) {
            expert_add_info_format(pinfo, ti_len, &ei_capwap_msg_element_length,
                           "IEEE80211 MAC Profile length %u wrong, must be = 1", optlen);
        break;
        }
        proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_msg_element_type_ieee80211_mac_profile, tvb, offset+4, 1, ENC_BIG_ENDIAN);

        break;

    default:
      expert_add_info_format(pinfo, ti_type, &ei_capwap_message_element_type,
                             "Dissector for CAPWAP Message Element"
                             " (%s) type not implemented, Contact"
                             " Wireshark developers if you want this supported",
                             val_to_str(msg_element_type, message_element_type_vals, "(%d)"));
        break;
    }

    return 2+2+optlen;
}
