static int dissect_jxta_message(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
    guint offset = 0;
    guint available;
    gint needed = 0;

    while (TRUE) {
        /
        available = tvb_reported_length_remaining(tvb, offset);
        if (available < sizeof(JXTA_MSG_SIG)) {
            needed = (gint) (sizeof(JXTA_MSG_SIG) - available);
            break;
        }

        if (tvb_memeql(tvb, offset, JXTA_MSG_SIG, sizeof(JXTA_MSG_SIG)) != 0) {
            /
            return 0;
        }

        offset += sizeof(JXTA_MSG_SIG);

        available = tvb_reported_length_remaining(tvb, offset);
        if (available < sizeof(guint8)) {
            needed = (gint) (sizeof(guint8) - available);
            break;
        } else {
            guint8 message_version = tvb_get_guint8(tvb, offset);

            offset += sizeof(guint8);

            if (0 != message_version) {
                /
                return 0;
            }
        }

        available = tvb_reported_length_remaining(tvb, offset);
        if (available < sizeof(guint16)) {
            needed = (gint) (sizeof(guint16) - available);
            break;
        } else {
            guint16 msg_ns_count = tvb_get_ntohs(tvb, offset);
            guint each_namespace;

            offset += sizeof(guint16);

            for (each_namespace = 0; each_namespace < msg_ns_count; each_namespace++) {
                guint16 namespace_len;

                available = tvb_reported_length_remaining(tvb, offset);
                if (available < sizeof(namespace_len)) {
                    needed = (gint) (sizeof(namespace_len) - available);
                    break;
                }

                namespace_len = tvb_get_ntohs(tvb, offset);

                available = tvb_reported_length_remaining(tvb, offset + sizeof(namespace_len));
                if (available < namespace_len) {
                    needed = (gint) (namespace_len - available);
                    break;
                }

                offset += sizeof(namespace_len) + namespace_len;
            }
        }

        /
        available = tvb_reported_length_remaining(tvb, offset);
        if (available < sizeof(guint16)) {
            needed = (gint) (sizeof(guint16) - available);
            break;
        } else {
            guint16 elem_count = tvb_get_ntohs(tvb, offset);
            guint each_elem;

            offset += sizeof(guint16);

            /

            for (each_elem = 0; each_elem < elem_count; each_elem++) {
                tvbuff_t *jxta_message_element_tvb = tvb_new_subset(tvb, offset, -1, -1);
                int processed = dissect_jxta_message_element(jxta_message_element_tvb, pinfo, NULL, 0, NULL);

                if (processed < 0) {
                    needed = -processed;
                    break;
                }

                if (0 == processed) {
                    /
                    g_warning( "Failure processing message element #%d of %d of frame %d", each_elem, elem_count, pinfo->fd->num ); 
                    return 0;
                }

                offset += processed;
            }
        }

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
        gchar src_addr[256];
        gchar dst_addr[256];

        address_to_str_buf(&pinfo->src, src_addr, sizeof src_addr);
        address_to_str_buf(&pinfo->dst, dst_addr, sizeof dst_addr);

        /
        if (PT_NONE != pinfo->ptype) {
            size_t len = strlen(src_addr);
            src_addr[len] = ':';
            g_snprintf(&src_addr[len + 1], 256 - len - 1, "%d", pinfo->srcport);

            len = strlen(dst_addr);
            dst_addr[len] = ':';
            g_snprintf(&dst_addr[len + 1], 256 - len - 1, "%d", pinfo->destport);
        }

        col_add_fstr(pinfo->cinfo, COL_INFO, "Message, %s -> %s", src_addr, dst_addr);

        col_set_writable(pinfo->cinfo, FALSE);
    }

    if (tree) {
        guint tree_offset = 0;
        proto_item *jxta_msg_tree_item = NULL;
        proto_tree *jxta_msg_tree = NULL;
        guint8 message_version;
        const gchar **namespaces = NULL;
        guint16 msg_ns_count;
        guint each_namespace;
        guint16 elem_count;
        guint each_elem;
        gchar src_addr[256];
        gchar dst_addr[256];
        proto_item *tree_item;

        address_to_str_buf(&pinfo->src, src_addr, sizeof src_addr);
        address_to_str_buf(&pinfo->dst, dst_addr, sizeof dst_addr);

        if (PT_NONE != pinfo->ptype) {
            size_t len = strlen(src_addr);
            src_addr[len] = ':';
            g_snprintf(&src_addr[len + 1], 256 - len - 1, "%d", pinfo->srcport);

            len = strlen(dst_addr);
            dst_addr[len] = ':';
            g_snprintf(&dst_addr[len + 1], 256 - len - 1, "%d", pinfo->destport);
        }

        jxta_msg_tree_item = proto_tree_add_protocol_format(tree, proto_message_jxta, tvb, tree_offset, -1,
                                                            "JXTA Message, %s -> %s", src_addr, dst_addr);

        jxta_msg_tree = proto_item_add_subtree(jxta_msg_tree_item, ett_jxta_msg);

        proto_tree_add_item(jxta_msg_tree, hf_jxta_message_sig, tvb, tree_offset, sizeof(JXTA_MSG_SIG), FALSE);
        tree_offset += sizeof(JXTA_MSG_SIG);

        tree_item = proto_tree_add_string(jxta_msg_tree, hf_jxta_message_src, tvb, 0, 0, src_addr);
        PROTO_ITEM_SET_GENERATED(tree_item);

        tree_item = proto_tree_add_string(jxta_msg_tree, hf_jxta_message_address, tvb, 0, 0, src_addr);
        PROTO_ITEM_SET_HIDDEN(tree_item);
        PROTO_ITEM_SET_GENERATED(tree_item);

        tree_item = proto_tree_add_string(jxta_msg_tree, hf_jxta_message_dst, tvb, 0, 0, dst_addr);
        PROTO_ITEM_SET_GENERATED(tree_item);

        tree_item = proto_tree_add_string(jxta_msg_tree, hf_jxta_message_address, tvb, 0, 0, dst_addr);
        PROTO_ITEM_SET_HIDDEN(tree_item);
        PROTO_ITEM_SET_GENERATED(tree_item);

        message_version = tvb_get_guint8(tvb, tree_offset);
        proto_tree_add_uint(jxta_msg_tree, hf_jxta_message_version, tvb, tree_offset, sizeof(guint8), message_version);
        tree_offset += sizeof(guint8);

        msg_ns_count = tvb_get_ntohs(tvb, tree_offset);
        proto_tree_add_uint(jxta_msg_tree, hf_jxta_message_namespaces_count, tvb, tree_offset, sizeof(guint16), msg_ns_count);
        tree_offset += sizeof(guint16);

        namespaces = ep_alloc((msg_ns_count + 2) * sizeof(const gchar *));
        namespaces[0] = "";
        namespaces[1] = "jxta";

        /
        for (each_namespace = 0; each_namespace < msg_ns_count; each_namespace++) {
            guint16 namespace_len = tvb_get_ntohs(tvb, tree_offset);

            namespaces[2 + each_namespace] = tvb_get_ephemeral_string(tvb, tree_offset + sizeof(namespace_len), namespace_len);
            proto_tree_add_item(jxta_msg_tree, hf_jxta_message_namespace_name, tvb, tree_offset, sizeof(namespace_len), FALSE);
            tree_offset += sizeof(namespace_len) + namespace_len;
        }

        /
        elem_count = tvb_get_ntohs(tvb, tree_offset);
        proto_tree_add_item(jxta_msg_tree, hf_jxta_message_element_count, tvb, tree_offset, sizeof(guint16), FALSE);
        tree_offset += sizeof(guint16);

        /
        for (each_elem = 0; each_elem < elem_count; each_elem++) {
            tvbuff_t *jxta_message_element_tvb = tvb_new_subset(tvb, tree_offset, -1, -1);

            tree_offset +=
                dissect_jxta_message_element(jxta_message_element_tvb, pinfo, jxta_msg_tree, msg_ns_count + 2, namespaces);
        }

        proto_item_set_end(jxta_msg_tree_item, tvb, tree_offset);

        DISSECTOR_ASSERT(tree_offset == offset);
    }

    if ((offset > 0) && (AT_URI == pinfo->src.type) && (AT_URI == pinfo->dst.type)) {
        jxta_tap_header *tap_header = se_alloc(sizeof(jxta_tap_header));

        tap_header->src_address = pinfo->src;
        tap_header->dest_address = pinfo->dst;
        tap_header->size = offset;

        tap_queue_packet(jxta_tap, pinfo, tap_header);
    }

    return offset;
}
