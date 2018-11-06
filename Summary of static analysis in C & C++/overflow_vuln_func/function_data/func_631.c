static int
dissect_control(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, int offset)
{
    proto_item  *pitem = NULL;
    guint        control_type;
    guint8       unknown_control_type;
    guint8       uuid_size;
    guint16      uuid_dst;
    guint16      uuid_src;
    guint16      response_message;
    guint        list_length;
    guint        i_item;

    proto_tree_add_item(tree, hf_btbnep_control_type, tvb, offset, 1, ENC_BIG_ENDIAN);
    control_type = tvb_get_guint8(tvb, offset);
    offset += 1;

    col_append_fstr(pinfo->cinfo, COL_INFO, " - %s", val_to_str_const(control_type, control_type_vals,  "Unknown type"));

    switch(control_type) {
        case 0x00: /
            proto_tree_add_item(tree, hf_btbnep_unknown_control_type, tvb, offset, 1, ENC_BIG_ENDIAN);
            unknown_control_type = tvb_get_guint8(tvb, offset);
            offset += 1;

            col_append_fstr(pinfo->cinfo, COL_INFO, " - Unknown(%s)", val_to_str_const(unknown_control_type, control_type_vals,  "Unknown type"));

            break;
        case 0x01: /
            proto_tree_add_item(tree, hf_btbnep_uuid_size, tvb, offset, 1, ENC_BIG_ENDIAN);
            uuid_size = tvb_get_guint8(tvb, offset);
            offset += 1;

            pitem = proto_tree_add_item(tree, hf_btbnep_destination_service_uuid, tvb, offset, uuid_size, ENC_NA);
            uuid_dst = tvb_get_ntohs(tvb, offset);
            proto_item_append_text(pitem, " (%s)", val_to_str_ext(uuid_dst, &bt_sig_uuid_vals_ext,  "Unknown uuid"));
            offset += uuid_size;

            pitem = proto_tree_add_item(tree, hf_btbnep_source_service_uuid, tvb, offset, uuid_size, ENC_NA);
            uuid_src = tvb_get_ntohs(tvb, offset);
            proto_item_append_text(pitem, " (%s)", val_to_str_ext(uuid_src, &bt_sig_uuid_vals_ext,  "Unknown uuid"));
            offset += uuid_size;

            col_append_fstr(pinfo->cinfo, COL_INFO, " - dst: <%s>, src: <%s>",
                    val_to_str_ext(uuid_dst, &bt_sig_uuid_vals_ext,  "Unknown uuid"),
                    val_to_str_ext(uuid_src, &bt_sig_uuid_vals_ext,  "Unknown uuid"));
            break;
        case 0x02: /
            proto_tree_add_item(tree, hf_btbnep_setup_connection_response_message, tvb, offset, 2, ENC_BIG_ENDIAN);
            response_message = tvb_get_ntohs(tvb, offset);
            offset += 2;
            col_append_fstr(pinfo->cinfo, COL_INFO, " - %s",
                    val_to_str_const(response_message, setup_connection_response_message_vals,  "Unknown response message"));
            break;
        case 0x03: /
            proto_tree_add_item(tree, hf_btbnep_list_length, tvb, offset, 2, ENC_BIG_ENDIAN);
            list_length = tvb_get_ntohs(tvb, offset);
            offset += 2;

            for (i_item = 0; i_item < list_length; i_item += 4) {
                proto_tree_add_item(tree, hf_btbnep_network_type_start, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;

                proto_tree_add_item(tree, hf_btbnep_network_type_end, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
            }
            break;
        case 0x04: /
            proto_tree_add_item(tree, hf_btbnep_filter_net_type_response_message, tvb, offset, 2, ENC_BIG_ENDIAN);
            response_message = tvb_get_ntohs(tvb, offset);
            offset += 2;
            col_append_fstr(pinfo->cinfo, COL_INFO, " - %s",
                    val_to_str_const(response_message, filter_net_type_response_message_vals,  "Unknown response message"));
            break;
        case 0x05: /
            proto_tree_add_item(tree, hf_btbnep_list_length, tvb, offset, 2, ENC_BIG_ENDIAN);
            list_length = tvb_get_ntohs(tvb, offset);
            offset += 2;

            for (i_item = 0; i_item < list_length; i_item += 12) {
                proto_tree_add_item(tree, hf_btbnep_multicast_address_start, tvb, offset, 6, ENC_NA);
                offset += 6;

                proto_tree_add_item(tree, hf_btbnep_multicast_address_end, tvb, offset, 6, ENC_NA);
                offset += 6;
            }
            break;
        case 0x06: /
            proto_tree_add_item(tree, hf_btbnep_filter_multi_addr_response_message, tvb, offset, 2, ENC_BIG_ENDIAN);
            response_message = tvb_get_ntohs(tvb, offset);
            offset += 2;
            col_append_fstr(pinfo->cinfo, COL_INFO, " - %s",
                    val_to_str_const(response_message, filter_multi_addr_response_message_vals,  "Unknown response message"));
            break;

    };

    return offset;
}
