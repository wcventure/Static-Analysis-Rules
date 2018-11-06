static int dissect_jxta_udp(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
    guint offset = 0;
    guint available;
    gint needed = 0;

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
        gint processed = 0;
        guint64 content_length = -1;
        gchar *content_type = NULL;

        available = tvb_reported_length_remaining(tvb, offset);
        if (available < sizeof(JXTA_UDP_SIG)) {
            needed = (gint) (sizeof(JXTA_UDP_SIG) - available);
            break;
        }

        if (tvb_memeql(tvb, offset, JXTA_UDP_SIG, sizeof(JXTA_UDP_SIG)) != 0) {
            /
            return 0;
        }

        offset += sizeof(JXTA_UDP_SIG);

        jxta_message_framing_tvb = tvb_new_subset(tvb, offset, -1, -1);
        processed = dissect_jxta_message_framing(jxta_message_framing_tvb, pinfo, NULL, &content_length, &content_type);

        if ((0 == processed) || (NULL == content_type) || (content_length <= 0) || (content_length > UINT_MAX)) {
            /
            return 0;
        }

        if (processed < 0) {
            needed = -processed;
            break;
        }
        
        /
        processed = dissect_jxta_message_framing(jxta_message_framing_tvb, pinfo, tree, &content_length, &content_type);


        offset += processed;

        available = tvb_reported_length_remaining(tvb, offset);
        if (available < content_length) {
            needed = (gint) (content_length - available);
            break;
        }

        offset += (guint) content_length;

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

    if (tree) {
        guint tree_offset = 0;
        proto_item *jxta_tree_item =
            proto_tree_add_protocol_format(tree, proto_jxta, tvb, offset, -1, "JXTA" );
        proto_tree *jxta_tree = proto_item_add_subtree(jxta_tree_item, ett_jxta);        
        proto_item *jxta_udp_tree_item =
            proto_tree_add_none_format(jxta_tree, hf_jxta_udp, tvb, tree_offset, -1, "JXTA UDP Message");
        proto_tree *jxta_udp_tree = proto_item_add_subtree(jxta_udp_tree_item, ett_jxta_udp);
        tvbuff_t *jxta_message_framing_tvb;
        guint64 content_length = -1;
        gchar *content_type = NULL;
        tvbuff_t *jxta_message_tvb;
        gboolean dissected = FALSE;
        gint processed = 0;

        proto_tree_add_item(jxta_udp_tree, hf_jxta_udpsig, tvb, tree_offset, sizeof(JXTA_UDP_SIG), FALSE);
        tree_offset += sizeof(JXTA_UDP_SIG);

        jxta_message_framing_tvb = tvb_new_subset(tvb, tree_offset, -1, -1);
        processed = dissect_jxta_message_framing(jxta_message_framing_tvb, pinfo, NULL, &content_length, &content_type);

        if ((0 == processed) || (NULL == content_type) || (content_length <= 0) || (content_length > UINT_MAX)) {
            /
            return 0;
        }

        processed = dissect_jxta_message_framing(jxta_message_framing_tvb, pinfo, tree, &content_length, &content_type);

        tree_offset += processed;

        jxta_message_tvb = tvb_new_subset(tvb, tree_offset, (gint) content_length, (gint) content_length);

        dissected = dissector_try_string(media_type_dissector_table, content_type, jxta_message_tvb, pinfo, tree);

        if (!dissected) {
            call_dissector(data_handle, jxta_message_tvb, pinfo, tree);
        }

        tree_offset += (guint) content_length;

        proto_item_set_end(jxta_udp_tree_item, tvb, tree_offset);

        DISSECTOR_ASSERT(offset == tree_offset);
    }

    return offset;
}
