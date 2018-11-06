static int dissect_jxta_stream(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
    guint offset = 0;
    guint available = tvb_reported_length_remaining(tvb, offset);
    gint processed;
    gint needed = 0;
    conversation_t *tpt_conversation = NULL;
    jxta_stream_conversation_data *tpt_conv_data = NULL;
    proto_item *jxta_tree_item = NULL;
    proto_tree *jxta_tree = NULL;
    
    /

    if (available < sizeof(JXTA_WELCOME_MSG_SIG)) {
        needed = (gint) (sizeof(JXTA_WELCOME_MSG_SIG) - available);
        goto Common_Exit;
    }

    if (0 == tvb_memeql(tvb, 0, JXTA_WELCOME_MSG_SIG, sizeof(JXTA_WELCOME_MSG_SIG))) {
        /
        address *welcome_addr;
        gboolean initiator = FALSE;

        tpt_conversation = get_tpt_conversation(pinfo, TRUE);
        tpt_conv_data = (jxta_stream_conversation_data *) conversation_get_proto_data(tpt_conversation, proto_jxta);

        if (0 == tpt_conv_data->initiator_welcome_frame) {
            /
            tpt_conv_data->tpt_ptype = pinfo->ptype;
            tpt_conv_data->initiator_welcome_frame = pinfo->fd->num;
            COPY_ADDRESS(&tpt_conv_data->initiator_tpt_address, &pinfo->src);
            tpt_conv_data->initiator_tpt_port = pinfo->srcport;

            welcome_addr = &tpt_conv_data->initiator_address;
            initiator = TRUE;
        } else {
            if (tpt_conv_data->initiator_welcome_frame >= pinfo->fd->num) {
                /
                tpt_conv_data->receiver_welcome_frame = tpt_conv_data->initiator_welcome_frame;
                tpt_conv_data->receiver_tpt_address = tpt_conv_data->initiator_tpt_address;
                tpt_conv_data->receiver_tpt_port = tpt_conv_data->initiator_tpt_port;
                tpt_conv_data->receiver_address = tpt_conv_data->initiator_address;
                tpt_conv_data->initiator_welcome_frame = pinfo->fd->num;
                COPY_ADDRESS(&tpt_conv_data->initiator_tpt_address, &pinfo->src);
                tpt_conv_data->initiator_tpt_port = pinfo->srcport;

                welcome_addr = &tpt_conv_data->initiator_address;
                initiator = TRUE;
            } else {
                /
                tpt_conv_data->tpt_ptype = pinfo->ptype;
                tpt_conv_data->receiver_welcome_frame = pinfo->fd->num;
                COPY_ADDRESS(&tpt_conv_data->receiver_tpt_address, &pinfo->src);
                tpt_conv_data->receiver_tpt_port = pinfo->srcport;

                welcome_addr = &tpt_conv_data->receiver_address;
                initiator = FALSE;
            }
        }

        processed = dissect_jxta_welcome(tvb, pinfo, NULL, welcome_addr, initiator);
        
        if( processed < 0 ) {
            needed = -processed;
            goto Common_Exit;
        }
        
        /
        jxta_tree_item =
            proto_tree_add_protocol_format(tree, proto_jxta, tvb, offset, -1, "JXTA" );
        jxta_tree = proto_item_add_subtree(jxta_tree_item, ett_jxta);

        processed = dissect_jxta_welcome(tvb, pinfo, jxta_tree, welcome_addr, initiator);
    } else {
        /
        guint64 content_length = -1;
        gchar *content_type = NULL;
        gint headers_len = dissect_jxta_message_framing(tvb, pinfo, NULL, &content_length, &content_type);

        /

        if ((0 == headers_len) || (NULL == content_type) || (content_length <= 0) || (content_length > UINT_MAX)) {
            /
            return 0;
        }

        if (headers_len < 0) {
            /
            needed = -headers_len;
            goto Common_Exit;
        }
        
        jxta_tree_item =
            proto_tree_add_protocol_format(tree, proto_jxta, tvb, offset, -1, "JXTA" );
        jxta_tree = proto_item_add_subtree(jxta_tree_item, ett_jxta);

        /
        headers_len = dissect_jxta_message_framing(tvb, pinfo, jxta_tree, &content_length, &content_type);

        available = tvb_reported_length_remaining(tvb, offset + headers_len);
        if (available >= content_length) {
            tvbuff_t *jxta_message_tvb = tvb_new_subset(tvb, offset + headers_len, (gint) content_length, (gint) content_length);
            conversation_t *peer_conversation = NULL;
            address saved_src_addr;
            guint32 saved_src_port = 0;
            address saved_dst_addr;
            guint32 saved_dst_port = 0;
            port_type saved_port_type = PT_NONE;
            gboolean dissected;

            tpt_conversation = get_tpt_conversation(pinfo, TRUE);

            if (NULL != tpt_conversation) {
                tpt_conv_data = (jxta_stream_conversation_data *) conversation_get_proto_data(tpt_conversation, proto_jxta);

                if ((AT_NONE != tpt_conv_data->initiator_address.type) && (AT_NONE != tpt_conv_data->receiver_address.type)) {
                    peer_conversation =
                        find_conversation(pinfo->fd->num, &tpt_conv_data->initiator_address, &tpt_conv_data->receiver_address,
                                          PT_NONE, 0, 0, NO_PORT_B);

                    if (NULL == peer_conversation) {
                        peer_conversation =
                            conversation_new(pinfo->fd->num, &tpt_conv_data->initiator_address,
                                             &tpt_conv_data->receiver_address, PT_NONE, 0, 0, NO_PORT_B);
                        conversation_set_dissector(peer_conversation, stream_jxta_handle);
                    }

                } else {
                    g_warning("Uninitialized peer conversation");
                }
            }

            /
            if (NULL != peer_conversation) {
                saved_src_addr = pinfo->src;
                saved_src_port = pinfo->srcport;
                saved_dst_addr = pinfo->dst;
                saved_dst_port = pinfo->destport;
                saved_port_type = pinfo->ptype;
                /
                if (ADDRESSES_EQUAL(&pinfo->src, &tpt_conv_data->initiator_tpt_address)
                    && tpt_conv_data->initiator_tpt_port == pinfo->srcport) {
                    /
                    pinfo->src = tpt_conv_data->initiator_address;
                    pinfo->dst = tpt_conv_data->receiver_address;
                } else if (ADDRESSES_EQUAL(&pinfo->src, &tpt_conv_data->receiver_tpt_address) &&
                           tpt_conv_data->receiver_tpt_port == pinfo->srcport) {
                    /
                    pinfo->src = tpt_conv_data->receiver_address;
                    pinfo->dst = tpt_conv_data->initiator_address;
                } else {
                    /
                }
                /
                pinfo->ptype = PT_NONE;
                pinfo->srcport = 0;
                pinfo->destport = 0;
            }

            dissected =
                dissector_try_string(media_type_dissector_table, content_type, jxta_message_tvb, pinfo, tree);

            if (!dissected) {
                call_dissector(data_handle, jxta_message_tvb, pinfo, jxta_tree);
            }

            /
            if (NULL != peer_conversation) {
                pinfo->src = saved_src_addr;
                pinfo->srcport = saved_src_port;
                pinfo->dst = saved_dst_addr;
                pinfo->destport = saved_dst_port;
                pinfo->ptype = saved_port_type;
            }

            processed = (guint) content_length + headers_len;
        } else {
            /
            needed = (gint) ((guint) content_length - available);
            goto Common_Exit;
        }
    }

    offset += processed;

Common_Exit:
    if ((needed > 0) && gDESEGMENT && pinfo->can_desegment) {
        /
        pinfo->desegment_offset = offset;
        pinfo->desegment_len = needed;
        return -needed;
    }

    return offset;
}
