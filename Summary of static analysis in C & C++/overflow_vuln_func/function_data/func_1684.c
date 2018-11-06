static int
dissect_capwap_message_element_vendor_fortinet_type(tvbuff_t *tvb, proto_tree *sub_msg_element_type_tree, guint offset, packet_info *pinfo _U_, guint optlen,  proto_item *msg_element_type_item)
{
    guint element_id;

    proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_element_id, tvb, offset, 2, ENC_BIG_ENDIAN);
    element_id = tvb_get_ntohs(tvb, offset);
    proto_item_append_text(msg_element_type_item, ": Fortinet %s", val_to_str(element_id, fortinet_element_id_vals,"Unknown Vendor Specific Element Type (%02d)") );
    offset += 2;

    /
    optlen -= 6;
    proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_value, tvb, offset, optlen, ENC_NA);

    switch(element_id){
        case VSP_FORTINET_AP_SCAN: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ap_scan_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ap_scan_bgscan_intv, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ap_scan_bgscan_idle, tvb, offset, 3, ENC_BIG_ENDIAN);
            offset += 3;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ap_scan_bgscan_rpt_intv, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ap_scan_fgscan_rpt_intv, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
        break;
        case VSP_FORTINET_PASSIVE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_passive_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_passive, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_DAEMON_RST: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_daemon_rst, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_MAC:{ /
            guint8 mac_length;
            proto_item *ti;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mac_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mac_wid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            ti =proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mac_len, tvb, offset, 1, ENC_BIG_ENDIAN);
            mac_length = tvb_get_guint8(tvb, offset);
            offset += 1;
            if(mac_length %6 != 0)
            {
                expert_add_info(pinfo, ti, &ei_capwap_fortinet_mac_len );
                break;
            }
            for(;mac_length > 0; mac_length -= 6){
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mac, tvb, offset, 6, ENC_NA);
                offset += 6;
            }
            }
        break;
        case VSP_FORTINET_WTP_ALLOW: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wtp_allow_sn, tvb, offset, optlen-1, ENC_ASCII|ENC_NA);
            offset += optlen - 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wtp_allow_allow, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_WBH_STA:{ /
            guint16 mac_length;
            proto_item *ti;

            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wbh_sta_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            ti = proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wbh_sta_len, tvb, offset, 1, ENC_BIG_ENDIAN);
            mac_length = tvb_get_guint8(tvb, offset);
            offset += 1;
            if(mac_length %6 != 0)
            {
                expert_add_info(pinfo, ti, &ei_capwap_fortinet_mac_len );
                break;
            }
            for(;mac_length > 0; mac_length -= 6){
                proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wbh_sta_mac, tvb, offset, 6, ENC_NA);
                offset += 6;
            }
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wbh_sta_bssid, tvb, offset, 6, ENC_NA);
            offset += 6;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wbh_sta_mhc, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            }
        break;
        case VSP_FORTINET_HTCAP: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_htcap_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_htcap_mcs, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_htcap_gi, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_htcap_bw, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_MGMT_VAP:{ /
            guint16 sn_length;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mvap_sn_length, tvb, offset, 2, ENC_NA);
            sn_length = tvb_get_ntohs(tvb, offset);
            offset += 2;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mvap_sn, tvb, offset, sn_length, ENC_ASCII|ENC_NA);
            offset += sn_length;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mvap_unknown, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mvap_unknown, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mvap_age, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mvap_period, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mvap_vfid, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
        break;
        }
        case VSP_FORTINET_MODE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mode_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mode, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_COEXT: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_coext_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_coext, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_AMSDU: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_amsdu_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_amsdu, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_PS_OPT: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ps_opt_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ps_opt, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_PURE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_pure_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_pure, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_EBP_TAG: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ebptag_ebp, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ebptag_tag, tvb, offset, 6, ENC_NA);
            offset += 6;
        break;
        case VSP_FORTINET_TELNET_ENABLE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_telnet_enable, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
        break;
        case VSP_FORTINET_ADMIN_PASSWD: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_admin_passwd, tvb, offset, optlen, ENC_ASCII|ENC_NA);
            offset += optlen;
        break;
        case VSP_FORTINET_REGCODE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_regcode, tvb, offset, 3, ENC_ASCII|ENC_NA);
            offset += 3;
        break;
        case VSP_FORTINET_COUNTRYCODE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_countrycode_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_countrycode_code, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_countrycode_string, tvb, offset, 3, ENC_ASCII|ENC_NA);
            offset += 3;
        break;
        case VSP_FORTINET_STA_SCAN: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_sta_scan_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_sta_scan, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
        break;
        case VSP_FORTINET_FHO: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_fho_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_fho, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_APHO: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_apho_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_apho, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_STA_LOCATE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_sta_locate_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_sta_locate_enable, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_sta_locate_interval, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
        break;
        case VSP_FORTINET_SPECTRUM_ANALYSIS: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_sa_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            optlen -= 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_sa_enable, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            optlen -= 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_sa_ssid, tvb, offset, optlen, ENC_ASCII|ENC_NA);
            offset += optlen;
        break;
        case VSP_FORTINET_DARRP_CFG: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_darrp_cfg_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_darrp_cfg_enable, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_darrp_cfg_interval, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_AP_SUPPRESS_LIST: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ap_suppress_list_ver, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ap_suppress_list_op, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ap_suppress_list_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_ap_suppress_list_len, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_WDS: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wds_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wds_wid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wds_enable, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_VAP_VLAN_TAG: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_vap_vlan_tag_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_vap_vlan_tag_wid, tvb, offset, 1, ENC_BIG_ENDIAN);
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_vap_vlan_tag, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
        break;
        case VSP_FORTINET_VAP_BITMAP: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_vap_bitmap_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_vap_bitmap, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
        break;
        case VSP_FORTINET_MCAST_RATE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mcast_rate_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mcast_rate_wid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mcast_rate, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
        break;
        case VSP_FORTINET_CFG: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_cfg_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_cfg_wid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_cfg_ip, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_cfg_mask, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
        break;
        case VSP_FORTINET_SPLIT_TUN_CFG: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_split_tun_cfg_enable_local_subnet, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_split_tun_cfg_cnt, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_MGMT_VLAN_TAG: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mgmt_vlan_id, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
        break;
        case VSP_FORTINET_VAP_PSK_PASSWD: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_vap_psk_passwd_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            optlen -= 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_vap_psk_passwd_wid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            optlen -= 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_vap_psk_passwd_key, tvb, offset, optlen, ENC_ASCII|ENC_NA);
            offset += optlen;
        break;
        case VSP_FORTINET_MESH_ETH_BRIDGE_ENABLE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mesh_eth_bridge_enable, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
        break;
        case VSP_FORTINET_MESH_ETH_BRIDGE_TYPE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_mesh_eth_bridge_type, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
        break;
        case VSP_FORTINET_WTP_CAP: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wtp_cap, tvb, offset, optlen, ENC_NA);
            offset += optlen;
        break;
        case VSP_FORTINET_TXPWR: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_txpwr_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_txpwr, tvb, offset, 2, ENC_NA);
            offset += 2;
        break;
        case VSP_FORTINET_WIDS_ENABLE: /
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wids_enable_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_wids_enable, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
        break;
        case 146: /
        case 152:
        case 153:
        case 163:
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_unknown_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            optlen -= 1;
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_unknown_wid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            optlen -= 1;
            expert_add_info_format(pinfo, msg_element_type_item, &ei_capwap_message_element_fortinet_type,
                                 "Dissector for CAPWAP Vendor Specific (Fortinet) Message Element"
                                 " (%d) type not implemented (VAP Stuff..)", element_id);
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_unknown, tvb, offset, optlen, ENC_NA);
            offset += optlen;
        break;

        case 65: /
        case 170:
        case 171:
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_unknown_rid, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            optlen -= 1;
            expert_add_info_format(pinfo, msg_element_type_item, &ei_capwap_message_element_fortinet_type,
                                 "Dissector for CAPWAP Vendor Specific (Fortinet) Message Element"
                                 " (%d) type not implemented (VAP Stuff..)", element_id);
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_unknown, tvb, offset, optlen, ENC_NA);
            offset += optlen;
        break;
        default:
            expert_add_info_format(pinfo, msg_element_type_item, &ei_capwap_message_element_fortinet_type,
                                 "Dissector for CAPWAP Vendor Specific (Fortinet) Message Element"
                                 " (%d) type not implemented", element_id);
            proto_tree_add_item(sub_msg_element_type_tree, hf_capwap_fortinet_unknown, tvb, offset, optlen, ENC_NA);
            offset += optlen;
        break;
    }

    return offset;
}
