static int dissect_jxta_message(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
    int offset = 0;
    int available;
    int needed = 0;

    while (TRUE) {
        /
        available = tvb_reported_length_remaining(tvb, offset);
        if (available < (int)sizeof(JXTA_MSG_SIG)) {
            needed = available - sizeof(JXTA_MSG_SIG);
            break;
        }

        if (tvb_memeql(tvb, offset, JXTA_MSG_SIG, sizeof(JXTA_MSG_SIG)) != 0) {
            /
            return 0;
        }

        offset += sizeof(JXTA_MSG_SIG);

        available = tvb_reported_length_remaining(tvb, offset);
        if (available < (int)sizeof(guint8)) {
            needed = available - sizeof(guint8);
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
        if (available < (int)sizeof(guint16)) {
            needed = available - sizeof(guint16);
            break;
        } else {
            guint16 msg_ns_count = tvb_get_ntohs(tvb, offset);
            unsigned int each_namespace;

            offset += sizeof(guint16);

            for (each_namespace = 0; each_namespace < msg_ns_count; each_namespace++) {
                guint8 namespace_len;

                available = tvb_reported_length_remaining(tvb, offset);
                if (available < (int)sizeof(guint8)) {
                    needed = available - sizeof(guint8);
                    break;
                }

                namespace_len = tvb_get_guint8(tvb, offset);

                available = tvb_reported_length_remaining(tvb, offset + 1);
                if (available < namespace_len) {
                    needed = available - namespace_len;
                    break;
                }

                offset += sizeof(guint8) + namespace_len;
            }
        }

        /
        available = tvb_reported_length_remaining(tvb, offset);
        if (available < (int)sizeof(guint16)) {
            needed = available - sizeof(guint16);
            break;
        } else {
            guint16 elem_count = tvb_get_ntohs(tvb, offset);
            unsigned int each_elem;

            offset += sizeof(guint16);

            /

            for (each_elem = 0; each_elem < elem_count; each_elem++) {
                tvbuff_t *jxta_message_element_tvb = tvb_new_subset(tvb, offset, -1, -1);
                int processed = dissect_jxta_message_element(jxta_message_element_tvb, pinfo, NULL, 0, NULL);

                if (processed <= 0) {
                    return processed;
                }

                offset += processed;
            }
        }

        break;
    }

    if ((needed > 0) && gDESEGMENT && pinfo->can_desegment) {
        return -needed;
    }

    if (tree) {
        int tree_offset = 0;
        proto_item *jxta_msg_tree_item = NULL;
        proto_tree *jxta_msg_tree = NULL;
        guint8 message_version;
        const char **namespaces = NULL;
        guint16 msg_ns_count;
        unsigned int each_namespace;
        guint16 elem_count;
        unsigned int each_elem;
        
        if ((AT_STRINGZ == pinfo->src.type) && (AT_STRINGZ == pinfo->dst.type)) {
	    jxta_msg_tree_item = proto_tree_add_protocol_format(tree, hf_jxta_message, tvb, tree_offset, -1,
	    "JXTA Message, %s -> %s", pinfo->src.data, pinfo->dst.data );
        } else {
	    jxta_msg_tree_item = proto_tree_add_item(tree, hf_jxta_message, tvb, tree_offset, -1, FALSE);
        }

        jxta_msg_tree = proto_item_add_subtree(jxta_msg_tree_item, ett_jxta_msg);

        if ( AT_STRINGZ == pinfo->src.type ) {
            proto_item * src_item = proto_tree_add_item(jxta_msg_tree, hf_jxta_message_src, tvb, 0, 0, FALSE);
            proto_item_append_text( src_item, "%s", pinfo->src.data );
            PROTO_ITEM_SET_GENERATED(src_item);
        }

        if ( AT_STRINGZ == pinfo->dst.type) {
            proto_item * dst_item = proto_tree_add_item(jxta_msg_tree, hf_jxta_message_dst, tvb, 0, 0, FALSE);
            proto_item_append_text( dst_item, "%s", pinfo->dst.data );
            PROTO_ITEM_SET_GENERATED(dst_item);
        }

        proto_tree_add_item(jxta_msg_tree, hf_jxta_message_sig, tvb, tree_offset, sizeof(JXTA_MSG_SIG), FALSE);
        tree_offset += sizeof(JXTA_MSG_SIG);

        message_version = tvb_get_guint8(tvb, tree_offset);
        proto_tree_add_uint(jxta_msg_tree, hf_jxta_message_version, tvb, tree_offset, sizeof(guint8), message_version);
        tree_offset += sizeof(guint8);

        msg_ns_count = tvb_get_ntohs(tvb, tree_offset);
        proto_tree_add_uint(jxta_msg_tree, hf_jxta_message_namespaces_count, tvb, tree_offset, sizeof(guint16), msg_ns_count);
        tree_offset += sizeof(guint16);

        namespaces = g_malloc((msg_ns_count + 2) * sizeof(const char *));
        namespaces[0] = "";
        namespaces[1] = "jxta";

        /
        for (each_namespace = 0; each_namespace < msg_ns_count; each_namespace++) {
            guint8 namespace_len = tvb_get_guint8(tvb, tree_offset);

            namespaces[2 + each_namespace] = tvb_get_string(tvb, tree_offset + 1, namespace_len);
            proto_tree_add_item(jxta_msg_tree, hf_jxta_message_namespace_name, tvb, tree_offset, sizeof(guint8), FALSE);
            tree_offset += sizeof(guint8) + namespace_len;
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

        for (each_namespace = 2; each_namespace < msg_ns_count; each_namespace++) {
            g_free((char *)namespaces[each_namespace]);
        }

        g_free((char *)namespaces);

        proto_item_set_end(jxta_msg_tree_item, tvb, tree_offset);

        DISSECTOR_ASSERT(tree_offset == offset);              
    }

    if ((offset > 0) && (AT_STRINGZ == pinfo->src.type) && (AT_STRINGZ == pinfo->dst.type)) {
        jxta_tap_header * tap_header = (jxta_tap_header *) g_mem_chunk_alloc(my_tap_structs);
        
        tap_header->src_address = pinfo->src;
        tap_header->dest_address = pinfo->dst;
        tap_header->size = offset;

        tap_queue_packet(jxta_tap, pinfo, tap_header);
    }

    return offset;
}
