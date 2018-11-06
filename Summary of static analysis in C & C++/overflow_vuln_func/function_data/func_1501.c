static int dissect_jxta_tcp(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
    int offset = 0;
    int available = 0;
    int processed;
    conversation_t *tpt_conversation =
        find_conversation(pinfo->fd->num, &pinfo->src, &pinfo->dst, pinfo->ptype, pinfo->srcport, pinfo->destport, 0);
    jxta_stream_conversation_data * tpt_conv_data;

    if (tpt_conversation == NULL) {
        /
        tpt_conversation =
            conversation_new(pinfo->fd->num, &pinfo->src, &pinfo->dst, pinfo->ptype, pinfo->srcport, pinfo->destport, 0);
    }

    conversation_set_dissector(tpt_conversation, tcp_jxta_handle);
    
    tpt_conv_data = (jxta_stream_conversation_data*) conversation_get_proto_data(tpt_conversation, proto_jxta);
    
    if( NULL == tpt_conv_data ) {
        tpt_conv_data = (jxta_stream_conversation_data*) g_mem_chunk_alloc(my_conv_structs);
        tpt_conv_data->tpt_ptype = PT_NONE;
        tpt_conv_data->initiator_welcome_frame = 0;
        tpt_conv_data->initiator_address.type = AT_NONE;
        tpt_conv_data->receiver_welcome_frame = 0;
        tpt_conv_data->receiver_address.type = AT_NONE;
        conversation_add_proto_data(tpt_conversation, proto_jxta, tpt_conv_data);
    }
    
    if (check_col(pinfo->cinfo, COL_PROTOCOL)) {
        col_set_str(pinfo->cinfo, COL_PROTOCOL, "JXTA");
    }

    if (0 == tvb_memeql(tvb, 0, JXTA_WELCOME_MSG_SIG, sizeof(JXTA_WELCOME_MSG_SIG))) {
        address * welcome_addr;
        gboolean initiator = FALSE;

        if( 0 == tpt_conv_data->initiator_welcome_frame ) {
            tpt_conv_data->tpt_ptype= pinfo->ptype;
            tpt_conv_data->initiator_welcome_frame = pinfo->fd->num;
            COPY_ADDRESS( &tpt_conv_data->initiator_tpt_address, &pinfo->src );
            tpt_conv_data->initiator_tpt_port = pinfo->srcport;

            welcome_addr = &tpt_conv_data->initiator_address;
            initiator = TRUE;
        } else {
            if( tpt_conv_data->initiator_welcome_frame >= pinfo->fd->num ) {
                /
                tpt_conv_data->receiver_welcome_frame = tpt_conv_data->initiator_welcome_frame;
                tpt_conv_data->receiver_tpt_address = tpt_conv_data->initiator_tpt_address;
                tpt_conv_data->receiver_tpt_port = tpt_conv_data->initiator_tpt_port;
                tpt_conv_data->receiver_address = tpt_conv_data->initiator_address;
                tpt_conv_data->initiator_welcome_frame = pinfo->fd->num;
                COPY_ADDRESS( &tpt_conv_data->initiator_tpt_address, &pinfo->src );
                tpt_conv_data->initiator_tpt_port = pinfo->srcport;

                welcome_addr = &tpt_conv_data->initiator_address;
                initiator = TRUE;
            } else {
                tpt_conv_data->tpt_ptype= pinfo->ptype;
                tpt_conv_data->receiver_welcome_frame = pinfo->fd->num;
                COPY_ADDRESS( &tpt_conv_data->receiver_tpt_address, &pinfo->src );
                tpt_conv_data->receiver_tpt_port = pinfo->srcport;

                welcome_addr = &tpt_conv_data->receiver_address;
                initiator = FALSE;
            }
        }

        processed = dissect_jxta_welcome(tvb, pinfo, tree, welcome_addr, initiator);
    } else {
        guint64 content_length = 0;
        char *content_type = NULL;
        gboolean dissected;
        conversation_t *peer_conversation = NULL;
        address saved_src_addr;
        guint32 saved_src_port = 0;
        address saved_dst_addr;
        guint32 saved_dst_port = 0;
        port_type saved_port_type = PT_NONE;

        if( (AT_NONE != tpt_conv_data->initiator_address.type) && (AT_NONE != tpt_conv_data->receiver_address.type) ) {
            peer_conversation =
                find_conversation(pinfo->fd->num, &tpt_conv_data->initiator_address, &tpt_conv_data->receiver_address, PT_NONE, 0, 0, NO_PORT_B);

            if( NULL == peer_conversation ) {
                peer_conversation =
                    conversation_new(pinfo->fd->num, &tpt_conv_data->initiator_address, &tpt_conv_data->receiver_address, PT_NONE, 0, 0, NO_PORT_B);
            }

            conversation_set_dissector(peer_conversation, tcp_jxta_handle);
        }

        /
        if( NULL != peer_conversation ) {
             saved_src_addr = pinfo->src;
             saved_src_port = pinfo->srcport;
             saved_dst_addr = pinfo->dst;
             saved_dst_port = pinfo->destport;
             saved_port_type = pinfo->ptype;
/
             if( ADDRESSES_EQUAL( &pinfo->src, &tpt_conv_data->initiator_tpt_address ) && 
                tpt_conv_data->initiator_tpt_port == pinfo->srcport ) {
/
                 pinfo->src = tpt_conv_data->initiator_address;
                 pinfo->dst = tpt_conv_data->receiver_address;
             } else if( ADDRESSES_EQUAL( &pinfo->src, &tpt_conv_data->receiver_tpt_address ) && 
                 tpt_conv_data->receiver_tpt_port == pinfo->srcport ) {
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

        processed = dissect_jxta_message_framing(tvb, pinfo, tree, &content_length, &content_type);

        if ((processed > 0) && (NULL != content_type) && (0 != content_length)) {
            int msg_offset = offset + processed;

            available = tvb_reported_length_remaining(tvb, msg_offset);
            if (available >= (int)content_length) {
                tvbuff_t *jxta_message_tvb = tvb_new_subset(tvb, msg_offset, (gint)content_length, (gint)content_length);

                if (check_col(pinfo->cinfo, COL_INFO)) {
                    col_set_str(pinfo->cinfo, COL_INFO, "Message");

                    if ( AT_STRINGZ == pinfo->src.type ) {
                        col_append_fstr( pinfo->cinfo, COL_INFO, " %s", pinfo->src.data );
                    }

                    if ((AT_STRINGZ == pinfo->dst.type) || (AT_STRINGZ == pinfo->dst.type)) {
                        col_append_str( pinfo->cinfo, COL_INFO, " ->" );
                    }

                    if ( AT_STRINGZ == pinfo->dst.type) {
                        col_append_fstr( pinfo->cinfo, COL_INFO, " %s", pinfo->dst.data );
                    }

                    col_set_writable(pinfo->cinfo, FALSE);
                }

                dissected = dissector_try_string(media_type_dissector_table, content_type, jxta_message_tvb, pinfo, tree);

                if (!dissected) {
                    dissector_handle_t data_handle = find_dissector("data");
                    call_dissector(data_handle, jxta_message_tvb, pinfo, tree);
                }

                processed += (int)content_length;
            } else {
                processed = available - (int)content_length;
            }

            g_free(content_type);
        }

        /
        if( NULL != peer_conversation ) {
            pinfo->src = saved_src_addr;
            pinfo->srcport = saved_src_port;
            pinfo->dst = saved_dst_addr;
            pinfo->destport = saved_dst_port;
            pinfo->ptype = saved_port_type;
        }
    }

    if (processed <= 0) {
        pinfo->desegment_offset = offset;
        pinfo->desegment_len = -processed;
        return processed;
    }

    offset += processed;

    return offset;
}
