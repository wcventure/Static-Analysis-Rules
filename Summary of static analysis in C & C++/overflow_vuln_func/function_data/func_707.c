static void
dissect_ieee802154_common(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint options)
{
    tvbuff_t                *volatile payload_tvb = NULL;
    proto_tree              *volatile ieee802154_tree = NULL;
    proto_item              *volatile proto_root = NULL;
    proto_item              *hidden_item;
    proto_item              *ti;
    proto_item              *mic_item = NULL;
    proto_tree              *header_tree = NULL;
    guint                   offset = 0;
    volatile gboolean       fcs_ok = TRUE;
    const char              *saved_proto;
    ws_decrypt_status       status;
    gboolean                dstPanPresent = FALSE;
    gboolean                srcPanPresent = FALSE;
    unsigned char           rx_mic[IEEE802154_CIPHER_SIZE];
    guint                   rx_mic_len = 0;
    ieee802154_packet      *packet = wmem_new0(wmem_packet_scope(), ieee802154_packet);
    ieee802154_short_addr   addr16;
    ieee802154_hints_t     *ieee_hints;

    heur_dtbl_entry_t      *hdtbl_entry;

    packet->short_table = ieee802154_map.short_table;

    /
    if(!pinfo->fd->flags.visited){
        ieee_hints = wmem_new0(wmem_file_scope(), ieee802154_hints_t);
        p_add_proto_data(wmem_file_scope(), pinfo, proto_ieee802154, 0, ieee_hints);
    } else {
        ieee_hints = (ieee802154_hints_t *)p_get_proto_data(wmem_file_scope(), pinfo, proto_ieee802154, 0);
    }

    /
    ieee_hints->packet = packet;

    /
    if (tree) {
        proto_root = proto_tree_add_protocol_format(tree, proto_ieee802154, tvb, 0, tvb_captured_length(tvb), "IEEE 802.15.4");
        ieee802154_tree = proto_item_add_subtree(proto_root, ett_ieee802154);
    }
    /
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "IEEE 802.15.4");

    /
    hidden_item = proto_tree_add_uint(ieee802154_tree, hf_ieee802154_frame_length, NULL, 0, 0, tvb_reported_length(tvb));
    PROTO_ITEM_SET_HIDDEN(hidden_item);

    /
    dissect_ieee802154_fcf(tvb, pinfo, ieee802154_tree, packet, &offset);

    /
    if (packet->seqno_suppression) {
        if (packet->version != IEEE802154_VERSION_2015) {
            expert_add_info(pinfo, proto_root, &ei_ieee802154_seqno_suppression);
        }
    } else { /
    packet->seqno = tvb_get_guint8(tvb, offset);
    if (tree) {
        proto_tree_add_uint(ieee802154_tree, hf_ieee802154_seqno, tvb, offset, 1, packet->seqno);
        /
        if (packet->frame_type == IEEE802154_FCF_ACK) {
            proto_item_append_text(proto_root, ", Sequence Number: %u", packet->seqno);
        }
    }
    offset += 1;
    }

    /
    /
    clear_address(&pinfo->net_dst);
    clear_address(&pinfo->dl_dst);
    clear_address(&pinfo->dst);
    clear_address(&pinfo->net_src);
    clear_address(&pinfo->dl_src);
    clear_address(&pinfo->src);

    if (packet->dst_addr_mode == IEEE802154_FCF_ADDR_RESERVED) {
        /
        expert_add_info(pinfo, proto_root, &ei_ieee802154_dst);
        return;
    }

    if (packet->src_addr_mode == IEEE802154_FCF_ADDR_RESERVED) {
        /
        expert_add_info(pinfo, proto_root, &ei_ieee802154_src);
        return;
    }

    if (packet->version == IEEE802154_VERSION_RESERVED) {
        /
        expert_add_info(pinfo, proto_root, &ei_ieee802154_frame_ver);
        return;
    }
    else if ((packet->version == IEEE802154_VERSION_2003) ||  /
             (packet->version == IEEE802154_VERSION_2006))  { /

        if ((packet->dst_addr_mode != IEEE802154_FCF_ADDR_NONE) && /
            (packet->src_addr_mode != IEEE802154_FCF_ADDR_NONE)) { /
            if (packet->pan_id_compression == 1) { /
                dstPanPresent = TRUE;
                srcPanPresent = FALSE; /
            }
            else { /
                dstPanPresent = TRUE;
                srcPanPresent = TRUE;
            }
        }
        else {
            if (packet->pan_id_compression == 1) { /
                expert_add_info(pinfo, proto_root, &ei_ieee802154_invalid_addressing);
                return;
            }
            else {
                /
                if ((packet->dst_addr_mode != IEEE802154_FCF_ADDR_NONE) &&        /
                    (packet->src_addr_mode == IEEE802154_FCF_ADDR_NONE)) {        /
                    dstPanPresent = TRUE;
                    srcPanPresent = FALSE;
                }
                else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_NONE) &&   /
                         (packet->src_addr_mode != IEEE802154_FCF_ADDR_NONE)) {   /
                    dstPanPresent = FALSE;
                    srcPanPresent = TRUE;
                }
                else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_NONE) &&   /
                         (packet->src_addr_mode == IEEE802154_FCF_ADDR_NONE)) {   /
                    dstPanPresent = FALSE;
                    srcPanPresent = FALSE;
                }
                else {
                    expert_add_info(pinfo, proto_root, &ei_ieee802154_invalid_addressing);
                    return;
                }
            }
        }
    }
    else if (packet->version == IEEE802154_VERSION_2015) {
        /
        if ((packet->frame_type == IEEE802154_FCF_BEACON) ||
            (packet->frame_type == IEEE802154_FCF_DATA)   ||
            (packet->frame_type == IEEE802154_FCF_ACK)    ||
            (packet->frame_type == IEEE802154_FCF_CMD)       ) {

            /

            /
            if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_NONE) &&      /
                (packet->src_addr_mode == IEEE802154_FCF_ADDR_NONE) &&      /
                (packet->pan_id_compression == 0)) {
                        dstPanPresent = FALSE;
                        srcPanPresent = FALSE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_NONE) && /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_NONE) && /
                     (packet->pan_id_compression == 1)) {
                        dstPanPresent = TRUE;
                        srcPanPresent = FALSE;
            }
            /
            else if ((packet->dst_addr_mode != IEEE802154_FCF_ADDR_NONE) && /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_NONE) && /
                     (packet->pan_id_compression == 0)) {
                        dstPanPresent = TRUE;
                        srcPanPresent = FALSE;
            }
            /
            else if ((packet->dst_addr_mode != IEEE802154_FCF_ADDR_NONE) && /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_NONE) && /
                     (packet->pan_id_compression == 1)) {
                        dstPanPresent = FALSE;
                        srcPanPresent = FALSE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_NONE) && /
                     (packet->src_addr_mode != IEEE802154_FCF_ADDR_NONE) && /
                     (packet->pan_id_compression == 0)) {
                        dstPanPresent = FALSE;
                        srcPanPresent = TRUE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_NONE) && /
                     (packet->src_addr_mode != IEEE802154_FCF_ADDR_NONE) && /
                     (packet->pan_id_compression == 1)) {
                        dstPanPresent = FALSE;
                        srcPanPresent = FALSE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_EXT) && /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_EXT) && /
                     (packet->pan_id_compression == 0)) {
                        dstPanPresent = TRUE;
                        srcPanPresent = FALSE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_EXT) && /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_EXT) && /
                     (packet->pan_id_compression == 1)) {
                        dstPanPresent = FALSE;
                        srcPanPresent = FALSE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_SHORT) && /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_SHORT) && /
                     (packet->pan_id_compression == 0)) {
                        dstPanPresent = TRUE;
                        srcPanPresent = TRUE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_SHORT) && /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_EXT) &&   /
                     (packet->pan_id_compression == 0)) {
                        dstPanPresent = TRUE;
                        srcPanPresent = TRUE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_EXT)   &&   /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_SHORT) &&   /
                     (packet->pan_id_compression == 0)) {
                        dstPanPresent = TRUE;
                        srcPanPresent = TRUE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_SHORT) &&   /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_EXT)   &&   /
                     (packet->pan_id_compression == 1)) {
                        dstPanPresent = TRUE;
                        srcPanPresent = FALSE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_EXT)   &&   /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_SHORT) &&   /
                     (packet->pan_id_compression == 1)) {
                        dstPanPresent = TRUE;
                        srcPanPresent = FALSE;
            }
            /
            else if ((packet->dst_addr_mode == IEEE802154_FCF_ADDR_SHORT) &&   /
                     (packet->src_addr_mode == IEEE802154_FCF_ADDR_SHORT) &&   /
                     (packet->pan_id_compression == 1)) {
                        dstPanPresent = TRUE;
                        srcPanPresent = FALSE;
            }
            else {
                expert_add_info(pinfo, proto_root, &ei_ieee802154_invalid_panid_compression2);
                return;
            }
        }
        else { /
            dstPanPresent = FALSE; /
            srcPanPresent = FALSE; /
        }
    }
    else {
        /
        expert_add_info(pinfo, proto_root, &ei_ieee802154_frame_ver);
        return;
    }

    /

    /
    if (dstPanPresent) {
        packet->dst_pan = tvb_get_letohs(tvb, offset);
        if (ieee802154_tree) {
            proto_tree_add_uint(ieee802154_tree, hf_ieee802154_dst_panID, tvb, offset, 2, packet->dst_pan);
        }
        offset += 2;
    }

    /
    if (packet->dst_addr_mode == IEEE802154_FCF_ADDR_SHORT) {
        gchar* dst_addr;

        /
        packet->dst16 = tvb_get_letohs(tvb, offset);

        /
        if (ieee_hints) {
            ieee_hints->dst16 = packet->dst16;
        }

        set_address_tvb(&pinfo->dl_dst, ieee802_15_4_short_address_type, 2, tvb, offset);
        copy_address_shallow(&pinfo->dst, &pinfo->dl_dst);
        dst_addr = address_to_str(wmem_packet_scope(), &pinfo->dst);

        proto_tree_add_uint(ieee802154_tree, hf_ieee802154_dst16, tvb, offset, 2, packet->dst16);
        proto_item_append_text(proto_root, ", Dst: %s", dst_addr);

        col_append_fstr(pinfo->cinfo, COL_INFO, ", Dst: %s", dst_addr);
        offset += 2;
    }
    else if (packet->dst_addr_mode == IEEE802154_FCF_ADDR_EXT) {
        guint64 *p_addr = (guint64 *)wmem_new(pinfo->pool, guint64);

        /
        packet->dst64 = tvb_get_letoh64(tvb, offset);

        /
        *p_addr = pntoh64(&(packet->dst64));

        /
        /
        set_address(&pinfo->dl_dst, AT_EUI64, 8, p_addr);
        copy_address_shallow(&pinfo->dst, &pinfo->dl_dst);
        if (tree) {
            proto_tree_add_item(ieee802154_tree, hf_ieee802154_dst64, tvb, offset, 8, ENC_LITTLE_ENDIAN);
            proto_item_append_text(proto_root, ", Dst: %s", eui64_to_display(wmem_packet_scope(), packet->dst64));
        }
        col_append_fstr(pinfo->cinfo, COL_INFO, ", Dst: %s", eui64_to_display(wmem_packet_scope(), packet->dst64));
        offset += 8;
    }

    /
    if (srcPanPresent) {
        packet->src_pan = tvb_get_letohs(tvb, offset);
        proto_tree_add_uint(ieee802154_tree, hf_ieee802154_src_panID, tvb, offset, 2, packet->src_pan);
        offset += 2;
    }
    else {
        if (dstPanPresent) {
            packet->src_pan = packet->dst_pan;
        }
        else {
            packet->src_pan = IEEE802154_BCAST_PAN;
        }
    }
    if (ieee_hints) {
        ieee_hints->src_pan = packet->src_pan;
    }

    /
    if (packet->src_addr_mode == IEEE802154_FCF_ADDR_SHORT) {
        gchar* src_addr;

        /
        packet->src16 = tvb_get_letohs(tvb, offset);

        if (!pinfo->fd->flags.visited) {
            /
            addr16.addr = packet->src16;
            addr16.pan = packet->src_pan;

            if (ieee_hints) {
                ieee_hints->src16 = packet->src16;
                ieee_hints->map_rec = (ieee802154_map_rec *)
                    g_hash_table_lookup(ieee802154_map.short_table, &addr16);
            }
        }

        set_address_tvb(&pinfo->dl_src, ieee802_15_4_short_address_type, 2, tvb, offset);
        copy_address_shallow(&pinfo->src, &pinfo->dl_src);
        src_addr = address_to_str(wmem_packet_scope(), &pinfo->src);

        /
        if (tree) {
            proto_tree_add_uint(ieee802154_tree, hf_ieee802154_src16, tvb, offset, 2, packet->src16);
            proto_item_append_text(proto_root, ", Src: %s", src_addr);

            if (ieee_hints && ieee_hints->map_rec) {
                /
                ti = proto_tree_add_eui64(ieee802154_tree, hf_ieee802154_src64, tvb, offset, 0,
                        ieee_hints->map_rec->addr64);
                PROTO_ITEM_SET_GENERATED(ti);

                if ( ieee_hints->map_rec->start_fnum ) {
                    ti = proto_tree_add_uint(ieee802154_tree, hf_ieee802154_src64_origin, tvb, 0, 0,
                        ieee_hints->map_rec->start_fnum);
                }
                else {
                    ti = proto_tree_add_uint_format_value(ieee802154_tree, hf_ieee802154_src64_origin, tvb, 0, 0,
                        ieee_hints->map_rec->start_fnum, "Pre-configured");
                }
                PROTO_ITEM_SET_GENERATED(ti);
            }
        }

        col_append_fstr(pinfo->cinfo, COL_INFO, ", Src: %s", src_addr);

        offset += 2;
    }
    else if (packet->src_addr_mode == IEEE802154_FCF_ADDR_EXT) {
        guint64 *p_addr = (guint64 *)wmem_new(pinfo->pool, guint64);

        /
        packet->src64 = tvb_get_letoh64(tvb, offset);

        /
        *p_addr = pntoh64(&(packet->src64));

        /
        /
        set_address(&pinfo->dl_src, AT_EUI64, 8, p_addr);
        copy_address_shallow(&pinfo->src, &pinfo->dl_src);
        if (tree) {
            proto_tree_add_item(ieee802154_tree, hf_ieee802154_src64, tvb, offset, 8, ENC_LITTLE_ENDIAN);
            proto_item_append_text(proto_root, ", Src: %s", eui64_to_display(wmem_packet_scope(), packet->src64));
        }

        col_append_fstr(pinfo->cinfo, COL_INFO, ", Src: %s", eui64_to_display(wmem_packet_scope(), packet->src64));
        offset += 8;
    }


    /
    if (tvb_bytes_exist(tvb, tvb_reported_length(tvb)-IEEE802154_FCS_LEN, IEEE802154_FCS_LEN)) {
        /
        guint16     fcs = tvb_get_letohs(tvb, tvb_reported_length(tvb)-IEEE802154_FCS_LEN);
        /
        if (options & DISSECT_IEEE802154_OPTION_CC24xx) {
            fcs_ok = (fcs & IEEE802154_CC24xx_CRC_OK);
        }
        else {
            guint16 fcs_calc = ieee802154_crc_tvb(tvb, tvb_reported_length(tvb)-IEEE802154_FCS_LEN);
            fcs_ok = (fcs == fcs_calc);
        }
    }

    /
    if ((packet->security_enable) && (packet->version != IEEE802154_VERSION_2003)) {
        dissect_ieee802154_aux_sec_header_and_key(tvb, pinfo, ieee802154_tree, packet, &offset);
    }

    /
    /
    if ((packet->version == IEEE802154_VERSION_2003) || (packet->version == IEEE802154_VERSION_2006)) {
        if (packet->frame_type == IEEE802154_FCF_BEACON) { /
            dissect_ieee802154_superframe(tvb, pinfo, ieee802154_tree, &offset); /
            dissect_ieee802154_gtsinfo(tvb, pinfo, ieee802154_tree, &offset);    /
            dissect_ieee802154_pendaddr(tvb, pinfo, ieee802154_tree, &offset);   /
        }

        if (packet->frame_type == IEEE802154_FCF_CMD) {
            /
            packet->command_id = tvb_get_guint8(tvb, offset);
            if (tree) {
                proto_tree_add_uint(ieee802154_tree, hf_ieee802154_cmd_id, tvb, offset, 1, packet->command_id);
            }
            offset++;

            /
            col_set_str(pinfo->cinfo, COL_INFO, val_to_str_const(packet->command_id, ieee802154_cmd_names, "Unknown Command"));
        }
    }
    else {
        if (packet->ie_present) {
            dissect_ieee802154_header_ie(tvb, pinfo, ieee802154_tree, &offset, packet);
        }
    }

    /
    if (packet->security_enable && (packet->version == IEEE802154_VERSION_2003)) {
        /
        packet->security_level = (ieee802154_security_level)ieee802154_sec_suite;

        /
        if (IEEE802154_IS_ENCRYPTED(packet->security_level)) {
            packet->frame_counter = tvb_get_letohl (tvb, offset);
            proto_tree_add_uint(ieee802154_tree, hf_ieee802154_sec_frame_counter, tvb, offset, (int)sizeof(guint32), packet->frame_counter);
            offset += (int)sizeof(guint32);

            packet->key_sequence_counter = tvb_get_guint8 (tvb, offset);
            proto_tree_add_uint(ieee802154_tree, hf_ieee802154_sec_key_sequence_counter, tvb, offset, (int)sizeof(guint8), packet->key_sequence_counter);
            offset += (int)sizeof(guint8);
        }
    }

    /
    if (packet->security_enable) {
        ieee802154_payload_info_t payload_info;

        payload_info.rx_mic = rx_mic;
        payload_info.rx_mic_length = &rx_mic_len;
        payload_info.status = &status;
        payload_info.key = NULL; /

        payload_tvb = dissect_ieee802154_payload(tvb, offset, pinfo, header_tree, packet, &payload_info,
                                     ieee802154_set_mac_key, dissect_ieee802154_decrypt);

        /
        if (!payload_tvb) {
            /
            gint reported_len = tvb_reported_length(tvb)-offset-rx_mic_len-IEEE802154_FCS_LEN;
            gint captured_len = tvb_captured_length(tvb)-offset;
            payload_tvb = tvb_new_subset_length_caplen(tvb, offset, MIN(captured_len, reported_len), reported_len);
        }

        /
        if (rx_mic_len) {
            mic_item = proto_tree_add_bytes(header_tree, hf_ieee802154_mic, tvb, 0, rx_mic_len, rx_mic);
            PROTO_ITEM_SET_GENERATED(mic_item);
        }

        /
        switch (status) {
        case DECRYPT_PACKET_SUCCEEDED:
        case DECRYPT_NOT_ENCRYPTED:
            /
            proto_item_append_text(mic_item, " [correct (key no. %d)]", payload_info.key_number);
            break;

        case DECRYPT_FRAME_COUNTER_SUPPRESSION_UNSUPPORTED:
            expert_add_info_format(pinfo, proto_root, &ei_ieee802154_decrypt_error, "Decryption of 802.15.4-2015 with frame counter suppression is not supported");
            call_data_dissector(payload_tvb, pinfo, tree);
            goto dissect_ieee802154_fcs;

        case DECRYPT_PACKET_TOO_SMALL:
            expert_add_info_format(pinfo, proto_root, &ei_ieee802154_decrypt_error, "Packet was too small to include the CRC and MIC");
            call_data_dissector(payload_tvb, pinfo, tree);
            goto dissect_ieee802154_fcs;

        case DECRYPT_PACKET_NO_EXT_SRC_ADDR:
            expert_add_info_format(pinfo, proto_root, &ei_ieee802154_decrypt_error, "No extended source address - can't decrypt");
            call_data_dissector(payload_tvb, pinfo, tree);
            goto dissect_ieee802154_fcs;

        case DECRYPT_PACKET_NO_KEY:
            expert_add_info_format(pinfo, proto_root, &ei_ieee802154_decrypt_error, "No encryption key set - can't decrypt");
            call_data_dissector(payload_tvb, pinfo, tree);
            goto dissect_ieee802154_fcs;

        case DECRYPT_PACKET_DECRYPT_FAILED:
            expert_add_info_format(pinfo, proto_root, &ei_ieee802154_decrypt_error, "Decrypt failed");
            call_data_dissector(payload_tvb, pinfo, tree);
            goto dissect_ieee802154_fcs;

        case DECRYPT_PACKET_MIC_CHECK_FAILED:
            expert_add_info_format(pinfo, proto_root, &ei_ieee802154_decrypt_error, "MIC check failed");
            proto_item_append_text(mic_item, " [incorrect]");
            /
            if (IEEE802154_IS_ENCRYPTED(packet->security_level)) {
                call_data_dissector(payload_tvb, pinfo, tree);
                goto dissect_ieee802154_fcs;
            }
            break;
        }
    }
    /
    else {
        /
        gint            reported_len = tvb_reported_length(tvb)-offset-IEEE802154_FCS_LEN;
        gint            captured_len = tvb_captured_length(tvb)-offset;
        if (reported_len < captured_len) captured_len = reported_len;
        payload_tvb = tvb_new_subset_length_caplen(tvb, offset, captured_len, reported_len);
    }

    /
    offset = 0;
    if (packet->payload_ie_present) {
        offset += dissect_ieee802154_payload_ie(payload_tvb, pinfo, ieee802154_tree, offset);
    }

    if ((packet->version == IEEE802154_VERSION_2015) && (packet->frame_type == IEEE802154_FCF_CMD)) {
        /
        packet->command_id = tvb_get_guint8(payload_tvb, offset);
        if (tree) {
            proto_tree_add_uint(ieee802154_tree, hf_ieee802154_cmd_id, payload_tvb, offset, 1, packet->command_id);
        }
        offset++;

        /
        if ((packet->version == IEEE802154_VERSION_2015) && (packet->command_id == IEEE802154_CMD_BEACON_REQ)) {
            col_set_str(pinfo->cinfo, COL_INFO, "Enhanced Beacon Request");
        }
        else {
            col_set_str(pinfo->cinfo, COL_INFO, val_to_str_const(packet->command_id, ieee802154_cmd_names, "Unknown Command"));
        }
    }

    if (offset > 0) {
      payload_tvb = tvb_new_subset_remaining(payload_tvb, offset);
      offset = 0;
    }

    /
    if ((!ieee802154_fcs_ok
         /
         && !(options & (DISSECT_IEEE802154_OPTION_ZBOSS | DISSECT_IEEE802154_OPTION_CC24xx)))
        || tvb_captured_length(payload_tvb) > 0) {
        /
        saved_proto = pinfo->current_proto;
        /
        TRY {
            switch (packet->frame_type) {
            case IEEE802154_FCF_BEACON:
                if (!dissector_try_heuristic(ieee802154_beacon_subdissector_list, payload_tvb, pinfo, tree, &hdtbl_entry, packet)) {
                    /
                    call_data_dissector(payload_tvb, pinfo, tree);
                }
                break;

            case IEEE802154_FCF_CMD:
                dissect_ieee802154_command(payload_tvb, pinfo, ieee802154_tree, packet);
                break;

            case IEEE802154_FCF_DATA:
                /
                if ((!fcs_ok && ieee802154_fcs_ok) || !tvb_reported_length(payload_tvb)) {
                    call_data_dissector(payload_tvb, pinfo, tree);
                    break;
                }
                if (options & DISSECT_IEEE802154_OPTION_ZBOSS) {
                    call_dissector_with_data(zigbee_nwk_handle, payload_tvb, pinfo, tree, packet);
                    break;
                }
                /
                if (dissector_try_uint_new(panid_dissector_table, packet->src_pan, payload_tvb, pinfo, tree, TRUE, packet)) {
                    break;
                }
                /
                if (((packet->dst_addr_mode == IEEE802154_FCF_ADDR_SHORT) ||
                     (packet->dst_addr_mode == IEEE802154_FCF_ADDR_EXT)) &&
                        (packet->dst_pan != packet->src_pan) &&
                        dissector_try_uint_new(panid_dissector_table, packet->src_pan, payload_tvb, pinfo, tree, TRUE, packet)) {
                    break;
                }
                /
                if (dissector_try_heuristic(ieee802154_heur_subdissector_list, payload_tvb, pinfo, tree, &hdtbl_entry, packet)) break;
                /
                /
            default:
                /
                call_data_dissector(payload_tvb, pinfo, tree);
            } /
        }
        CATCH_ALL {
            /
            show_exception(payload_tvb, pinfo, tree, EXCEPT_CODE, GET_MESSAGE);
            pinfo->current_proto = saved_proto;
        }
        ENDTRY;
    }
    /
