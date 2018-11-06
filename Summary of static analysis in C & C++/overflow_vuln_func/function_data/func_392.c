static gint
dissect_btle(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data)
{
    proto_item           *btle_item;
    proto_tree           *btle_tree;
    proto_item           *sub_item;
    proto_tree           *sub_tree;
    gint                  offset = 0;
    guint32               access_address;
    guint8                length;
    tvbuff_t              *next_tvb;
    guint8                *dst_bd_addr;
    guint8                *src_bd_addr;
    connection_address_t  *connection_address = NULL;
    wmem_tree_t           *wmem_tree;
    wmem_tree_key_t        key[5];
    guint32                interface_id;
    guint32                adapter_id;
    guint32                connection_access_address;
    guint32                frame_number;
    enum {CRC_INDETERMINATE,
          CRC_CAN_BE_CALCULATED,
          CRC_INCORRECT,
          CRC_CORRECT} crc_status = CRC_INDETERMINATE;
    guint32      crc_init = 0x555555; /
    guint32      packet_crc;
    const btle_context_t  *btle_context = (const btle_context_t *) data;
    bluetooth_data_t      *bluetooth_data = NULL;
    ubertooth_data_t      *ubertooth_data = NULL;

    if (btle_context)
        bluetooth_data = btle_context->previous_protocol_data.bluetooth_data;
    if (bluetooth_data)
        ubertooth_data = bluetooth_data->previous_protocol_data.ubertooth_data;

    src_bd_addr = (gchar *) wmem_alloc(pinfo->pool, 6);
    dst_bd_addr = (gchar *) wmem_alloc(pinfo->pool, 6);

    if (btle_context && btle_context->crc_checked_at_capture) {
        crc_status = btle_context->crc_valid_at_capture ? CRC_CORRECT : CRC_INCORRECT;
    }

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "LE LL");

    btle_item = proto_tree_add_item(tree, proto_btle, tvb, offset, -1, ENC_NA);
    btle_tree = proto_item_add_subtree(btle_item, ett_btle);

    sub_item = proto_tree_add_item(btle_tree, hf_access_address, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    access_address = tvb_get_letohl(tvb, offset);
    if (btle_context) {
        switch(btle_context->aa_category) {
        case E_AA_MATCHED:
            expert_add_info(pinfo, sub_item, &ei_access_address_matched);
            break;
        case E_AA_ILLEGAL:
            expert_add_info(pinfo, sub_item, &ei_access_address_illegal);
            break;
        case E_AA_BIT_ERRORS:
            expert_add_info(pinfo, sub_item, &ei_access_address_bit_errors);
            break;
        default:
            break;
        }
    }
    offset += 4;

    if (bluetooth_data)
        interface_id = bluetooth_data->interface_id;
    else if (pinfo->phdr->presence_flags & WTAP_HAS_INTERFACE_ID)
        interface_id = pinfo->phdr->interface_id;
    else
        interface_id = HCI_INTERFACE_DEFAULT;

    if (bluetooth_data)
        adapter_id = bluetooth_data->adapter_id;
    else if (ubertooth_data)
        adapter_id = ubertooth_data->bus_id << 8 | ubertooth_data->device_address;
    else
        adapter_id = HCI_ADAPTER_DEFAULT;

    frame_number = pinfo->fd->num;

    if (access_address == ACCESS_ADDRESS_ADVERTISING) {
        proto_item  *advertising_header_item;
        proto_tree  *advertising_header_tree;
        proto_item  *link_layer_data_item;
        proto_tree  *link_layer_data_tree;
        guint8       pdu_type;

        if (crc_status == CRC_INDETERMINATE) {
            /
            crc_status = CRC_CAN_BE_CALCULATED;
        }

        advertising_header_item = proto_tree_add_item(btle_tree, hf_advertising_header, tvb, offset, 2, ENC_LITTLE_ENDIAN);
        advertising_header_tree = proto_item_add_subtree(advertising_header_item, ett_advertising_header);

        pdu_type = tvb_get_guint8(tvb, offset) & 0x0F;
        proto_tree_add_item(advertising_header_tree, hf_advertising_header_rfu_1, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(advertising_header_tree, hf_advertising_header_randomized_tx, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        switch (pdu_type) {
        case 0x00: /
        case 0x02: /
        case 0x04: /
        case 0x06: /
            proto_tree_add_item(advertising_header_tree, hf_advertising_header_reserved, tvb, offset, 1, ENC_LITTLE_ENDIAN);
            break;
        default:
            proto_tree_add_item(advertising_header_tree, hf_advertising_header_randomized_rx, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        }
        proto_tree_add_item(advertising_header_tree, hf_advertising_header_pdu_type, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        proto_item_append_text(advertising_header_item, " (PDU Type: %s, TxAdd=%s, RxAdd=%s)",
                val_to_str_ext_const(pdu_type, &pdu_type_vals_ext, "Unknown"),
                (tvb_get_guint8(tvb, offset) & 0x20) ? "true" : "false",
                (tvb_get_guint8(tvb, offset) & 0x10) ? "true" : "false");
        offset += 1;

        col_set_str(pinfo->cinfo, COL_INFO, val_to_str_ext_const(pdu_type, &pdu_type_vals_ext, "Unknown"));

        proto_tree_add_item(advertising_header_tree, hf_advertising_header_rfu_2, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(advertising_header_tree, hf_advertising_header_length, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        length = tvb_get_guint8(tvb, offset) & 0x3f;
        offset += 1;

        switch (pdu_type) {
        case 0x00: /
        case 0x02: /
        case 0x06: /
            offset = dissect_bd_addr(hf_advertising_address, btle_tree, tvb, offset, src_bd_addr);

            SET_ADDRESS(&pinfo->net_src, AT_ETHER, 6, src_bd_addr);
            COPY_ADDRESS_SHALLOW(&pinfo->dl_src, &pinfo->net_src);
            COPY_ADDRESS_SHALLOW(&pinfo->src, &pinfo->net_src);

            SET_ADDRESS(&pinfo->net_dst, AT_STRINGZ, 10, "broadcast");
            COPY_ADDRESS_SHALLOW(&pinfo->dl_dst, &pinfo->net_dst);
            COPY_ADDRESS_SHALLOW(&pinfo->dst, &pinfo->net_dst);

            if (!pinfo->fd->flags.visited) {
                address *addr;

                addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_src, sizeof(address));
                addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_src.data, pinfo->dl_src.len);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_SRC, addr);

                addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_dst, sizeof(address));
                addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_dst.data, pinfo->dl_dst.len);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_DST, addr);
            }

            if (tvb_reported_length_remaining(tvb, offset) > 3) {
                next_tvb = tvb_new_subset_length(tvb, offset, tvb_reported_length_remaining(tvb, offset) - 3);
                call_dissector(btcommon_ad_handle, next_tvb, pinfo, btle_tree);
            }

            offset += tvb_reported_length_remaining(tvb, offset) - 3;

            break;
        case 0x01: /
            offset = dissect_bd_addr(hf_advertising_address, btle_tree, tvb, offset, src_bd_addr);
            offset = dissect_bd_addr(hf_initiator_addresss, btle_tree, tvb, offset, dst_bd_addr);

            SET_ADDRESS(&pinfo->net_src, AT_ETHER, 6, src_bd_addr);
            COPY_ADDRESS_SHALLOW(&pinfo->dl_src, &pinfo->net_src);
            COPY_ADDRESS_SHALLOW(&pinfo->src, &pinfo->net_src);

            SET_ADDRESS(&pinfo->net_dst, AT_ETHER, 6, dst_bd_addr);
            COPY_ADDRESS_SHALLOW(&pinfo->dl_dst, &pinfo->net_dst);
            COPY_ADDRESS_SHALLOW(&pinfo->dst, &pinfo->net_dst);

            if (!pinfo->fd->flags.visited) {
                address *addr;

                addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_src, sizeof(address));
                addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_src.data, pinfo->dl_src.len);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_SRC, addr);

                addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_dst, sizeof(address));
                addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_dst.data, pinfo->dl_dst.len);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_DST, addr);
            }

            break;
        case 0x03: /
            offset = dissect_bd_addr(hf_scanning_address, btle_tree, tvb, offset, src_bd_addr);
            offset = dissect_bd_addr(hf_advertising_address, btle_tree, tvb, offset, dst_bd_addr);

            SET_ADDRESS(&pinfo->net_src, AT_ETHER, 6, src_bd_addr);
            COPY_ADDRESS_SHALLOW(&pinfo->dl_src, &pinfo->net_src);
            COPY_ADDRESS_SHALLOW(&pinfo->src, &pinfo->net_src);

            SET_ADDRESS(&pinfo->net_dst, AT_ETHER, 6, dst_bd_addr);
            COPY_ADDRESS_SHALLOW(&pinfo->dl_dst, &pinfo->net_dst);
            COPY_ADDRESS_SHALLOW(&pinfo->dst, &pinfo->net_dst);

            if (!pinfo->fd->flags.visited) {
                address *addr;

                addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_src, sizeof(address));
                addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_src.data, pinfo->dl_src.len);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_SRC, addr);

                addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_dst, sizeof(address));
                addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_dst.data, pinfo->dl_dst.len);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_DST, addr);
            }

            break;
        case 0x04: /
            offset = dissect_bd_addr(hf_advertising_address, btle_tree, tvb, offset, src_bd_addr);

            SET_ADDRESS(&pinfo->net_src, AT_ETHER, 6, src_bd_addr);
            COPY_ADDRESS_SHALLOW(&pinfo->dl_src, &pinfo->net_src);
            COPY_ADDRESS_SHALLOW(&pinfo->src, &pinfo->net_src);

            SET_ADDRESS(&pinfo->net_dst, AT_STRINGZ, 10, "broadcast");
            COPY_ADDRESS_SHALLOW(&pinfo->dl_dst, &pinfo->net_dst);
            COPY_ADDRESS_SHALLOW(&pinfo->dst, &pinfo->net_dst);

            if (!pinfo->fd->flags.visited) {
                address *addr;

                addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_src, sizeof(address));
                addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_src.data, pinfo->dl_src.len);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_SRC, addr);

                addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_dst, sizeof(address));
                addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_dst.data, pinfo->dl_dst.len);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_DST, addr);
            }

            sub_item = proto_tree_add_item(btle_tree, hf_scan_response_data, tvb, offset, tvb_reported_length_remaining(tvb, offset) - 3, ENC_NA);
            sub_tree = proto_item_add_subtree(sub_item, ett_scan_response_data);

            if (tvb_reported_length_remaining(tvb, offset) > 3) {
                next_tvb = tvb_new_subset_length(tvb, offset, tvb_reported_length_remaining(tvb, offset) - 3);
                call_dissector(btcommon_ad_handle, next_tvb, pinfo, sub_tree);
            }

            offset += tvb_reported_length_remaining(tvb, offset) - 3;

            break;
        case 0x05: /
            offset = dissect_bd_addr(hf_initiator_addresss, btle_tree, tvb, offset, src_bd_addr);
            offset = dissect_bd_addr(hf_advertising_address, btle_tree, tvb, offset, dst_bd_addr);

            SET_ADDRESS(&pinfo->net_src, AT_ETHER, 6, src_bd_addr);
            COPY_ADDRESS_SHALLOW(&pinfo->dl_src, &pinfo->net_src);
            COPY_ADDRESS_SHALLOW(&pinfo->src, &pinfo->net_src);

            SET_ADDRESS(&pinfo->net_dst, AT_ETHER, 6, dst_bd_addr);
            COPY_ADDRESS_SHALLOW(&pinfo->dl_dst, &pinfo->net_dst);
            COPY_ADDRESS_SHALLOW(&pinfo->dst, &pinfo->net_dst);

            if (!pinfo->fd->flags.visited) {
                address *addr;

                addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_src, sizeof(address));
                addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_src.data, pinfo->dl_src.len);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_SRC, addr);

                addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_dst, sizeof(address));
                addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_dst.data, pinfo->dl_dst.len);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_DST, addr);
            }

            link_layer_data_item = proto_tree_add_item(btle_tree, hf_link_layer_data, tvb, offset, 22, ENC_NA);
            link_layer_data_tree = proto_item_add_subtree(link_layer_data_item, ett_link_layer_data);

            proto_tree_add_item(link_layer_data_tree, hf_link_layer_data_access_address, tvb, offset, 4, ENC_LITTLE_ENDIAN);
            connection_access_address = tvb_get_letohl(tvb, offset);
            offset += 4;

            proto_tree_add_item(link_layer_data_tree, hf_link_layer_data_crc_init, tvb, offset, 3, ENC_LITTLE_ENDIAN);
            offset += 3;

            proto_tree_add_item(link_layer_data_tree, hf_link_layer_data_window_size, tvb, offset, 1, ENC_LITTLE_ENDIAN);
            offset += 1;

            proto_tree_add_item(link_layer_data_tree, hf_link_layer_data_window_offset, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            offset += 2;

            proto_tree_add_item(link_layer_data_tree, hf_link_layer_data_interval, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            offset += 2;

            proto_tree_add_item(link_layer_data_tree, hf_link_layer_data_latency, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            offset += 2;

            proto_tree_add_item(link_layer_data_tree, hf_link_layer_data_timeout, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            offset += 2;

            sub_item = proto_tree_add_item(link_layer_data_tree, hf_link_layer_data_channel_map, tvb, offset, 5, ENC_NA);
            sub_tree = proto_item_add_subtree(sub_item, ett_channel_map);

            call_dissector(btcommon_le_channel_map_handle, tvb_new_subset_length(tvb, offset, 5), pinfo, sub_tree);
            offset += 5;

            proto_tree_add_item(link_layer_data_tree, hf_link_layer_data_hop, tvb, offset, 1, ENC_LITTLE_ENDIAN);
            proto_tree_add_item(link_layer_data_tree, hf_link_layer_data_sleep_clock_accuracy, tvb, offset, 1, ENC_LITTLE_ENDIAN);
            offset += 1;

            if (!pinfo->fd->flags.visited) {
                key[0].length = 1;
                key[0].key = &interface_id;
                key[1].length = 1;
                key[1].key = &adapter_id;
                key[2].length = 1;
                key[2].key = &connection_access_address;
                key[3].length = 1;
                key[3].key = &frame_number;
                key[4].length = 0;
                key[4].key = NULL;

                connection_address = wmem_new(wmem_file_scope(), connection_address_t);
                connection_address->interface_id   = interface_id;
                connection_address->adapter_id     = adapter_id;
                connection_address->access_address = connection_access_address;

                memcpy(connection_address->master_bd_addr, src_bd_addr, 6);
                memcpy(connection_address->slave_bd_addr,  dst_bd_addr, 6);

                wmem_tree_insert32_array(connection_addresses, key, connection_address);
            }

            break;
        default:
            if (tvb_reported_length_remaining(tvb, offset) > 3) {
                proto_tree_add_expert(btle_tree, pinfo, &ei_unknown_data, tvb, offset, tvb_reported_length_remaining(tvb, offset) - 3);
                offset += tvb_reported_length_remaining(tvb, offset) - 3;
            }
        }
    } else { /
        proto_item  *data_header_item;
        proto_tree  *data_header_tree;
        guint8       llid;
        guint8       control_opcode;

        key[0].length = 1;
        key[0].key = &interface_id;
        key[1].length = 1;
        key[1].key = &adapter_id;
        key[2].length = 1;
        key[2].key = &access_address;
        key[3].length = 0;
        key[3].key = NULL;

        wmem_tree = (wmem_tree_t *) wmem_tree_lookup32_array(connection_addresses, key);
        if (wmem_tree) {
            connection_address = (connection_address_t *) wmem_tree_lookup32_le(wmem_tree, pinfo->fd->num);
            if (connection_address) {
                gchar  *str_addr;
                int     str_addr_len = 18 + 1;

                str_addr = (gchar *) wmem_alloc(pinfo->pool, str_addr_len);

                sub_item = proto_tree_add_ether(btle_tree, hf_master_bd_addr, tvb, 0, 0, connection_address->master_bd_addr);
                PROTO_ITEM_SET_GENERATED(sub_item);

                sub_item = proto_tree_add_ether(btle_tree, hf_slave_bd_addr, tvb, 0, 0, connection_address->slave_bd_addr);
                PROTO_ITEM_SET_GENERATED(sub_item);

                g_snprintf(str_addr, str_addr_len, "unknown_0x%08x", connection_address->access_address);

                SET_ADDRESS(&pinfo->net_src, AT_STRINGZ, str_addr_len, str_addr);
                COPY_ADDRESS_SHALLOW(&pinfo->dl_src, &pinfo->net_src);
                COPY_ADDRESS_SHALLOW(&pinfo->src, &pinfo->net_src);

                SET_ADDRESS(&pinfo->net_dst, AT_STRINGZ, str_addr_len, str_addr);
                COPY_ADDRESS_SHALLOW(&pinfo->dl_dst, &pinfo->net_dst);
                COPY_ADDRESS_SHALLOW(&pinfo->dst, &pinfo->net_dst);

                if (!pinfo->fd->flags.visited) {
                    address *addr;

                    addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_src, sizeof(address));
                    addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_src.data, pinfo->dl_src.len);
                    p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_SRC, addr);

                    addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_dst, sizeof(address));
                    addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_dst.data, pinfo->dl_dst.len);
                    p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_DST, addr);
                }
            }
        }

        data_header_item = proto_tree_add_item(btle_tree, hf_data_header, tvb, offset, 2, ENC_LITTLE_ENDIAN);
        data_header_tree = proto_item_add_subtree(data_header_item, ett_data_header);

        proto_tree_add_item(data_header_tree, hf_data_header_rfu, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(data_header_tree, hf_data_header_more_data, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(data_header_tree, hf_data_header_sequence_number, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(data_header_tree, hf_data_header_next_expected_sequence_number, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(data_header_tree, hf_data_header_llid, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        llid = tvb_get_guint8(tvb, offset) & 0x03;
        offset += 1;

        proto_tree_add_item(data_header_tree, hf_data_header_rfu, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(data_header_tree, hf_data_header_length, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        length = tvb_get_guint8(tvb, offset) & 0x1f;
        offset += 1;

        switch (llid) {
        case 0x01: /
/
            if (length > 0) {
                col_set_str(pinfo->cinfo, COL_INFO, "L2CAP Fragment");
                proto_tree_add_item(btle_tree, hf_l2cap_fragment, tvb, offset, length, ENC_NA);
                offset += length;
            } else {
                col_set_str(pinfo->cinfo, COL_INFO, "Empty PDU");
            }

            break;
        case 0x02: /
            if (length > 0) {
                if (tvb_get_letohs(tvb, offset) > length) {
/
                    col_set_str(pinfo->cinfo, COL_INFO, "L2CAP Fragment");
                    proto_tree_add_item(btle_tree, hf_l2cap_fragment, tvb, offset, length, ENC_NA);
                    offset += length;
                } else {
                    bthci_acl_data_t  *acl_data;
                    gint               saved_p2p_dir;

                    col_set_str(pinfo->cinfo, COL_INFO, "L2CAP Data");

                    acl_data = wmem_new(wmem_packet_scope(), bthci_acl_data_t);
                    acl_data->interface_id = interface_id;
                    acl_data->adapter_id   = adapter_id;
                    acl_data->chandle      = 0; /
                    acl_data->remote_bd_addr_oui = 0;
                    acl_data->remote_bd_addr_id  = 0;

                    saved_p2p_dir = pinfo->p2p_dir;
                    pinfo->p2p_dir = P2P_DIR_UNKNOWN;

                    next_tvb = tvb_new_subset_length(tvb, offset, length);
                    call_dissector_with_data(btl2cap_handle, next_tvb, pinfo, tree, acl_data);
                    offset += length;

                    pinfo->p2p_dir = saved_p2p_dir;
                }
            }
            break;
        case 0x03: /
            proto_tree_add_item(btle_tree, hf_control_opcode, tvb, offset, 1, ENC_LITTLE_ENDIAN);
            control_opcode = tvb_get_guint8(tvb, offset);
            offset += 1;

            col_add_fstr(pinfo->cinfo, COL_INFO, "Control Opcode: %s",
                    val_to_str_ext_const(control_opcode, &control_opcode_vals_ext, "Unknown"));

            switch (control_opcode) {
            case 0x05: /
            case 0x06: /
            case 0x0A: /
            case 0x0B: /
            case 0x12: /
            case 0x13: /
                if (tvb_reported_length_remaining(tvb, offset) > 3) {
                    proto_tree_add_expert(btle_tree, pinfo, &ei_unknown_data, tvb, offset, tvb_reported_length_remaining(tvb, offset) - 3);
                    offset += tvb_reported_length_remaining(tvb, offset) - 3;
                }

                break;
            case 0x00: /
                proto_tree_add_item(btle_tree, hf_control_window_size, tvb, offset, 1, ENC_LITTLE_ENDIAN);
                offset += 1;

                proto_tree_add_item(btle_tree, hf_control_window_offset, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_interval, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_latency, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_timeout, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_instant, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                break;
            case 0x01: /
                sub_item = proto_tree_add_item(btle_tree, hf_control_channel_map, tvb, offset, 5, ENC_NA);
                sub_tree = proto_item_add_subtree(sub_item, ett_channel_map);

                call_dissector(btcommon_le_channel_map_handle, tvb_new_subset_length(tvb, offset, 5), pinfo, sub_tree);
                offset += 5;

                proto_tree_add_item(btle_tree, hf_control_instant, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                break;
            case 0x02: /
            case 0x0D: /
                proto_tree_add_item(btle_tree, hf_control_error_code, tvb, offset, 1, ENC_LITTLE_ENDIAN);
                offset += 1;

                break;
            case 0x03: /
                proto_tree_add_item(btle_tree, hf_control_random_number, tvb, offset, 8, ENC_LITTLE_ENDIAN);
                offset += 8;

                proto_tree_add_item(btle_tree, hf_control_encrypted_diversifier, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_master_session_key_diversifier, tvb, offset, 8, ENC_LITTLE_ENDIAN);
                offset += 8;

                proto_tree_add_item(btle_tree, hf_control_master_session_initialization_vector, tvb, offset, 4, ENC_LITTLE_ENDIAN);
                offset += 4;

                break;
            case 0x04: /
                proto_tree_add_item(btle_tree, hf_control_slave_session_key_diversifier, tvb, offset, 8, ENC_LITTLE_ENDIAN);
                offset += 8;

                proto_tree_add_item(btle_tree, hf_control_slave_session_initialization_vector, tvb, offset, 4, ENC_LITTLE_ENDIAN);
                offset += 4;

                break;
            case 0x07: /
                proto_tree_add_item(btle_tree, hf_control_unknown_type, tvb, offset, 1, ENC_LITTLE_ENDIAN);
                offset += 1;

                break;
            case 0x08: /
            case 0x09: /
            case 0x0E: /
                sub_item = proto_tree_add_item(btle_tree, hf_control_feature_set, tvb, offset, 8, ENC_LITTLE_ENDIAN);
                sub_tree = proto_item_add_subtree(sub_item, ett_features);

                proto_tree_add_item(sub_tree, hf_control_feature_set_le_encryption, tvb, offset, 1, ENC_NA);
                proto_tree_add_item(sub_tree, hf_control_feature_set_connection_parameters_request_procedure, tvb, offset, 1, ENC_NA);
                proto_tree_add_item(sub_tree, hf_control_feature_set_extended_reject_indication, tvb, offset, 1, ENC_NA);
                proto_tree_add_item(sub_tree, hf_control_feature_set_slave_initiated_features_exchange, tvb, offset, 1, ENC_NA);
                proto_tree_add_item(sub_tree, hf_control_feature_set_le_ping, tvb, offset, 1, ENC_NA);
                proto_tree_add_item(sub_tree, hf_control_feature_set_reserved_5_7, tvb, offset, 1, ENC_NA);
                offset += 1;

                proto_tree_add_item(sub_tree, hf_control_feature_set_reserved, tvb, offset, 7, ENC_NA);
                offset += 7;

                break;
            case 0x0C: /
                proto_tree_add_item(btle_tree, hf_control_version_number, tvb, offset, 1, ENC_LITTLE_ENDIAN);
                offset += 1;

                proto_tree_add_item(btle_tree, hf_control_company_id, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_subversion_number, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                break;
            case 0x0F: /
            case 0x10: /
                proto_tree_add_item(btle_tree, hf_control_interval_min, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_interval_max, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_latency, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_timeout, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_preffered_periodicity, tvb, offset, 1, ENC_LITTLE_ENDIAN);
                offset += 1;

                proto_tree_add_item(btle_tree, hf_control_reference_connection_event_count, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_offset_0, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_offset_1, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_offset_2, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_offset_3, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_offset_4, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                proto_tree_add_item(btle_tree, hf_control_offset_5, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;

                break;
            case 0x11: /
                proto_tree_add_item(btle_tree, hf_control_reject_opcode, tvb, offset, 1, ENC_LITTLE_ENDIAN);
                offset += 1;

                proto_tree_add_item(btle_tree, hf_control_error_code, tvb, offset, 1, ENC_LITTLE_ENDIAN);
                offset += 1;

                break;
            default:
                if (tvb_reported_length_remaining(tvb, offset) > 3) {
                    proto_tree_add_expert(btle_tree, pinfo, &ei_unknown_data, tvb, offset, tvb_reported_length_remaining(tvb, offset) - 3);
                    offset += tvb_reported_length_remaining(tvb, offset) - 3;
                }
            }

            break;
        default:
            if (tvb_reported_length_remaining(tvb, offset) > 3) {
                proto_tree_add_expert(btle_tree, pinfo, &ei_unknown_data, tvb, offset, tvb_reported_length_remaining(tvb, offset) - 3);
                offset += tvb_reported_length_remaining(tvb, offset) - 3;
            }
        }

        if ((crc_status == CRC_INDETERMINATE) &&
            btle_context && btle_context->connection_info_valid) {
            /
            crc_init = btle_context->connection_info.CRCInit;
            crc_status = CRC_CAN_BE_CALCULATED;
        }
    }

    /
    packet_crc = reverse_bits_per_byte(tvb_get_ntoh24(tvb, offset));
    sub_item = proto_tree_add_uint(btle_tree, hf_crc, tvb, offset, 3, packet_crc);
    offset += 3;
    if (crc_status == CRC_CAN_BE_CALCULATED) {
        guint32 crc = btle_crc(tvb, length, crc_init);
        crc_status = (packet_crc == crc) ? CRC_CORRECT : CRC_INCORRECT;
    }
    switch(crc_status) {
    case CRC_INDETERMINATE:
        expert_add_info(pinfo, sub_item, &ei_crc_cannot_be_determined);
        break;
    case CRC_INCORRECT:
        expert_add_info(pinfo, sub_item, &ei_crc_incorrect);
        break;
    case CRC_CORRECT:
        expert_add_info(pinfo, sub_item, &ei_crc_correct);
        break;
    default:
        break;
    }

    return offset;
}
