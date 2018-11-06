static int dissect_jxta_message_framing(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree, guint64 * content_length,
                                        char **content_type)
{
    int offset = 0;
    int available;
    int needed = 0;

    /
    do {
        guint8 headername_len;
        guint8 headername_offset;
        guint16 headervalue_len;
        guint16 headervalue_offset;

        available = tvb_reported_length_remaining(tvb, offset);
        if (available < (int)sizeof(guint8)) {
            needed = available - sizeof(guint8);
            break;
        } else {
            headername_len = tvb_get_guint8(tvb, offset);
            offset += sizeof(guint8);
            headername_offset = offset;

            available = tvb_reported_length_remaining(tvb, offset);
            if (available < headername_len) {
                needed = available - headername_len;
                break;
            }

            if (0 == headername_len) {
                break;
            }
            offset += headername_len;
        }

        available = tvb_reported_length_remaining(tvb, offset);
        if (available < (int)sizeof(guint16)) {
            needed = available - sizeof(guint16);
            break;
        } else {
            headervalue_len = tvb_get_ntohs(tvb, offset);
            offset += sizeof(guint16);
            headervalue_offset = offset;

            available = tvb_reported_length_remaining(tvb, offset);
            if (available < headervalue_len) {
                needed = available - headervalue_len;
                break;
            }

            offset += headervalue_len;
        }

        if (content_type && (sizeof("content-type") - 1) == headername_len) {
            if (0 == tvb_strncaseeql(tvb, headername_offset, "content-type", sizeof("content-type") - 1)) {
                *content_type = tvb_get_string(tvb, headervalue_offset, headervalue_len);
            }
        }


        if (content_length && (sizeof(guint64) == headervalue_len) && ((sizeof("content-length") - 1) == headername_len)) {
            if (0 == tvb_strncaseeql(tvb, headername_offset, "content-length", sizeof("content-length") - 1)) {
                *content_length = tvb_get_ntoh64(tvb, headervalue_offset);
            }
        }
    } while (TRUE);

    if ((needed > 0) && gDESEGMENT && pinfo->can_desegment) {
        return -needed;
    }

    /
    if (tree) {
        int tree_offset = 0;
        proto_item *framing_tree_item = proto_tree_add_item(tree, hf_jxta_framing, tvb, tree_offset, -1, FALSE);
        proto_tree *framing_tree = proto_item_add_subtree(framing_tree_item, ett_jxta_framing);

        /
        do {
            guint8 headernamelen = tvb_get_guint8(tvb, tree_offset);
            proto_item *framing_header_tree_item =
                proto_tree_add_item(framing_tree, hf_jxta_framing_header, tvb, tree_offset, -1, FALSE);
            proto_tree *framing_header_tree = proto_item_add_subtree(framing_header_tree_item, ett_jxta_framing_header);

            /
            proto_tree_add_item(framing_header_tree, hf_jxta_framing_header_name, tvb, tree_offset, 1, headernamelen);

            /
            if (headernamelen > 0) {
                proto_item_append_text(framing_header_tree_item, " \"%s\"",
                                       tvb_format_text(tvb, tree_offset + sizeof(guint8), headernamelen));
            }

            tree_offset += sizeof(guint8) + headernamelen;

            if (headernamelen > 0) {
                guint16 headervaluelen = tvb_get_ntohs(tvb, tree_offset);

                if (tree) {
                    proto_tree_add_uint(framing_header_tree, hf_jxta_framing_header_value_length, tvb, tree_offset,
                                        sizeof(guint16), headervaluelen);

                /

                    /
                    proto_tree_add_item(framing_header_tree, hf_jxta_framing_header_value, tvb, tree_offset + sizeof(guint16),
                                        headervaluelen, FALSE);
                }

                tree_offset += sizeof(guint16) + headervaluelen;
            }

            proto_item_set_end(framing_header_tree_item, tvb, tree_offset);

            if (0 == headernamelen) {
                break;
            }
        } while (TRUE);

        proto_item_set_end(framing_tree_item, tvb, tree_offset);

        DISSECTOR_ASSERT(offset == tree_offset);
    }

    /
    return offset;
}
