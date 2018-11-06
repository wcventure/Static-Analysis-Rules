static int
dissect_amqp_0_10_frame(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_item *ti;
    proto_item *amqp_tree = NULL;
    guint8      frame_type;
    guint16     length;
    guint32     struct_length;
    guint       offset;

#if 0  /
    conversation_t *conv;
    amqp_conv *conn;

    /
    conv = find_or_create_conversation(pinfo);
    conn = conversation_get_proto_data(conv, proto_amqp);
#endif

    /
    if (tvb_memeql(tvb, 0, "AMQP", 4) == 0) {
        guint8         proto_major;
        guint8         proto_minor;
        wmem_strbuf_t *strbuf;

        proto_major = tvb_get_guint8(tvb, 6);
        proto_minor = tvb_get_guint8(tvb, 7);
        strbuf      = wmem_strbuf_new_label(wmem_packet_scope());
        wmem_strbuf_append_printf(strbuf,
                                  "Protocol-Header %d-%d ",
                                  proto_major,
                                  proto_minor);
        col_append_str(pinfo->cinfo, COL_INFO, wmem_strbuf_get_str(strbuf));
        col_set_fence(pinfo->cinfo, COL_INFO);

        if (tree) {
            ti = proto_tree_add_item(tree, proto_amqp, tvb, 0, -1, ENC_NA);
            amqp_tree = proto_item_add_subtree(ti, ett_amqp_init);
            proto_tree_add_item(amqp_tree, hf_amqp_init_protocol,      tvb, 0, 4, ENC_ASCII|ENC_NA);
            proto_tree_add_item(amqp_tree, hf_amqp_init_id_major,      tvb, 4, 1, ENC_BIG_ENDIAN);
            proto_tree_add_item(amqp_tree, hf_amqp_init_id_minor,      tvb, 5, 1, ENC_BIG_ENDIAN);
            proto_tree_add_item(amqp_tree, hf_amqp_init_version_major, tvb, 6, 1, ENC_BIG_ENDIAN);
            proto_tree_add_item(amqp_tree, hf_amqp_init_version_minor, tvb, 7, 1, ENC_BIG_ENDIAN);
        }
        return 8;
    }

    /
    if (tree) {
        ti = proto_tree_add_item(tree, proto_amqp, tvb, 0, -1, ENC_NA);
        amqp_tree = proto_item_add_subtree(ti, ett_amqp);
        proto_tree_add_item(amqp_tree, hf_amqp_0_10_format,   tvb, 0, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(amqp_tree, hf_amqp_0_10_position, tvb, 0, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(amqp_tree, hf_amqp_0_10_type,     tvb, 1, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(amqp_tree, hf_amqp_0_10_size,     tvb, 2, 2, ENC_BIG_ENDIAN);
        proto_tree_add_item(amqp_tree, hf_amqp_0_10_track,    tvb, 5, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(amqp_tree, hf_amqp_channel,       tvb, 6, 2, ENC_BIG_ENDIAN);
    }

    frame_type = tvb_get_guint8(tvb, 1);
    length     = tvb_get_ntohs(tvb, 2);
    offset     = 12;
    THROW_ON((length <= 13), ReportedBoundsError);

    switch(frame_type) {
    case AMQP_0_10_FRAME_COMMAND:
        /
    case AMQP_0_10_FRAME_CONTROL:
        proto_tree_add_item(amqp_tree, hf_amqp_0_10_class, tvb, offset+0, 1, ENC_BIG_ENDIAN);
        switch(tvb_get_guint8(tvb, offset + 0)) {
        case AMQP_0_10_CLASS_CONNECTION:
            dissect_amqp_0_10_connection(tvb, pinfo, amqp_tree,
                                         offset, length);
            break;
        case AMQP_0_10_CLASS_SESSION:
            dissect_amqp_0_10_session(tvb, pinfo, amqp_tree,
                                      offset, length);
            break;
        case AMQP_0_10_CLASS_EXECUTION:
            dissect_amqp_0_10_execution(tvb, pinfo, amqp_tree,
                                        offset, length);
            break;
        case AMQP_0_10_CLASS_MESSAGE:
            dissect_amqp_0_10_message(tvb, pinfo, amqp_tree,
                                      offset, length);
            break;
        case AMQP_0_10_CLASS_TX:
            dissect_amqp_0_10_tx(tvb, pinfo, amqp_tree,
                                 offset, length);
            break;
        case AMQP_0_10_CLASS_DTX:
            dissect_amqp_0_10_dtx(tvb, pinfo, amqp_tree,
                                  offset, length);
            break;
        case AMQP_0_10_CLASS_EXCHANGE:
            dissect_amqp_0_10_exchange(tvb, pinfo, amqp_tree,
                                       offset, length);
            break;
        case AMQP_0_10_CLASS_QUEUE:
            dissect_amqp_0_10_queue(tvb, pinfo, amqp_tree,
                                    offset, length);
            break;
        case AMQP_0_10_CLASS_FILE:
            dissect_amqp_0_10_file(tvb, pinfo, amqp_tree,
                                   offset, length);
            break;
        case AMQP_0_10_CLASS_STREAM:
            dissect_amqp_0_10_stream(tvb, pinfo, amqp_tree,
                                     offset, length);
            break;
        default:
            expert_add_info_format(pinfo, amqp_tree, &ei_amqp_unknown_command_class, "Unknown command/control class %d", tvb_get_guint8(tvb, offset + 0));
        }
        break;

    case AMQP_0_10_FRAME_HEADER:
        col_append_str(pinfo->cinfo, COL_INFO, "header ");
        col_set_fence(pinfo->cinfo, COL_INFO);
        do {
            struct_length = tvb_get_ntohl(tvb, offset);
            AMQP_INCREMENT(offset, 4, length);
            THROW_ON((offset + struct_length > length), ReportedBoundsError);
            THROW_ON((offset + struct_length < offset), ReportedBoundsError);

            ti = proto_tree_add_item(amqp_tree,
                                     hf_amqp_0_10_undissected_struct32,
                                     tvb, offset, struct_length, ENC_NA);
            dissect_amqp_0_10_struct32(tvb, pinfo, ti, offset, struct_length);
            AMQP_INCREMENT(offset, struct_length, length);
        } while (offset < length);
        break;

    case AMQP_0_10_FRAME_BODY:
        col_append_str(pinfo->cinfo, COL_INFO, "message-body ");
        col_set_fence(pinfo->cinfo, COL_INFO);
        proto_tree_add_item(amqp_tree,
                            hf_amqp_0_10_message_body,
                            tvb, offset, length - 12, ENC_NA);
        break;

    default:
        expert_add_info_format(pinfo, amqp_tree, &ei_amqp_unknown_frame_type, "Unknown frame type %d", frame_type);
    }

    return tvb_length(tvb);
}
