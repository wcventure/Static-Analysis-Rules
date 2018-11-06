static gint
dissect_btavdtp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data)
{
    proto_item       *ti;
    proto_tree       *btavdtp_tree       = NULL;
    proto_tree       *signal_tree        = NULL;
    proto_item       *signal_item        = NULL;
    btl2cap_data_t   *l2cap_data;
    gint             offset = 0;
    gint             i_sep         = 1;
    gint             packet_type   = 0;
    gint             message_type  = 0;
    gint             signal_id     = 0;
    guint            delay;
    wmem_tree_t      *subtree;
    wmem_tree_key_t  key[8];
    channels_info_t  *channels_info;
    guint32          interface_id;
    guint32          adapter_id;
    guint32          chandle;
    guint32          psm;
    guint32          direction;
    guint32          cid;
    guint32          frame_number;
    sep_entry_t      *sep;
    tvbuff_t         *next_tvb;
    guint32          seid;
    gint             codec = -1;
    gint             content_protection_type = 0;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "AVDTP");

    direction = pinfo->p2p_dir;
    switch (direction) {
        case P2P_DIR_SENT:
            col_set_str(pinfo->cinfo, COL_INFO, "Sent ");
            break;

        case P2P_DIR_RECV:
            col_set_str(pinfo->cinfo, COL_INFO, "Rcvd ");
            break;
        default:
            col_add_fstr(pinfo->cinfo, COL_INFO, "Unknown direction %d ",
                direction);
            goto LABEL_data;
            break;
    }

    l2cap_data = (btl2cap_data_t *) data;
    DISSECTOR_ASSERT(l2cap_data);

    interface_id = l2cap_data->interface_id;
    adapter_id = l2cap_data->adapter_id;
    chandle = l2cap_data->chandle;
    psm = l2cap_data->psm;
    cid = l2cap_data->cid;
    frame_number = pinfo->fd->num;

    key[0].length = 1;
    key[0].key    = &interface_id;
    key[1].length = 1;
    key[1].key    = &adapter_id;
    key[2].length = 1;
    key[2].key    = &chandle;
    key[3].length = 1;
    key[3].key    = &psm;
    key[4].length = 0;
    key[4].key    = NULL;

    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(channels, key);
    channels_info = (subtree) ? (channels_info_t *) wmem_tree_lookup32_le(subtree, frame_number) : NULL;
    if (!(channels_info &&
            ((*channels_info->adapter_disconnect_in_frame >= pinfo->fd->num &&
            *channels_info->hci_disconnect_in_frame >= pinfo->fd->num &&
            *channels_info->l2cap_disconnect_in_frame >= pinfo->fd->num &&
            channels_info->disconnect_in_frame >= pinfo->fd->num) ||
            (*channels_info->adapter_disconnect_in_frame == 0 ||
            *channels_info->hci_disconnect_in_frame == 0 ||
            *channels_info->l2cap_disconnect_in_frame == 0 ||
            channels_info->disconnect_in_frame == 0)))) {

        channels_info = (channels_info_t *) wmem_new (wmem_file_scope(), channels_info_t);
        channels_info->control_local_cid = l2cap_data->local_cid;
        channels_info->control_remote_cid = l2cap_data->remote_cid;
        channels_info->media_local_cid = BTL2CAP_UNKNOWN_CID;
        channels_info->media_remote_cid = BTL2CAP_UNKNOWN_CID;
        channels_info->disconnect_in_frame = G_MAXUINT32;
        channels_info->l2cap_disconnect_in_frame   = l2cap_data->disconnect_in_frame;
        channels_info->hci_disconnect_in_frame     = l2cap_data->hci_disconnect_in_frame;
        channels_info->adapter_disconnect_in_frame = l2cap_data->adapter_disconnect_in_frame;
        channels_info->sep = NULL;

        if (!pinfo->fd->flags.visited || (
                *channels_info->adapter_disconnect_in_frame == 0 ||
                *channels_info->hci_disconnect_in_frame == 0 ||
                *channels_info->l2cap_disconnect_in_frame == 0 ||
                channels_info->disconnect_in_frame == 0)) {
            key[4].length = 1;
            key[4].key    = &frame_number;
            key[5].length = 0;
            key[5].key    = NULL;

            channels_info->stream_numbers = wmem_tree_new(wmem_file_scope());

            if (*channels_info->adapter_disconnect_in_frame > 0 &&
                    *channels_info->hci_disconnect_in_frame > 0 &&
                    *channels_info->l2cap_disconnect_in_frame > 0 &&
                    channels_info->disconnect_in_frame > 0) {
                wmem_tree_insert32_array(channels, key, channels_info);
            }
        } else {
            channels_info->stream_numbers = NULL;
        }
    }

    if (!(l2cap_data->local_cid == channels_info->control_local_cid &&
            l2cap_data->remote_cid == channels_info->control_remote_cid) &&
            (channels_info->media_local_cid == BTL2CAP_UNKNOWN_CID ||
            (l2cap_data->local_cid == channels_info->media_local_cid &&
            l2cap_data->remote_cid == channels_info->media_remote_cid))) {

        if (!pinfo->fd->flags.visited && channels_info->media_local_cid == BTL2CAP_UNKNOWN_CID) {
            channels_info->media_local_cid = l2cap_data->local_cid;
            channels_info->media_remote_cid = l2cap_data->remote_cid;
        }
        /

        if (!channels_info->sep) {
            ti = proto_tree_add_item(tree, proto_btavdtp, tvb, offset, -1, ENC_NA);
            btavdtp_tree = proto_item_add_subtree(ti, ett_btavdtp);

            col_append_fstr(pinfo->cinfo, COL_INFO, "Media stream on cid=0x%04x", l2cap_data->cid);
            proto_tree_add_item(btavdtp_tree, hf_btavdtp_data, tvb, offset, -1, ENC_NA);
        } else {
            col_append_fstr(pinfo->cinfo, COL_INFO, "Media stream ACP SEID [%u - %s %s]",
                    channels_info->sep->seid, get_sep_media_type(
                            interface_id, adapter_id, chandle, direction,
                            channels_info->sep->seid,
                            frame_number),
                    get_sep_type(interface_id, adapter_id, chandle, direction,
                            channels_info->sep->seid,
                            frame_number));

            if (channels_info->sep->media_type == MEDIA_TYPE_AUDIO) {
                sep_data_t                    sep_data;
                media_stream_number_value_t  *media_stream_number_value;
                media_packet_info_t          *previous_media_packet_info;
                media_packet_info_t          *current_media_packet_info;
                nstime_t                      first_abs_ts;
                gdouble                       cummulative_frame_duration;

                sep_data.codec        = channels_info->sep->codec;
                sep_data.vendor_id    = channels_info->sep->vendor_id;
                sep_data.vendor_codec = channels_info->sep->vendor_codec;
                sep_data.acp_seid     = channels_info->sep->seid;
                sep_data.int_seid     = channels_info->sep->int_seid;
                sep_data.content_protection_type = channels_info->sep->content_protection_type;

                media_stream_number_value = (media_stream_number_value_t *) wmem_tree_lookup32_le(channels_info->stream_numbers, frame_number - 1);
                if (media_stream_number_value) {
                    sep_data.stream_number = media_stream_number_value->stream_number;
                } else {
                    sep_data.stream_number = 1;
                }

                key[0].length = 1;
                key[0].key    = &interface_id;
                key[1].length = 1;
                key[1].key    = &adapter_id;
                key[2].length = 1;
                key[2].key    = &chandle;
                key[3].length = 1;
                key[3].key    = &cid;
                key[4].length = 1;
                key[4].key    = &direction;
                key[5].length = 0;
                key[5].key    = NULL;

                subtree = (wmem_tree_t *) wmem_tree_lookup32_array(media_packet_times, key);
                previous_media_packet_info = (subtree) ? (media_packet_info_t *) wmem_tree_lookup32_le(subtree, frame_number - 1) : NULL;
                if (previous_media_packet_info && previous_media_packet_info->stream_number == sep_data.stream_number ) {
                    sep_data.previous_media_packet_info = previous_media_packet_info;
                    first_abs_ts = previous_media_packet_info->first_abs_ts;
                    cummulative_frame_duration = previous_media_packet_info->cummulative_frame_duration;
                } else {
                    first_abs_ts = pinfo->fd->abs_ts;
                    cummulative_frame_duration = 0.0;
                    sep_data.previous_media_packet_info = (media_packet_info_t *) wmem_new(wmem_epan_scope(), media_packet_info_t);
                    sep_data.previous_media_packet_info->abs_ts = pinfo->fd->abs_ts;
                    sep_data.previous_media_packet_info->first_abs_ts = first_abs_ts;
                    sep_data.previous_media_packet_info->cummulative_frame_duration = cummulative_frame_duration;
                    sep_data.previous_media_packet_info->stream_number = sep_data.stream_number;
                }

                if (!pinfo->fd->flags.visited) {
                    key[5].length = 1;
                    key[5].key    = &frame_number;
                    key[6].length = 0;
                    key[6].key    = NULL;

                    current_media_packet_info = wmem_new(wmem_file_scope(), media_packet_info_t);
                    current_media_packet_info->abs_ts = pinfo->fd->abs_ts;
                    current_media_packet_info->first_abs_ts = first_abs_ts;
                    current_media_packet_info->cummulative_frame_duration = cummulative_frame_duration;
                    current_media_packet_info->stream_number = sep_data.stream_number;

                    wmem_tree_insert32_array(media_packet_times, key, current_media_packet_info);
                }

                key[5].length = 0;
                key[5].key    = NULL;

                subtree = (wmem_tree_t *) wmem_tree_lookup32_array(media_packet_times, key);
                current_media_packet_info = (subtree) ? (media_packet_info_t *) wmem_tree_lookup32(subtree, frame_number) : NULL;
                if (current_media_packet_info)
                    sep_data.current_media_packet_info = current_media_packet_info;
                else
                    sep_data.current_media_packet_info = NULL;

                next_tvb = tvb_new_subset_remaining(tvb, offset);
                call_dissector_with_data(bta2dp_handle, next_tvb, pinfo, tree, &sep_data);
            } else if (channels_info->sep->media_type == MEDIA_TYPE_VIDEO) {
                sep_data_t  sep_data;

                sep_data.codec    = channels_info->sep->codec;
                sep_data.acp_seid = channels_info->sep->seid;
                sep_data.int_seid = channels_info->sep->int_seid;
                sep_data.content_protection_type = channels_info->sep->content_protection_type;

                next_tvb = tvb_new_subset_remaining(tvb, offset);
                call_dissector_with_data(btvdp_handle, next_tvb, pinfo, tree, &sep_data);
            } else {
                ti = proto_tree_add_item(tree, proto_btavdtp, tvb, offset, -1, ENC_NA);
                btavdtp_tree = proto_item_add_subtree(ti, ett_btavdtp);

                col_append_fstr(pinfo->cinfo, COL_INFO, "Media stream on cid=0x%04x", l2cap_data->cid);
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_data, tvb, offset, -1, ENC_NA);
            }
        }

        return tvb_length(tvb);
    } else if (!(l2cap_data->local_cid == channels_info->control_local_cid &&
            l2cap_data->remote_cid == channels_info->control_remote_cid)) {
        /
        ti = proto_tree_add_item(tree, proto_btavdtp, tvb, offset, -1, ENC_NA);
        btavdtp_tree = proto_item_add_subtree(ti, ett_btavdtp);

        col_append_fstr(pinfo->cinfo, COL_INFO, "Unknown channel stream on cid=0x%04x", l2cap_data->cid);
        proto_tree_add_item(btavdtp_tree, hf_btavdtp_data, tvb, offset, -1, ENC_NA);
        return tvb_length(tvb);
    }

    /
    ti = proto_tree_add_item(tree, proto_btavdtp, tvb, offset, -1, ENC_NA);
    btavdtp_tree = proto_item_add_subtree(ti, ett_btavdtp);

    /
    message_type = (tvb_get_guint8(tvb, offset) & AVDTP_MESSAGE_TYPE_MASK);
    packet_type = (tvb_get_guint8(tvb, offset) & AVDTP_PACKET_TYPE_MASK) >> 2;

    signal_item = proto_tree_add_item(btavdtp_tree, hf_btavdtp_signal, tvb, offset,
            (packet_type == PACKET_TYPE_START) ? 3 : 2, ENC_NA);
    signal_tree = proto_item_add_subtree(signal_item, ett_btavdtp_sep);

    proto_tree_add_item(signal_tree, hf_btavdtp_transaction, tvb, offset, 1, ENC_NA);
    proto_tree_add_item(signal_tree, hf_btavdtp_packet_type, tvb, offset, 1, ENC_NA);
    proto_tree_add_item(signal_tree, hf_btavdtp_message_type, tvb, offset, 1, ENC_NA);

    if (packet_type == PACKET_TYPE_START) {
        offset += 1;
        proto_tree_add_item(signal_tree, hf_btavdtp_number_of_signal_packets, tvb, offset, 1, ENC_NA);
    }

    if (packet_type == PACKET_TYPE_CONTINUE || packet_type == PACKET_TYPE_END) goto LABEL_data;

    offset += 1;
    proto_tree_add_item(signal_tree, hf_btavdtp_rfa0,         tvb, offset, 1, ENC_NA);
    proto_tree_add_item(signal_tree, hf_btavdtp_signal_id,    tvb, offset, 1, ENC_NA);

    signal_id   = tvb_get_guint8(tvb, offset) & AVDTP_SIGNAL_ID_MASK;
    proto_item_append_text(signal_item, ": %s (%s)",
            val_to_str_const(signal_id, signal_id_vals, "Unknown signal"),
            val_to_str_const(message_type, message_type_vals, "Unknown message type"));

    col_append_fstr(pinfo->cinfo, COL_INFO, "%s - %s",
                    val_to_str_const(message_type, message_type_vals, "Unknown message type"),
                    val_to_str_const(signal_id, signal_id_vals, "Unknown signal"));

    offset += 1;
    if (message_type != MESSAGE_TYPE_GENERAL_REJECT) switch (signal_id) {
        case SIGNAL_ID_DISCOVER:
            if (message_type == MESSAGE_TYPE_COMMAND) break;
            if (message_type == MESSAGE_TYPE_REJECT) {
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }
            offset = dissect_sep(tvb, pinfo, btavdtp_tree, offset,
                    interface_id, adapter_id, chandle);
            break;
        case SIGNAL_ID_GET_CAPABILITIES:
        case SIGNAL_ID_GET_ALL_CAPABILITIES:
            if (message_type == MESSAGE_TYPE_COMMAND) {
                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, NULL, interface_id,
                        adapter_id, chandle, frame_number);
                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }
            offset = dissect_capabilities(tvb, pinfo, btavdtp_tree, offset, NULL, NULL, NULL, NULL);
            break;
        case SIGNAL_ID_SET_CONFIGURATION:
            if (message_type == MESSAGE_TYPE_COMMAND) {
                guint32  int_seid;
                guint32  vendor_id;
                guint16  vendor_codec;

                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, &seid, interface_id,
                        adapter_id, chandle, frame_number);
                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_INT, 0, &int_seid, interface_id,
                        adapter_id, chandle, frame_number);
                offset = dissect_capabilities(tvb, pinfo, btavdtp_tree, offset,
                        &codec, &content_protection_type, &vendor_id, &vendor_codec);

                key[0].length = 1;
                key[0].key    = &interface_id;
                key[1].length = 1;
                key[1].key    = &adapter_id;
                key[2].length = 1;
                key[2].key    = &chandle;
                key[3].length = 1;
                key[3].key    = &direction;
                key[4].length = 1;
                key[4].key    = &seid;
                key[5].length = 0;
                key[5].key    = NULL;

                subtree = (wmem_tree_t *) wmem_tree_lookup32_array(sep_list, key);
                sep = (subtree) ? (sep_entry_t *) wmem_tree_lookup32_le(subtree, frame_number) : NULL;
                if (sep) {
                    sep->codec = codec;
                    sep->vendor_id = vendor_id;
                    sep->vendor_codec = vendor_codec;
                    sep->content_protection_type = content_protection_type;
                    sep->int_seid = int_seid;
                }

                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_service_category, tvb, offset, 1, ENC_NA);
                offset += 1;

                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }
            break;
        case SIGNAL_ID_GET_CONFIGURATION:
            if (message_type == MESSAGE_TYPE_COMMAND) {
                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, NULL, interface_id,
                        adapter_id, chandle, frame_number);
                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }
            offset = dissect_capabilities(tvb, pinfo, btavdtp_tree, offset, NULL, NULL, NULL, NULL);
            break;
        case SIGNAL_ID_RECONFIGURE:
            if (message_type == MESSAGE_TYPE_COMMAND) {
                guint32  vendor_id;
                guint16  vendor_codec;

                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, &seid, interface_id,
                        adapter_id, chandle, frame_number);
                offset = dissect_capabilities(tvb, pinfo, btavdtp_tree, offset,
                        &codec, &content_protection_type, &vendor_id, &vendor_codec);

                key[0].length = 1;
                key[0].key    = &interface_id;
                key[1].length = 1;
                key[1].key    = &adapter_id;
                key[2].length = 1;
                key[2].key    = &chandle;
                key[3].length = 1;
                key[3].key    = &direction;
                key[4].length = 1;
                key[4].key    = &seid;
                key[5].length = 0;
                key[5].key    = NULL;

                subtree = (wmem_tree_t *) wmem_tree_lookup32_array(sep_list, key);
                sep = (subtree) ? (sep_entry_t *) wmem_tree_lookup32_le(subtree, frame_number) : NULL;
                if (sep) {
                    sep->codec = codec;
                    sep->vendor_id = vendor_id;
                    sep->vendor_codec = vendor_codec;
                    sep->content_protection_type = content_protection_type;
                }

                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_service_category, tvb, offset, 1, ENC_NA);
                offset += 1;

                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }
            break;
        case SIGNAL_ID_OPEN:
             if (message_type == MESSAGE_TYPE_COMMAND) {
                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, &seid, interface_id,
                        adapter_id, chandle, frame_number);

                if (!pinfo->fd->flags.visited) {
                    key[0].length = 1;
                    key[0].key    = &interface_id;
                    key[1].length = 1;
                    key[1].key    = &adapter_id;
                    key[2].length = 1;
                    key[2].key    = &chandle;
                    key[3].length = 1;
                    key[3].key    = &direction;
                    key[4].length = 1;
                    key[4].key    = &seid;
                    key[5].length = 0;
                    key[5].key    = NULL;

                    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(sep_list, key);
                    sep = (subtree) ? (sep_entry_t *) wmem_tree_lookup32_le(subtree, frame_number) : NULL;
                    if (sep) {
                        sep->state = SEP_STATE_OPEN;

                        key[3].length = 1;
                        key[3].key    = &frame_number;
                        key[4].length = 0;
                        key[4].key    = NULL;

                        wmem_tree_insert32_array(sep_open, key, sep);
                    }
                }
                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }
            if (message_type == MESSAGE_TYPE_ACCEPT && !pinfo->fd->flags.visited) {

                key[0].length = 1;
                key[0].key    = &interface_id;
                key[1].length = 1;
                key[1].key    = &adapter_id;
                key[2].length = 1;
                key[2].key    = &chandle;
                key[3].length = 0;
                key[3].key    = NULL;

                subtree = (wmem_tree_t *) wmem_tree_lookup32_array(sep_open, key);
                sep = (subtree) ? (sep_entry_t *) wmem_tree_lookup32_le(subtree, frame_number) : NULL;
                if (sep && sep->state == SEP_STATE_OPEN) {
                    sep->state = SEP_STATE_IN_USE;
                    channels_info->sep = sep;
                }
            }
            break;
        case SIGNAL_ID_START:
            if (message_type == MESSAGE_TYPE_COMMAND) {
                i_sep = 1;
                while (tvb_length_remaining(tvb, offset) > 0) {
                    offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                            SEID_ACP, i_sep, NULL,
                            interface_id, adapter_id, chandle, frame_number);
                    i_sep += 1;
                }
                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, NULL,
                        interface_id, adapter_id, chandle, frame_number);
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }

            if (message_type == MESSAGE_TYPE_ACCEPT && !pinfo->fd->flags.visited) {
                media_stream_number_value_t  *media_stream_number_value;
                guint32                       stream_number = 0;

                media_stream_number_value = (media_stream_number_value_t *) wmem_tree_lookup32_le(channels_info->stream_numbers, frame_number - 1);
#if RTP_PLAYER_WORKAROUND == TRUE
                {
                    media_stream_number_value_t  *file_scope_stream_number_value;

                    if (media_stream_number_value) {
                        stream_number = media_stream_number_value->stream_number;
                    } else {
                        file_scope_stream_number_value = (media_stream_number_value_t *) wmem_tree_lookup32_le(file_scope_stream_number, frame_number - 1);
                        if (file_scope_stream_number_value)
                            stream_number = file_scope_stream_number_value->stream_number + 1;
                        else
                            stream_number = 0;
                    }

                    file_scope_stream_number_value = wmem_new(wmem_file_scope(), media_stream_number_value_t);
                    file_scope_stream_number_value->stream_number = stream_number;
                    wmem_tree_insert32(file_scope_stream_number, frame_number, file_scope_stream_number_value);
                }
#else
                if (media_stream_number_value)
                    stream_number = media_stream_number_value->stream_number;
                else
                    stream_number = 0;
#endif

                media_stream_number_value = wmem_new(wmem_file_scope(), media_stream_number_value_t);
                media_stream_number_value->stream_number = stream_number + 1;

                wmem_tree_insert32(channels_info->stream_numbers, frame_number, media_stream_number_value);
            }
            break;
        case SIGNAL_ID_CLOSE:
            if (message_type == MESSAGE_TYPE_COMMAND) {
                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, NULL, interface_id,
                        adapter_id, chandle, frame_number);
                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }
            if (!pinfo->fd->flags.visited && message_type == MESSAGE_TYPE_ACCEPT &&
                    channels_info->disconnect_in_frame > pinfo->fd->num) {
                channels_info->disconnect_in_frame = pinfo->fd->num;
            }
            break;
        case SIGNAL_ID_SUSPEND:
            if (message_type == MESSAGE_TYPE_COMMAND) {
                i_sep = 1;
                while (tvb_length_remaining(tvb, offset) > 0) {
                    offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                            SEID_ACP, i_sep, NULL,
                            interface_id, adapter_id, chandle, frame_number);
                    i_sep += 1;
                }
                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, NULL, interface_id,
                        adapter_id, chandle, frame_number);
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }
            break;
        case SIGNAL_ID_ABORT:
            if (message_type == MESSAGE_TYPE_COMMAND) {
                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, NULL, interface_id,
                        adapter_id, chandle, frame_number);
                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }
            if (!pinfo->fd->flags.visited && message_type == MESSAGE_TYPE_ACCEPT &&
                    channels_info->disconnect_in_frame > pinfo->fd->num) {
                channels_info->disconnect_in_frame = pinfo->fd->num;
            }
            break;
        case SIGNAL_ID_SECURITY_CONTROL:
            if (message_type == MESSAGE_TYPE_COMMAND) {
                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, NULL, interface_id,
                        adapter_id, chandle, frame_number);
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_data, tvb, offset, -1, ENC_NA);
                offset += tvb_length_remaining(tvb, offset);
                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }

            proto_tree_add_item(btavdtp_tree, hf_btavdtp_data, tvb, offset, -1, ENC_NA);
            offset += tvb_length_remaining(tvb, offset);
            break;
        case SIGNAL_ID_DELAY_REPORT:
            if (message_type == MESSAGE_TYPE_COMMAND) {
                proto_item  *pitem;
                delay = tvb_get_ntohs(tvb, offset + 1);
                col_append_fstr(pinfo->cinfo, COL_INFO, "(%u.%u ms)", delay/10, delay%10);
                offset = dissect_seid(tvb, pinfo, btavdtp_tree, offset,
                        SEID_ACP, 0, NULL,
                        interface_id, adapter_id, chandle, frame_number);
                pitem = proto_tree_add_item(btavdtp_tree, hf_btavdtp_delay, tvb, offset, 2, ENC_BIG_ENDIAN);
                proto_item_append_text(pitem, " (1/10 ms)");
                offset += 2;
                break;
            }
            if (message_type == MESSAGE_TYPE_REJECT) {
                proto_tree_add_item(btavdtp_tree, hf_btavdtp_error_code, tvb, offset, 1, ENC_NA);
                offset += 1;
                break;
            }
            break;
    }

    LABEL_data:

    if (tvb_length_remaining(tvb, offset) > 0) {
        proto_tree_add_item(btavdtp_tree, hf_btavdtp_data, tvb, offset, -1, ENC_NA);
    }

    return offset;
}
