static gint
dissect_darwin_buffer_packet_header(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
        usb_conv_info_t *usb_conv_info, guint64 *id)
{
    guint8   transfer_type;
    guint8   request_type;
    guint8   endpoint_byte;
    guint8   transfer_type_and_direction;
    guint8   header_length;

    proto_tree_add_item(tree, hf_usb_darwin_bcd_version, tvb, 0, 2, ENC_LITTLE_ENDIAN);

    header_length = tvb_get_guint8(tvb, 2);
    proto_tree_add_item(tree, hf_usb_darwin_header_len, tvb, 2, 1, ENC_LITTLE_ENDIAN);

    request_type = tvb_get_guint8(tvb, 3);
    usb_conv_info->is_request = (request_type == DARWIN_IO_SUBMIT);
    proto_tree_add_uint(tree, hf_usb_darwin_request_type, tvb, 3, 1, request_type);

    proto_tree_add_item(tree, hf_usb_darwin_io_len, tvb, 4, 4, ENC_LITTLE_ENDIAN);

    proto_tree_add_item(tree, hf_usb_darwin_io_status, tvb, 8, 4, ENC_LITTLE_ENDIAN);

    proto_tree_add_item(tree, hf_usb_darwin_iso_num_packets, tvb, 12, 4, ENC_LITTLE_ENDIAN);

    *id = tvb_get_guint64(tvb, 16, ENC_LITTLE_ENDIAN);
    proto_tree_add_uint64(tree, hf_usb_darwin_io_id, tvb, 16, 8, *id);

    proto_tree_add_item(tree, hf_usb_darwin_device_location, tvb, 24, 4, ENC_LITTLE_ENDIAN);
    usb_conv_info->bus_id = tvb_get_letohl(tvb, 24) >> 24;

    proto_tree_add_item(tree, hf_usb_darwin_speed, tvb, 28, 1, ENC_LITTLE_ENDIAN);

    usb_conv_info->device_address = (guint16)tvb_get_guint8(tvb, 29);
    proto_tree_add_uint(tree, hf_usb_darwin_device_address, tvb, 29, 1, usb_conv_info->device_address);

    endpoint_byte = tvb_get_guint8(tvb, 30);
    usb_conv_info->endpoint = endpoint_byte & 0x7F;
    if (endpoint_byte & URB_TRANSFER_IN) {
        usb_conv_info->direction = P2P_DIR_RECV;
    }
    else {
        usb_conv_info->direction = P2P_DIR_SENT;
    }
    proto_tree_add_uint(tree, hf_usb_darwin_endpoint_address, tvb, 30, 1, endpoint_byte);
    proto_tree_add_bitmask(tree, tvb, 30, hf_usb_endpoint_number, ett_usb_endpoint, usb_endpoint_fields, ENC_LITTLE_ENDIAN);

    transfer_type = tvb_get_guint8(tvb, 31);
    usb_conv_info->transfer_type = darwin_endpoint_to_linux[transfer_type];
    proto_tree_add_uint(tree, hf_usb_darwin_endpoint_type, tvb, 31, 1, transfer_type);

    transfer_type_and_direction = darwin_endpoint_to_linux[transfer_type] | (endpoint_byte & 0x80);
    col_append_str(pinfo->cinfo, COL_INFO,
                   val_to_str(transfer_type_and_direction, usb_transfer_type_and_direction_vals, "Unknown type %x"));
    col_append_str(pinfo->cinfo, COL_INFO, usb_conv_info->is_request == TRUE ? " (submitted)" : " (completed)");

    usb_conv_info->is_setup = FALSE;
    if ((usb_conv_info->is_request == TRUE) && (usb_conv_info->transfer_type == URB_CONTROL)) {
        usb_conv_info->is_setup = TRUE;
    }

    usb_conv_info->setup_requesttype = 0;

    return header_length;
}
