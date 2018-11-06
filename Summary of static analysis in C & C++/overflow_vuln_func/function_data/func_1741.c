static int dissect_jxta_message_element(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree, guint ns_count,
                                        const gchar ** namespaces)
{
    guint offset = 0;
    guint available;
    gint needed = 0;
    guint8 flags;

    /

    while (TRUE) {
        /
        available = tvb_reported_length_remaining(tvb, offset);
        if (available < sizeof(JXTA_MSGELEM_SIG)) {
            needed = (gint) (sizeof(JXTA_MSGELEM_SIG) - available);
        }

        if (tvb_memeql(tvb, offset, JXTA_MSGELEM_SIG, sizeof(JXTA_MSGELEM_SIG)) != 0) {
            /
            return 0;
        }

        offset += sizeof(JXTA_MSGELEM_SIG);

        /
        available = tvb_reported_length_remaining(tvb, offset);
        if (available < sizeof(guint8)) {
            needed = (gint) (sizeof(guint8) - available);
            break;
        }

        offset += sizeof(guint8);

        /
        available = tvb_reported_length_remaining(tvb, offset);
        if (available < sizeof(guint8)) {
            needed = (gint) (sizeof(guint8) - available);
            break;
        } else {
            flags = tvb_get_guint8(tvb, offset);
            offset += sizeof(guint8);
        }

        /
        available = tvb_reported_length_remaining(tvb, offset);
        if (available < sizeof(guint16)) {
            needed = (gint) (sizeof(guint16) - available);
            break;
        } else {
            guint16 name_len = tvb_get_ntohs(tvb, offset);
            offset += sizeof(guint16);

            available = tvb_reported_length_remaining(tvb, offset);
            if (available < name_len) {
                needed = (gint) (name_len - available);
                break;
            }

            offset += name_len;
        }

        /
        if ((flags & 0x01) != 0) {
            guint16 type_len;

            available = tvb_reported_length_remaining(tvb, offset);
            if (available < sizeof(guint16)) {
                needed = (gint) (sizeof(guint16) - available);
                break;
            }

            type_len = tvb_get_ntohs(tvb, offset);
            offset += sizeof(guint16);

            available = tvb_reported_length_remaining(tvb, offset);
            if (available < type_len) {
                needed = (gint) (type_len - available);
                break;
            }

            offset += type_len;
        }

        /
        if ((flags & 0x02) != 0) {
            guint16 encoding_len;

            available = tvb_reported_length_remaining(tvb, offset);
            if (available < sizeof(guint16)) {
                needed = (gint) (sizeof(guint16) - available);
                break;
            }

            encoding_len = tvb_get_ntohs(tvb, offset);
            offset += sizeof(guint16);

            available = tvb_reported_length_remaining(tvb, offset);
            if (available < encoding_len) {
                needed = (gint) (encoding_len - available);
                break;
            }

            offset += encoding_len;
        }

        /
        available = tvb_reported_length_remaining(tvb, offset);
        if (available < sizeof(guint16)) {
            needed = (gint) (sizeof(guint16) - available);
            break;
        } else {
            guint32 content_len = tvb_get_ntohl(tvb, offset);
            offset += sizeof(guint32);

            available = tvb_reported_length_remaining(tvb, offset);
            if (available < content_len) {
                needed = (gint) (content_len - available);
                break;
            }

            offset += content_len;
        }

        /
        if ((flags & 0x04) != 0) {
            tvbuff_t *jxta_signature_element_tvb;
            int processed;

            jxta_signature_element_tvb = tvb_new_subset(tvb, offset, -1, -1);

            processed = dissect_jxta_message_element(jxta_signature_element_tvb, pinfo, NULL, 0, NULL);

            if (processed == 0) {
                return offset;
            }

            if (processed < 0) {
                needed = -processed;
                break;
            }

            offset += processed;
        }

        break;
    }

    if ((needed > 0) && gDESEGMENT && pinfo->can_desegment) {
        pinfo->desegment_offset = 0;
        pinfo->desegment_len = needed;
        return -needed;
    }

    /
    if (tree) {
        guint tree_offset = 0;
        proto_item *jxta_elem_tree_item = proto_tree_add_item(tree, hf_jxta_element, tvb, tree_offset, -1, FALSE);
        proto_tree *jxta_elem_tree = proto_item_add_subtree(jxta_elem_tree_item, ett_jxta_elem);
        guint8 namespaceID;
        proto_item *namespace_ti;
        guint16 name_len;
        proto_item *flags_ti;
        proto_tree *jxta_elem_flags_tree = NULL;
        guint32 content_len;
        gchar *mediatype = NULL;
        gboolean media_type_recognized = FALSE;
        tvbuff_t *element_content_tvb;
        proto_item * jxta_elem_length_item = NULL;

        proto_tree_add_item(jxta_elem_tree, hf_jxta_element_sig, tvb, tree_offset, sizeof(JXTA_MSGELEM_SIG), FALSE);
        tree_offset += sizeof(JXTA_MSGELEM_SIG);

        namespaceID = tvb_get_guint8(tvb, tree_offset);
        namespace_ti =
            proto_tree_add_uint(jxta_elem_tree, hf_jxta_element_namespaceid, tvb, tree_offset, sizeof(guint8), namespaceID);
        if (namespaceID < ns_count) {
            proto_item_append_text(namespace_ti, " (%s)", namespaces[namespaceID]);
        } else {
            proto_item_append_text(namespace_ti, " * BAD *");
        }
        tree_offset += sizeof(guint8);

        flags = tvb_get_guint8(tvb, tree_offset);
        flags_ti = proto_tree_add_uint(jxta_elem_tree, hf_jxta_element_flags, tvb, tree_offset, sizeof(guint8), flags);
        jxta_elem_flags_tree = proto_item_add_subtree(flags_ti, ett_jxta_elem_flags);
        proto_tree_add_boolean(jxta_elem_flags_tree, hf_jxta_element_flag_hasType, tvb, tree_offset, 1, flags);
        proto_tree_add_boolean(jxta_elem_flags_tree, hf_jxta_element_flag_hasEncoding, tvb, tree_offset, 1, flags);
        proto_tree_add_boolean(jxta_elem_flags_tree, hf_jxta_element_flag_hasSignature, tvb, tree_offset, 1, flags);
        tree_offset += sizeof(guint8);

        name_len = tvb_get_ntohs(tvb, tree_offset);
        proto_item_append_text(jxta_elem_tree_item, " \"%s\"", tvb_format_text(tvb, tree_offset + sizeof(guint16), name_len));
        proto_tree_add_item(jxta_elem_tree, hf_jxta_element_name, tvb, tree_offset, sizeof(guint16), FALSE);
        tree_offset += sizeof(guint16) + name_len;

        /
        if ((flags & 0x01) != 0) {
            guint16 type_len = tvb_get_ntohs(tvb, tree_offset);
            proto_tree_add_item(jxta_elem_tree, hf_jxta_element_type, tvb, tree_offset, sizeof(guint16), FALSE);
            tree_offset += sizeof(guint16);

            mediatype = tvb_get_ephemeral_string(tvb, tree_offset, type_len);

            /
            {
                gchar *parms_at = strchr(mediatype, ';');

                if (NULL != parms_at) {
                    *parms_at = '\0';
                }
            }

            /
#if GLIB_MAJOR_VERSION < 2
            g_strdown(mediatype);
#else
            {
                gchar *mediatype_lowercase = g_ascii_strdown(mediatype, -1);
                mediatype = mediatype_lowercase;
            }
#endif
            tree_offset += type_len;
        }

        /
        if ((flags & 0x02) != 0) {
            guint16 encoding_len = tvb_get_ntohs(tvb, tree_offset);
            proto_tree_add_item(jxta_elem_tree, hf_jxta_element_encoding, tvb, tree_offset, sizeof(guint16), FALSE);
            tree_offset += sizeof(guint16) + encoding_len;
        }

        /
        content_len = tvb_get_ntohl(tvb, tree_offset);
        jxta_elem_length_item = proto_tree_add_item(jxta_elem_tree, hf_jxta_element_content_len, tvb, tree_offset, sizeof(guint32), FALSE);
        tree_offset += sizeof(guint32);

        element_content_tvb = tvb_new_subset(tvb, tree_offset, content_len, content_len);

        if (mediatype) {
            if (0 == strcmp("application/x-jxta-tls-block", mediatype)) {
                /
                dissector_handle_t ssl_handle = find_dissector("ssl");
                if (NULL != ssl_handle) {
                    int processed = call_dissector(ssl_handle, element_content_tvb, pinfo, jxta_elem_tree);
                    media_type_recognized = processed != 0;
                }
            } else if (0 == strcmp("application/gzip", mediatype)) {
                tvbuff_t *uncomp_tvb = tvb_uncompress(element_content_tvb, 0, tvb_length(element_content_tvb));
                
                if( NULL != uncomp_tvb ) {
                    proto_item_append_text( jxta_elem_length_item, " -> (%u uncompressed)", tvb_length(uncomp_tvb) );

                    tvb_set_child_real_data_tvbuff(element_content_tvb, uncomp_tvb);
		    add_new_data_source(pinfo, uncomp_tvb, "Uncompressed Element Content");
                
                    /
                    media_type_recognized = dissector_try_string(media_type_dissector_table,
                                                             "text/xml", uncomp_tvb, pinfo, jxta_elem_tree);
                }
            } else {
                media_type_recognized = dissector_try_string(media_type_dissector_table,
                                                             mediatype, element_content_tvb, pinfo, jxta_elem_tree);
            }

        }

        if (!media_type_recognized) {
            /
            call_dissector(data_handle, element_content_tvb, pinfo, jxta_elem_tree);
        }
        tree_offset += content_len;

        /
        if ((flags & 0x04) != 0) {
            tvbuff_t *jxta_message_element_tvb = tvb_new_subset(tvb, tree_offset, -1, -1);

            tree_offset += dissect_jxta_message_element(jxta_message_element_tvb, pinfo, jxta_elem_tree, ns_count, namespaces);
        }

        proto_item_set_end(jxta_elem_tree_item, tvb, tree_offset);

        DISSECTOR_ASSERT(tree_offset == offset);
    }

    return offset;
}
