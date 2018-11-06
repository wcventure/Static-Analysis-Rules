static void
dissect_zcl_msg_confirm(tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
    guint   msg_len;
    guint8 *msg_data;
    nstime_t confirm_time;

    /
    proto_tree_add_item(tree, hf_zbee_zcl_msg_message_id, tvb, *offset, 4, ENC_LITTLE_ENDIAN);
    *offset += 4;

    /
    confirm_time.secs = tvb_get_letohl(tvb, *offset) + ZBEE_ZCL_NSTIME_UTC_OFFSET;
    confirm_time.nsecs = 0;
    proto_tree_add_time(tree, hf_zbee_zcl_msg_confirm_time, tvb, *offset, 4, &confirm_time);
    *offset += 4;

    /
    if ( tvb_reported_length_remaining(tvb, *offset) <= 0 ) return;
    proto_tree_add_item(tree, hf_zbee_zcl_msg_confirm_ctrl, tvb, *offset, 1, ENC_NA);
    *offset += 1;

    /
    if ( tvb_reported_length_remaining(tvb, *offset) <= 0 ) return;
    msg_len = tvb_get_guint8(tvb, *offset); /
    proto_tree_add_item(tree, hf_zbee_zcl_msg_confirm_response_length, tvb, *offset, 1, ENC_NA);
    *offset += 1;

    /
    if (msg_len > 0) {
        msg_data = tvb_get_string_enc(wmem_packet_scope(), tvb, *offset, msg_len, ENC_LITTLE_ENDIAN);
        proto_tree_add_string(tree, hf_zbee_zcl_msg_confirm_response, tvb, *offset, msg_len, msg_data);
        *offset += msg_len;
    }

} /
