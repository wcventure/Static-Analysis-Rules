static void
dissect_amqp_0_10_queue(tvbuff_t *tvb,
                        packet_info *pinfo,
                        proto_tree *tree,
                        int offset, guint16 length)
{
    proto_item  *args_tree;
    proto_item  *ti;
    proto_item  *flags_item;
    guint8       method;
    guint8       flag1, flag2;
    guint8       str_size;
    guint32      map_length;
    int          flags_offset;
    const gchar *method_name;

    method = tvb_get_guint8(tvb, offset+1);
    method_name = val_to_str_const(method, amqp_0_10_queue_methods,
                                   "<invalid queue method>");
    col_append_str(pinfo->cinfo, COL_INFO, method_name);
    col_append_str(pinfo->cinfo, COL_INFO, " ");
    col_set_fence(pinfo->cinfo, COL_INFO);

    proto_tree_add_item(tree, hf_amqp_0_10_queue_method,
                        tvb, offset+1, 1, ENC_BIG_ENDIAN);
    AMQP_INCREMENT(offset, 2, length);
    /
    flag1 = tvb_get_guint8(tvb, offset);
    flag2 = tvb_get_guint8(tvb, offset+1);
    ti = proto_tree_add_item(tree, hf_amqp_0_10_session_header, tvb, offset, 2, ENC_BIG_ENDIAN);
    if ((flag1 != 1) || ((flag2 & 0xfe) != 0))
        proto_item_append_text(ti, " (Invalid)");
    else
        proto_tree_add_item(tree, hf_amqp_0_10_session_header_sync,
                            tvb, offset + 1, 1, ENC_BIG_ENDIAN);
    AMQP_INCREMENT(offset, 2, length);

    ti = proto_tree_add_item(tree, hf_amqp_method_arguments,
                             tvb, offset, length - 4, ENC_NA);
    args_tree = proto_item_add_subtree(ti, ett_args);

    /
    flags_offset = offset;
    flag1 = tvb_get_guint8(tvb, offset);
    flag2 = tvb_get_guint8(tvb, offset+1);
    flags_item = proto_tree_add_item(args_tree,
                                     hf_amqp_0_10_argument_packing_flags,
                                     tvb, offset, 2, ENC_BIG_ENDIAN);
    AMQP_INCREMENT(offset, 2, length);
    switch (method) {
    case AMQP_0_10_METHOD_QUEUE_DECLARE:
        if ((flag1 & ~0x7f) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_queue_name,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        if (flag1 & 0x02) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_queue_alt_exchange,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        /
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_queue_declare_passive,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_queue_declare_durable,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_queue_declare_exclusive,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_queue_declare_auto_delete,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        if (flag1 & 0x40) {     /
            map_length = tvb_get_ntohl(tvb, offset);
            AMQP_INCREMENT(offset, 4, length);
            THROW_ON(((offset + map_length) > length), ReportedBoundsError);
            ti = proto_tree_add_item(args_tree,
                                     hf_amqp_0_10_method_queue_declare_arguments,
                                     tvb,
                                     offset - 4,
                                     map_length + 4, ENC_NA);
            dissect_amqp_0_10_map (tvb,
                                   offset,
                                   offset + map_length,
                                   map_length,
                                   ti);
            AMQP_INCREMENT(offset, map_length, length);
        }
        break;

    case AMQP_0_10_METHOD_QUEUE_DELETE:
        if ((flag1 & ~0x07) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_queue_name,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        /
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_queue_delete_if_unused,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_queue_delete_if_empty,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        break;

    case AMQP_0_10_METHOD_QUEUE_PURGE:
        if ((flag1 & ~0x01) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_queue_name,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        break;

    case AMQP_0_10_METHOD_QUEUE_QUERY:
        if ((flag1 & ~0x01) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_queue_name,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        break;
    }
}
