static void
dissect_amqp_0_10_struct_message_properties(tvbuff_t *tvb,
                                            packet_info *pinfo,
                                            proto_tree *tree,
                                            int offset,
                                            guint32 struct_length)
{
    proto_item *ti;
    proto_item *frag;
    proto_item *args_tree;
    proto_item *flags_item, *subflags_item;
    guint8      flag1, flag2;
    guint8      subflag1, subflag2;
    guint8      len8;
    guint16     len16;
    guint32     map_length;
    e_guid_t    uuid;
    int         max_length;

    max_length = offset + struct_length;
    frag = proto_item_add_subtree(tree, ett_args);
    AMQP_INCREMENT(offset, 2, max_length);  /
    flag1 = tvb_get_guint8(tvb, offset);
    flag2 = tvb_get_guint8(tvb, offset+1);
    flags_item = proto_tree_add_item(frag,
                                     hf_amqp_0_10_argument_packing_flags,
                                     tvb, offset, 2, ENC_BIG_ENDIAN);
    if (flag2 & ~0x01)
        expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
    AMQP_INCREMENT(offset, 2, max_length);
    if (flag1 & 0x01) {
        /
        proto_tree_add_item(frag,
                            hf_amqp_0_10_struct_message_properties_content_len,
                            tvb, offset, 8, ENC_BIG_ENDIAN);
        AMQP_INCREMENT(offset, 8, max_length);
    }
    if (flag1 & 0x02) {
        /
        tvb_get_guid(tvb, offset, &uuid, ENC_BIG_ENDIAN);
        proto_tree_add_guid(frag,
                            hf_amqp_0_10_struct_message_properties_message_id,
                            tvb, offset, 16, &uuid);
        AMQP_INCREMENT(offset, 16, max_length);
    }
    if (flag1 & 0x04) {
        /
        len16 = tvb_get_ntohs(tvb, offset);
        proto_tree_add_item(frag,
                            hf_amqp_0_10_struct_message_properties_correlation,
                            tvb, offset + 2, len16, ENC_NA);
        AMQP_INCREMENT(offset, (2 + len16), max_length);
    }
    if (flag1 & 0x08) {
        /
        /
        len16 = tvb_get_ntohs(tvb, offset);
        AMQP_INCREMENT(offset, 2, max_length);
        ti = proto_tree_add_item(frag,
                                 hf_amqp_0_10_struct_message_properties_reply_to,
                                 tvb, offset, len16, ENC_NA);
        args_tree = proto_item_add_subtree(ti, ett_args);
        subflags_item = proto_tree_add_item(args_tree,
                                            hf_amqp_0_10_argument_packing_flags,
                                            tvb, offset, 2, ENC_BIG_ENDIAN);
        subflag1 = tvb_get_guint8(tvb, offset);
        subflag2 = tvb_get_guint8(tvb, offset + 1);
        if ((subflag1 & ~0x03) || (subflag2 != 0))
            expert_add_info(pinfo, subflags_item, &ei_amqp_bad_flag_value);
        AMQP_INCREMENT(offset, 2, max_length);
        if (subflag1 & 0x01) {
            /
            len8 = tvb_get_guint8(tvb, offset);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_struct_reply_to_exchange,
                                tvb, offset + 1, len8, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + len8), max_length);
        }
        if (subflag1 & 0x02) {
            /
            len8 = tvb_get_guint8(tvb, offset);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_struct_reply_to_routing_key,
                                tvb, offset + 1, len8, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + len8), max_length);
        }
    }
    if (flag1 & 0x10) {
        /
        len8 = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(frag,
                            hf_amqp_0_10_struct_message_properties_content_type,
                            tvb, offset + 1, len8, ENC_ASCII|ENC_NA);
        AMQP_INCREMENT(offset, (1 + len8), max_length);
    }
    if (flag1 & 0x20) {
        /
        len8 = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(frag,
                            hf_amqp_0_10_struct_message_properties_content_encoding,
                            tvb, offset + 1, len8, ENC_ASCII|ENC_NA);
        AMQP_INCREMENT(offset, (1 + len8), max_length);
    }
    if (flag1 & 0x40) {
        /
        len16 = tvb_get_ntohs(tvb, offset);
        proto_tree_add_item(frag,
                            hf_amqp_0_10_struct_message_properties_user_id,
                            tvb, offset + 2, len16, ENC_NA);
        AMQP_INCREMENT(offset, (2 + len16), max_length);
    }
    if (flag1 & 0x80) {
        /
        len16 = tvb_get_ntohs(tvb, offset);
        proto_tree_add_item(frag,
                            hf_amqp_0_10_struct_message_properties_app_id,
                            tvb, offset + 2, len16, ENC_NA);
        AMQP_INCREMENT(offset, (2 + len16), max_length);
    }
    if (flag2 & 0x01) {
        /
        map_length = tvb_get_ntohl(tvb, offset);
        AMQP_INCREMENT(offset, 4, max_length);
        ti = proto_tree_add_item(frag,
                                 hf_amqp_0_10_struct_message_properties_application_headers,
                                 tvb,
                                 offset,
                                 map_length, ENC_NA);
        dissect_amqp_0_10_map (tvb,
                               offset,
                               offset + map_length,
                               map_length,
                               ti);
        AMQP_INCREMENT(offset, map_length, max_length);
    }
}
