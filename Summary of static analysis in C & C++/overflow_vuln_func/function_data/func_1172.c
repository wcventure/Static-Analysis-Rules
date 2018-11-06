static void dissect_wa_payload(guint32 starting_offset, proto_item *parent_tree, tvbuff_t *tvb, guint32 control_word, guint8 version)
{
    guint32 if_type, if_status, flags_bitfield;
    guint8 iLoop, isr, n, ret;
    guint32 offset, delta, num_bss_entries, st_index[NUM_STATE_CHANGES], bss_array[NUM_BSS];
    proto_tree *st_change_index_tree[NUM_STATE_CHANGES], *fs_flags;
    proto_tree *bss_tree[NUM_BSS], *fs_flags_tree;
    proto_item *stIndex[NUM_STATE_CHANGES], *bssIndex[NUM_BSS];
    const guint8 *tag_data_ptr;
    guint32 tag_len;
    char out_buff[SHORT_STR];
    char print_buff[SHORT_STR];

    switch (control_word)
    {
        case 0x11:   /
            proto_tree_add_item(parent_tree,
                hf_waveagent_payfill, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_paysize, tvb, starting_offset+4, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_avgrate, tvb, starting_offset+8, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_totalframes, tvb, starting_offset+12, 4, ENC_BIG_ENDIAN);

            break;

        case 0x23:   /
            proto_tree_add_item(parent_tree,
                hf_waveagent_ifindex, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_bssidstartindex, tvb, starting_offset+4, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_bssidstopindex, tvb, starting_offset+8, 4, ENC_BIG_ENDIAN);

            break;

        case 0x24:   /
        case 0x25:   /
        case 0x8a:   /
            proto_tree_add_item(parent_tree,
                hf_waveagent_ifindex, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            break;

        case 0x26:   /
            proto_tree_add_item(parent_tree,
                hf_waveagent_ifindex, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_oidcode, tvb, starting_offset+4, 4, ENC_BIG_ENDIAN);

            break;

        case 0x30:   /
            proto_tree_add_item(parent_tree,
                hf_waveagent_ifindex, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            if_type = tvb_get_ntohl(tvb, starting_offset + 4);

            proto_tree_add_item(parent_tree,
                hf_waveagent_iftype, tvb, starting_offset + 4, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_ifdhcp, tvb, starting_offset + 8, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_ifmacaddr, tvb, starting_offset + 12, 6, ENC_NA);

            /

            proto_tree_add_item(parent_tree,
                hf_waveagent_iflinkspeed, tvb, starting_offset + 20, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_ifdescription, tvb, starting_offset + 24, 128, ENC_ASCII|ENC_NA);

            /

            /

            /
            if (if_type == WLAN_INTERFACE)
                dissect_wlan_if_stats(starting_offset + 156, parent_tree, tvb);

            /

            /

            proto_tree_add_item(parent_tree,
                hf_waveagent_ifiptype, tvb, starting_offset + 252, 2, ENC_BIG_ENDIAN);

            if (tvb_get_ntohs(tvb, starting_offset + 252) == IPV4_TYPE) {
                proto_tree_add_item(parent_tree,
                    hf_waveagent_ifipv4, tvb, starting_offset + 260, 4, ENC_BIG_ENDIAN);  /
            }
            else {
                proto_tree_add_item(parent_tree,
                    hf_waveagent_ifipv6, tvb, starting_offset + 260, 16, ENC_NA);
            }

            proto_tree_add_item(parent_tree,
                hf_waveagent_ifdhcpserver, tvb, starting_offset + 284, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_ifgateway, tvb, starting_offset + 308, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_ifdnsserver, tvb, starting_offset + 332, 4, ENC_BIG_ENDIAN);

            break;

        case 0x31:   /
            /
            st_index[0] = ett_scindex0;
            st_index[1] = ett_scindex1;
            st_index[2] = ett_scindex2;
            st_index[3] = ett_scindex3;
            st_index[4] = ett_scindex4;
            st_index[5] = ett_scindex5;
            st_index[6] = ett_scindex6;
            st_index[7] = ett_scindex7;

            proto_tree_add_item(parent_tree,
                hf_waveagent_ifindex, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            if_type = tvb_get_ntohl(tvb, starting_offset + 4);

            proto_tree_add_item(parent_tree,
                hf_waveagent_iftype, tvb, starting_offset + 4, 4, ENC_BIG_ENDIAN);

            offset = starting_offset + 8;
            delta = 156;

            for (iLoop = 0; iLoop < NUM_STATE_CHANGES; iLoop++) {
                int current_offset;
                current_offset = offset + iLoop * delta;

                /
                if_status = tvb_get_ntohl(tvb, current_offset);
                if (if_status == 0) continue;  /

                /
                stIndex[iLoop] = proto_tree_add_uint_format_value(parent_tree, 
                    hf_waveagent_ifwlanl2status, tvb, current_offset, 4, if_status, "Interface state change %d", iLoop);

                st_change_index_tree[iLoop] = proto_item_add_subtree(stIndex[iLoop], st_index[iLoop]);

                if (if_type == WLAN_INTERFACE) {
                    proto_tree_add_item(st_change_index_tree[iLoop],
                        hf_waveagent_ifwlanl2status, tvb, current_offset, 4, ENC_BIG_ENDIAN);
                } else {
                    proto_tree_add_item(st_change_index_tree[iLoop],
                        hf_waveagent_ifethl2status, tvb, current_offset, 4, ENC_BIG_ENDIAN);
                }

                proto_tree_add_item(st_change_index_tree[iLoop],
                    hf_waveagent_ifl3status, tvb, current_offset + 4, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(st_change_index_tree[iLoop],
                    hf_waveagent_iflinkspeed, tvb, current_offset + 8, 4, ENC_BIG_ENDIAN);

                if (if_type == WLAN_INTERFACE) {
                    dissect_wlan_if_stats(current_offset + 12, st_change_index_tree[iLoop], tvb);
                }

                proto_tree_add_item(st_change_index_tree[iLoop],
                    hf_waveagent_snap, tvb, current_offset + 108, 8, ENC_BIG_ENDIAN);

                proto_tree_add_item(st_change_index_tree[iLoop],
                    hf_waveagent_ifiptype, tvb, current_offset + 116, 2, ENC_BIG_ENDIAN);

                if (tvb_get_ntohs(tvb, current_offset + 116) == IPV4_TYPE) {
                    proto_tree_add_item(st_change_index_tree[iLoop],
                        hf_waveagent_ifipv4, tvb, current_offset + 124, 4, ENC_BIG_ENDIAN);  /
                }
                else {
                    proto_tree_add_item(st_change_index_tree[iLoop],
                        hf_waveagent_ifipv6, tvb, current_offset + 124, 16, ENC_NA);
                }

                /
            }

            break;

        case 0x32:   /
            proto_tree_add_item(parent_tree,
                hf_waveagent_ifindex, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_oidcode, tvb, starting_offset + 4, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_oidvalue, tvb, starting_offset + 12, 1024, ENC_ASCII|ENC_NA);

            break;

        case 0x2e:   /
            proto_tree_add_item(parent_tree,
                hf_waveagent_ifindex, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            bss_array[0] = ett_bss0;
            bss_array[1] = ett_bss1;
            bss_array[2] = ett_bss2;
            bss_array[3] = ett_bss3;
            bss_array[4] = ett_bss4;
            bss_array[5] = ett_bss5;
            bss_array[6] = ett_bss6;
            bss_array[7] = ett_bss7;

            proto_tree_add_item(parent_tree,
                hf_waveagent_totalbssid, tvb, starting_offset + 4, 4, ENC_BIG_ENDIAN);

            num_bss_entries = tvb_get_ntohl(tvb, starting_offset + 8);

            proto_tree_add_item(parent_tree,
                hf_waveagent_returnedbssid, tvb, starting_offset + 8, 4, ENC_BIG_ENDIAN);

            /

            offset = starting_offset + 16;
            delta = 148;

            for (iLoop = 0; iLoop < num_bss_entries; iLoop++)
            {
                int current_offset;
                current_offset = offset + iLoop * delta;

                bssIndex[iLoop] = proto_tree_add_item(parent_tree,
                    hf_waveagent_scanssid, tvb, current_offset, 32, ENC_ASCII|ENC_NA);

                bss_tree[iLoop] = proto_item_add_subtree(bssIndex[iLoop], bss_array[iLoop]);

                tag_len = tvb_get_ntohl(tvb, current_offset + 52);

                if (tag_len > 0) {
                    tag_data_ptr = tvb_get_ptr (tvb, offset + 36, tag_len);

                    for (isr = 0, n = 0; isr < tag_len; isr++) {
                        if (tag_data_ptr[isr] == 0xFF){
                            proto_tree_add_string (bss_tree[iLoop], hf_waveagent_ifwlansupprates, tvb, offset + 36 + isr,
                                1, "BSS requires support for mandatory features of HT PHY (IEEE 802.11 - Clause 20)");
                        } else {
                            ret = g_snprintf (print_buff + n, SHORT_STR - n, "%2.1f%s ",
                                      (tag_data_ptr[isr] & 0x7F) * 0.5,
                                      (tag_data_ptr[isr] & 0x80) ? "(B)" : "");
                            if (ret >= SHORT_STR - n) {
                                /
                                break;
                            }
                            n += ret;
                        }
                    }

                    g_snprintf (out_buff, SHORT_STR, "%s [Mbit/sec]", print_buff);
                }
                else {
                    g_snprintf (out_buff, SHORT_STR, "Not defined");
                }

                proto_tree_add_string (bss_tree[iLoop], hf_waveagent_ifwlansupprates, tvb, offset + 36,
                    tag_len, out_buff);

                proto_tree_add_item(bss_tree[iLoop],
                    hf_waveagent_scanbssid, tvb, current_offset + 56, 6, ENC_NA);

                proto_tree_add_item(bss_tree[iLoop],
                    hf_waveagent_ifwlancapabilities, tvb, current_offset + 62, 2, ENC_BIG_ENDIAN);

                proto_tree_add_item(bss_tree[iLoop],
                    hf_waveagent_ifwlanrssi, tvb, current_offset + 64, 4, ENC_BIG_ENDIAN);

                /
        /
                proto_tree_add_item(bss_tree[iLoop],
                    hf_waveagent_ifwlanchannel, tvb, current_offset + 72, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(bss_tree[iLoop],
                    hf_waveagent_ifwlanprivacy, tvb, current_offset + 76, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(bss_tree[iLoop],
                    hf_waveagent_ifwlanbssmode, tvb, current_offset + 80, 4, ENC_BIG_ENDIAN);
            }

            break;

        case 0x2f:   /
            if (version < 3) {
                /
                proto_tree_add_item(parent_tree,
                    hf_waveagent_capstatus, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_protocolversion, tvb, starting_offset, 1, ENC_BIG_ENDIAN);

                starting_offset += 4;
            }

            proto_tree_add_item(parent_tree,
                hf_waveagent_capimpl, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                       hf_waveagent_state, tvb, starting_offset + 4, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                       hf_waveagent_appstate, tvb, starting_offset + 8, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rxdatapckts, tvb, starting_offset + 12, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rxdatabytes, tvb, starting_offset + 20, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rxpcktrate, tvb, starting_offset + 28, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rxbyterate, tvb, starting_offset + 36, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_txdatapckts, tvb, starting_offset + 44, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_txdatabytes, tvb, starting_offset + 52, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_txpcktrate, tvb, starting_offset + 60, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_txbyterate, tvb, starting_offset + 68, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_looppckts, tvb, starting_offset + 76, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_loopbytes, tvb, starting_offset + 84, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rxctlpckts, tvb, starting_offset + 92, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rxctlbytes, tvb, starting_offset + 100, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_txctlpckts, tvb, starting_offset + 108, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_txctlbytes, tvb, starting_offset + 116, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_unknowncmds, tvb, starting_offset + 124, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_snap, tvb, starting_offset + 132, 8, ENC_BIG_ENDIAN);

        /
            proto_tree_add_item(parent_tree,
                hf_waveagent_rx1pl, tvb, starting_offset + 284, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rx2pl, tvb, starting_offset + 292, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rx3pl, tvb, starting_offset + 300, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rx4pl, tvb, starting_offset + 308, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rx5pl, tvb, starting_offset + 316, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_rxoospkts, tvb, starting_offset + 324, 8, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_jitter, tvb, starting_offset + 356, 8, ENC_BIG_ENDIAN);

            if (version >= 3) {
                proto_tree_add_item(parent_tree,
                    hf_waveagent_delayfactor, tvb, starting_offset + 364, 8, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_medialossrate, tvb, starting_offset + 372, 8, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_txstartts, tvb, starting_offset + 380, 8, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_txendts, tvb, starting_offset + 388, 8, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_rxstartts, tvb, starting_offset + 396, 8, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_rxendts, tvb, starting_offset + 404, 8, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_latencysum, tvb, starting_offset + 412, 8, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_latencycount, tvb, starting_offset + 420, 8, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_txflowstop, tvb, starting_offset + 428, 8, ENC_BIG_ENDIAN);
            }

            break;

        case 0x40:
            proto_tree_add_item(parent_tree,
                hf_waveagent_ifindex, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_connectflags, tvb, starting_offset + 4, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_connecttype, tvb, starting_offset + 8, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_scanssid, tvb, starting_offset + 12, 32, ENC_ASCII|ENC_NA);

            num_bss_entries = tvb_get_ntohl(tvb, starting_offset + 142);

            offset = starting_offset + 46;
            delta = 6;
            for (iLoop = 0; iLoop < num_bss_entries; iLoop++)
            {
                int current_offset;
                current_offset = offset + iLoop * delta;

                proto_tree_add_item(parent_tree,
                    hf_waveagent_scanbssid, tvb, current_offset, 6, ENC_NA);
            }

            proto_tree_add_item(parent_tree,
                hf_waveagent_minrssi, tvb, starting_offset + 146, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_connecttimeout, tvb, starting_offset + 150, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_connectattempts, tvb, starting_offset + 154, 4, ENC_BIG_ENDIAN);

            break;

        case 0x41:
            proto_tree_add_item(parent_tree,
                hf_waveagent_ifindex, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_reason, tvb, starting_offset + 4, 4, ENC_BIG_ENDIAN);

            break;

        case 0x81:   /
            if (version < 3) {
                /
                proto_tree_add_item(parent_tree,
                    hf_waveagent_capstatus, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_protocolversion, tvb, starting_offset, 1, ENC_BIG_ENDIAN);

                starting_offset += 4;
            }

            proto_tree_add_item(parent_tree,
                hf_waveagent_capimpl, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_id, tvb, starting_offset + 4, 128, ENC_ASCII|ENC_NA);

            proto_tree_add_item(parent_tree,
                hf_waveagent_bindtag, tvb, starting_offset + 136, 128, ENC_ASCII|ENC_NA);

            proto_tree_add_item(parent_tree,
                hf_waveagent_version, tvb, starting_offset + 268, 128, ENC_ASCII|ENC_NA);

            proto_tree_add_item(parent_tree,
                hf_waveagent_brokerip, tvb, starting_offset + 400, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_brokerport, tvb, starting_offset + 404, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_bindlevel, tvb, starting_offset + 408, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_bindport, tvb, starting_offset + 412, 4, ENC_BIG_ENDIAN);

            if (version >= 3) {
                proto_tree_add_item(parent_tree,
                    hf_waveagent_capabilities2, tvb, starting_offset + 416, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_numinterfaces, tvb, starting_offset + 420, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_ifmask, tvb, starting_offset + 424, 4, ENC_BIG_ENDIAN);
            }

            break;

        case 0x82:    /
            proto_tree_add_item(parent_tree,
                hf_waveagent_bindtag, tvb, starting_offset, 128, ENC_ASCII|ENC_NA);

            proto_tree_add_item(parent_tree,
                hf_waveagent_brokerip, tvb, starting_offset + 132, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_brokerport, tvb, starting_offset + 136, 4, ENC_BIG_ENDIAN);

            break;

        case 0x85:    /
            if (version < 3) {
                proto_tree_add_item(parent_tree,
                    hf_waveagent_rxflownum, tvb, starting_offset, 4, ENC_BIG_ENDIAN);
            }

            proto_tree_add_item(parent_tree,
                hf_waveagent_mode, tvb, starting_offset + 7, 1, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_endpointtype, tvb, starting_offset + 7, 1, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_bindport, tvb, starting_offset + 8, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_bindlevel, tvb, starting_offset + 12, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_remoteport, tvb, starting_offset + 16, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_remoteaddr, tvb, starting_offset + 24, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_dscp, tvb, starting_offset + 40, 4, ENC_BIG_ENDIAN);

            flags_bitfield = tvb_get_ntohl(tvb, starting_offset + 44);

            fs_flags = proto_tree_add_uint(parent_tree, hf_waveagent_fsflags,
                                        tvb, starting_offset + 44, 4, flags_bitfield);

            fs_flags_tree = proto_item_add_subtree(fs_flags, ett_fsflags);

            proto_tree_add_item(fs_flags_tree,
                    hf_waveagent_fscbrflag, tvb, starting_offset + 47, 1, ENC_LITTLE_ENDIAN);

            proto_tree_add_item(fs_flags_tree,
                    hf_waveagent_fscombinedsetupflag, tvb, starting_offset + 47, 1, ENC_LITTLE_ENDIAN);

            if (version >= 3) {
                proto_tree_add_item(parent_tree,
                    hf_waveagent_ifindex, tvb, starting_offset + 48, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_payfill, tvb, starting_offset + 52, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_paysize, tvb, starting_offset + 56, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_avgrate, tvb, starting_offset + 60, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(parent_tree,
                    hf_waveagent_totalframes, tvb, starting_offset + 64, 4, ENC_BIG_ENDIAN);
            }

            break;

        case 0x8b:
            proto_tree_add_item(parent_tree,
                hf_waveagent_destip, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_destport, tvb, starting_offset + 4, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_connectflags, tvb, starting_offset + 8, 4, ENC_BIG_ENDIAN);

            break;

        case 0x3f:  /
        case 0x8f:  /
            proto_tree_add_item(parent_tree,
                hf_waveagent_commandstatus, tvb, starting_offset, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_syserrno, tvb, starting_offset + 4, 4, ENC_BIG_ENDIAN);

            proto_tree_add_item(parent_tree,
                hf_waveagent_statusstring, tvb, starting_offset + 8, 128, ENC_ASCII|ENC_NA);

            break;
    }
}