dissect_ieee802154_fcs:
    /
    offset = tvb_reported_length(tvb)-IEEE802154_FCS_LEN;
    /
    if (tvb_bytes_exist(tvb, offset, IEEE802154_FCS_LEN) && (tree)) {
        proto_tree  *field_tree;
        guint16     fcs = tvb_get_letohs(tvb, offset);

        /
        if ((options & DISSECT_IEEE802154_OPTION_CC24xx)) {
            /
            field_tree = proto_tree_add_subtree_format(ieee802154_tree, tvb, offset, 2, ett_ieee802154_fcs, NULL,
                        "Frame Check Sequence (TI CC24xx format): FCS %s", (fcs_ok) ? "OK" : "Bad");
            /
            proto_tree_add_int(field_tree, hf_ieee802154_rssi, tvb, offset++, 1, (gint8) (fcs & IEEE802154_CC24xx_RSSI));
            proto_tree_add_boolean(field_tree, hf_ieee802154_fcs_ok, tvb, offset, 1, (gboolean) (fcs & IEEE802154_CC24xx_CRC_OK));
            proto_tree_add_uint(field_tree, hf_ieee802154_correlation, tvb, offset, 1, (guint8) ((fcs & IEEE802154_CC24xx_CORRELATION) >> 8));
        }
        else {
            ti = proto_tree_add_uint(ieee802154_tree, hf_ieee802154_fcs, tvb, offset, 2, fcs);
            if (fcs_ok) {
                proto_item_append_text(ti, " (Correct)");
            }
            else {
                proto_item_append_text(ti, " (Incorrect, expected FCS=0x%04x)", ieee802154_crc_tvb(tvb, offset));
            }
            /
            ti = proto_tree_add_boolean(ieee802154_tree, hf_ieee802154_fcs_ok, tvb, offset, 2, fcs_ok);
            PROTO_ITEM_SET_HIDDEN(ti);
        }
    }
    else if (tree) {
        /
        ti = proto_tree_add_boolean_format_value(ieee802154_tree, hf_ieee802154_fcs_ok, tvb, offset, 2, fcs_ok, "Unknown");
        PROTO_ITEM_SET_HIDDEN(ti);
    }

    /
    if (!fcs_ok) {
        col_append_str(pinfo->cinfo, COL_INFO, ", Bad FCS");
        if (tree) proto_item_append_text(proto_root, ", Bad FCS");

        /
        expert_add_info(pinfo, proto_root, &ei_ieee802154_fcs);
    }
} /
