static void dissect_rar(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, proto_item *pdu_ti,
                        gint offset, mac_lte_info *p_mac_lte_info, mac_lte_tap_info *tap_info)
{
    gint     number_of_rars = 0;   /
    guint8   rapids[64];
    gboolean backoff_indicator_seen = FALSE;
    guint8   backoff_indicator = 0;
    guint8   extension;
    gint     n;
    proto_tree *rar_headers_tree;
    proto_item *ti;
    proto_item *rar_headers_ti;
    proto_item *padding_length_ti;
    int        start_headers_offset = offset;

    write_pdu_label_and_info(pdu_ti, NULL, pinfo,
                             "RAR (RA-RNTI=%u, SF=%u) ",
                             p_mac_lte_info->rnti, p_mac_lte_info->subframeNumber);

    /
    ti = proto_tree_add_item(tree, hf_mac_lte_rar, tvb, offset, -1, FALSE);
    PROTO_ITEM_SET_HIDDEN(ti);

    /
    rar_headers_ti = proto_tree_add_item(tree,
                                         hf_mac_lte_rar_headers,
                                         tvb, offset, 0, FALSE);
    rar_headers_tree = proto_item_add_subtree(rar_headers_ti, ett_mac_lte_rar_headers);


    /
    /
    do {
        int start_header_offset = offset;
        proto_tree *rar_header_tree;
        proto_item *rar_header_ti;
        guint8 type_value;
        guint8 first_byte = tvb_get_guint8(tvb, offset);

        /
        rar_header_ti = proto_tree_add_item(rar_headers_tree,
                                            hf_mac_lte_rar_header,
                                            tvb, offset, 0, FALSE);
        rar_header_tree = proto_item_add_subtree(rar_header_ti, ett_mac_lte_rar_header);

        /
        extension = (first_byte & 0x80) >> 7;
        proto_tree_add_item(rar_header_tree, hf_mac_lte_rar_extension, tvb, offset, 1, FALSE);

        /
        type_value = (first_byte & 0x40) >> 6;
        proto_tree_add_item(rar_header_tree, hf_mac_lte_rar_t, tvb, offset, 1, FALSE);

        if (type_value == 0) {
            /

            guint8 reserved;
            proto_item *tii;
            proto_item *bi_ti;

            /
            reserved = (tvb_get_guint8(tvb, offset) & 0x30) >> 4;
            tii = proto_tree_add_item(rar_header_tree, hf_mac_lte_rar_reserved, tvb, offset, 1, FALSE);
            if (reserved != 0) {
                expert_add_info_format(pinfo, tii, PI_MALFORMED, PI_ERROR,
                                       "RAR header Reserved bits not zero (found 0x%x)", reserved);
            }

            /
            backoff_indicator = tvb_get_guint8(tvb, offset) & 0x0f;
            bi_ti = proto_tree_add_item(rar_header_tree, hf_mac_lte_rar_bi, tvb, offset, 1, FALSE);

            /
            if (backoff_indicator_seen) {
                expert_add_info_format(pinfo, bi_ti, PI_MALFORMED, PI_ERROR,
                                       "MAC RAR PDU has > 1 Backoff Indicator subheader present");
            }
            backoff_indicator_seen = TRUE;

            write_pdu_label_and_info(pdu_ti, rar_header_ti, pinfo,
                                     "(Backoff Indicator=%sms)",
                                     val_to_str_const(backoff_indicator, rar_bi_vals, "Illegal-value "));

            /
            if (number_of_rars > 0) {
                expert_add_info_format(pinfo, bi_ti, PI_MALFORMED, PI_WARN,
                                       "Backoff Indicator must appear as first subheader");
            }

        }
        else {
            /
            /
            rapids[number_of_rars] = tvb_get_guint8(tvb, offset) & 0x3f;
            proto_tree_add_item(rar_header_tree, hf_mac_lte_rar_rapid, tvb, offset, 1, FALSE);

            proto_item_append_text(rar_header_ti, "(RAPID=%u)", rapids[number_of_rars]);

            number_of_rars++;
        }

        offset++;

        /
        proto_item_set_len(rar_header_ti, offset - start_header_offset);

    } while (extension);

    /
    proto_item_append_text(rar_headers_ti, " (%u RARs", number_of_rars);
    if (backoff_indicator_seen) {
        proto_item_append_text(rar_headers_ti, ", BI=%sms)",
                               val_to_str_const(backoff_indicator, rar_bi_vals, "Illegal-value "));
    }
    else {
        proto_item_append_text(rar_headers_ti, ")");
    }

    /
    proto_item_set_len(rar_headers_ti, offset-start_headers_offset);


    /
    /
    for (n=0; n < number_of_rars; n++) {
        offset = dissect_rar_entry(tvb, pinfo, tree, pdu_ti, offset, rapids[n]);
    }

    /
    tap_info->number_of_rars += number_of_rars;

    /
    if (tvb_length_remaining(tvb, offset) > 0) {
        proto_tree_add_item(tree, hf_mac_lte_padding_data,
                            tvb, offset, -1, FALSE);
    }
    padding_length_ti = proto_tree_add_int(tree, hf_mac_lte_padding_length,
                                           tvb, offset, 0,
                                           p_mac_lte_info->length - offset);
    PROTO_ITEM_SET_GENERATED(padding_length_ti);

    /
    tap_info->padding_bytes += (p_mac_lte_info->length - offset);
}
