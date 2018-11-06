static void
dissect_amqp_0_10_stream(tvbuff_t *tvb,
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
    method_name = val_to_str_const(method, amqp_0_10_stream_methods,
                                   "<invalid stream method>");
    col_append_str(pinfo->cinfo, COL_INFO, method_name);
    col_append_str(pinfo->cinfo, COL_INFO, " ");
    col_set_fence(pinfo->cinfo, COL_INFO);

    proto_tree_add_item(tree, hf_amqp_0_10_stream_method,
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
    case AMQP_0_10_METHOD_STREAM_QOS:
        if ((flag1 & ~0x0f) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_qos_prefetch_size,
                                tvb, offset, 4, ENC_BIG_ENDIAN);
            AMQP_INCREMENT(offset, 4, length);
        }
        if (flag1 & 0x02) {     /
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_qos_prefetch_count,
                                tvb, offset, 2, ENC_BIG_ENDIAN);
            AMQP_INCREMENT(offset, 2, length);
        }
        if (flag1 & 0x04) {     /
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_qos_prefetch_size,
                                tvb, offset, 4, ENC_BIG_ENDIAN);
            AMQP_INCREMENT(offset, 4, length);
        }
        /
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_stream_qos_global,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        break;

    case AMQP_0_10_METHOD_STREAM_QOS_OK:
        /
        break;

    case AMQP_0_10_METHOD_STREAM_CONSUME:
        if ((flag1 & ~0x3f) || (flag2 != 0))
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
                                hf_amqp_0_10_method_stream_consumer_tag,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        /
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_stream_consume_no_local,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_stream_consume_exclusive,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_stream_consume_nowait,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        if (flag1 & 0x20) {     /
            map_length = tvb_get_ntohl(tvb, offset);
            AMQP_INCREMENT(offset, 4, length);
            THROW_ON(((offset + map_length) > length), ReportedBoundsError);
            ti = proto_tree_add_item(args_tree,
                                     hf_amqp_0_10_method_stream_consume_arguments,
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

    case AMQP_0_10_METHOD_STREAM_CONSUME_OK:
    case AMQP_0_10_METHOD_STREAM_CANCEL:
        if ((flag1 & ~0x01) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_consumer_tag,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        break;

    case AMQP_0_10_METHOD_STREAM_PUBLISH:
        if ((flag1 & ~0x0f) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) <= length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_publish_exchange,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        if (flag1 & 0x02) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_publish_routing_key,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        /
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_stream_publish_mandatory,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_stream_publish_immediate,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        break;

    case AMQP_0_10_METHOD_STREAM_RETURN:
        if ((flag1 & ~0x0f) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_return_reply_code,
                                tvb, offset, 2, ENC_BIG_ENDIAN);
            AMQP_INCREMENT(offset, 2, length);
        }
        if (flag1 & 0x02) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_return_reply_text,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        if (flag1 & 0x04) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_return_exchange,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        if (flag1 & 0x08) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_return_routing_key,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        break;

    case AMQP_0_10_METHOD_STREAM_DELIVER:
        if ((flag1 & ~0x0f) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_deliver_consumer_tag,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        if (flag1 & 0x02) {     /
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_deliver_delivery_tag,
                                tvb, offset, 8, ENC_BIG_ENDIAN);
            AMQP_INCREMENT(offset, 8, length);
        }
        if (flag1 & 0x04) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_deliver_exchange,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        if (flag1 & 0x08) {     /
            str_size = tvb_get_guint8(tvb, offset);
            THROW_ON(((offset + 1 + str_size) > length), ReportedBoundsError);
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_stream_deliver_queue,
                                tvb, offset + 1, str_size, ENC_ASCII|ENC_NA);
            AMQP_INCREMENT(offset, (1 + str_size), length);
        }
        break;
    }
}
