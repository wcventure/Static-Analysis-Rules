static int dissect_jxta_udp(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
    int offset = 0;
    int available;
    int needed = 0;

    conversation_t *conversation =
        find_conversation(pinfo->fd->num, &pinfo->src, &pinfo->dst, pinfo->ptype, pinfo->srcport, pinfo->destport, 0);

    if (conversation == NULL) {
        /
        conversation =
            conversation_new(pinfo->fd->num, &pinfo->src, &pinfo->dst, pinfo->ptype, pinfo->srcport, pinfo->destport, 0);
    }

    DISSECTOR_ASSERT(find_dissector("jxta.udp"));

    conversation_set_dissector(conversation, find_dissector("jxta.udp"));

    while (TRUE) {
        tvbuff_t *jxta_message_framing_tvb;
        int processed = 0;
        guint64 content_length = -1;

        available = tvb_reported_length_remaining(tvb, offset);
        if (available < (int)sizeof(JXTA_UDP_SIG)) {
            needed = available - sizeof(JXTA_UDP_SIG);
            break;
        }

        if (tvb_memeql(tvb, offset, JXTA_UDP_SIG, sizeof(JXTA_UDP_SIG)) != 0) {
            /
            return 0;
        }

        offset += sizeof(JXTA_UDP_SIG);

        jxta_message_framing_tvb = tvb_new_subset(tvb, offset, -1, -1);
        processed = dissect_jxta_message_framing(jxta_message_framing_tvb, pinfo, NULL, &content_length, NULL);

        if (0 == processed) {
            /
            return 0;
        }

        if (processed < 0) {
            needed = -processed;
            break;
        }

        offset += processed;

        available = tvb_reported_length_remaining(tvb, offset);
        if (available < (int)content_length) {
            needed = available - (int)content_length;
            break;
        }

        offset += (int)content_length;

        break;
    }

    if ((needed > 0) && gDESEGMENT && pinfo->can_desegment) {
        pinfo->desegment_offset = 0;
        pinfo->desegment_len = needed;
        return -needed;
    }

    if (check_col(pinfo->cinfo, COL_PROTOCOL)) {
        col_set_str(pinfo->cinfo, COL_PROTOCOL, "JXTA");
    }

    if (check_col(pinfo->cinfo, COL_INFO)) {
        col_set_str(pinfo->cinfo, COL_INFO, "UDP Message");
    }

    col_set_writable(pinfo->cinfo, FALSE);

    if (tree) {
        int tree_offset = 0;
        proto_item *jxta_udp_tree_item = proto_tree_add_item(tree, hf_jxta_udp, tvb, tree_offset, -1, FALSE);
        proto_tree *jxta_udp_tree = proto_item_add_subtree(jxta_udp_tree_item, ett_jxta_udp);
        tvbuff_t *jxta_message_framing_tvb;
        guint64 content_length = -1;
        char *content_type = NULL;
        tvbuff_t *jxta_message_tvb;
        gboolean dissected = FALSE;
        int processed = 0;


        proto_tree_add_item(jxta_udp_tree, hf_jxta_udpsig, tvb, tree_offset, sizeof(JXTA_UDP_SIG), FALSE);
        tree_offset += sizeof(JXTA_UDP_SIG);

        jxta_message_framing_tvb = tvb_new_subset(tvb, tree_offset, -1, -1);
        processed = dissect_jxta_message_framing(jxta_message_framing_tvb, pinfo, jxta_udp_tree, &content_length, &content_type);
        tree_offset += processed;

        jxta_message_tvb = tvb_new_subset(tvb, tree_offset, (gint) content_length, (gint) content_length);
        dissected = dissector_try_string(media_type_dissector_table, content_type, jxta_message_tvb, pinfo, jxta_udp_tree);
        g_free(content_type);

        if (!dissected) {
            dissector_handle_t data_handle = find_dissector("data");
            call_dissector(data_handle, jxta_message_tvb, pinfo, jxta_udp_tree);
        }

        tree_offset += (int)content_length;

        proto_item_set_end(jxta_udp_tree_item, tvb, tree_offset);

        DISSECTOR_ASSERT(offset == tree_offset);
    }
    
    /     
    tap_queue_packet(jxta_tap, pinfo, NULL );

    return offset;
}
