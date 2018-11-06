static void
add_capabilities (proto_tree *tree, packet_info *pinfo, tvbuff_t *tvb, guint8 pdu_type)
{
    proto_tree *wsp_capabilities, *cap_subtree, *cap_subtree2;
    proto_item *ti, *cap_item, *cap_item2;

    char       *capaName, *str;
    guint32     offset       = 0;
    guint32     len          = 0;
    guint32     capaStart    = 0; /
    guint32     capaLen      = 0; /
    guint32     capaValueLen = 0; /
    guint32     tvb_len      = tvb_reported_length(tvb);
    gboolean    ok           = FALSE;
    guint8      peek;
    guint32     value;

    if (tvb_len == 0) {
        return;
    }

    ti = proto_tree_add_item(tree, hf_capabilities_section,
            tvb, 0, tvb_len, ENC_NA);
    wsp_capabilities = proto_item_add_subtree(ti, ett_capabilities);

    while (offset < tvb_len) {
        /
        capaStart = offset;
        /
        capaValueLen = tvb_get_guintvar(tvb, offset, &len);
        capaLen = capaValueLen + len;

        cap_subtree = proto_tree_add_subtree(wsp_capabilities, tvb, offset, capaLen, ett_capabilities_entry, &cap_item, "Capability");
        offset += len;
        /
        peek = tvb_get_guint8(tvb, offset);
        if (is_token_text(peek)) { /
            /
            capaName = (gchar *)tvb_get_stringz_enc(wmem_packet_scope(), tvb, capaStart, (gint *)&len, ENC_ASCII);

            /
            if (g_ascii_strcasecmp(capaName, "client-sdu-size") == 0) {
                peek = WSP_CAPA_CLIENT_SDU_SIZE;
            } else if (g_ascii_strcasecmp(capaName, "server-sdu-size") == 0) {
                peek = WSP_CAPA_SERVER_SDU_SIZE;
            } else if (g_ascii_strcasecmp(capaName, "protocol options") == 0) {
                peek = WSP_CAPA_PROTOCOL_OPTIONS;
            } else if (g_ascii_strcasecmp(capaName, "method-mor") == 0) {
                peek = WSP_CAPA_METHOD_MOR;
            } else if (g_ascii_strcasecmp(capaName, "push-mor") == 0) {
                peek = WSP_CAPA_PUSH_MOR;
            } else if (g_ascii_strcasecmp(capaName, "extended methods") == 0) {
                peek = WSP_CAPA_EXTENDED_METHODS;
            } else if (g_ascii_strcasecmp(capaName, "header code pages") == 0) {
                peek = WSP_CAPA_HEADER_CODE_PAGES;
            } else if (g_ascii_strcasecmp(capaName, "aliases") == 0) {
                peek = WSP_CAPA_ALIASES;
            } else if (g_ascii_strcasecmp(capaName, "client-message-size") == 0) {
                peek = WSP_CAPA_CLIENT_MESSAGE_SIZE;
            } else if (g_ascii_strcasecmp(capaName, "server-message-size") == 0) {
                peek = WSP_CAPA_SERVER_MESSAGE_SIZE;
            } else {
                expert_add_info_format(pinfo, cap_item, &ei_wsp_capability_invalid,
                        "Unknown or invalid textual capability: %s", capaName);
                /
                offset = capaStart + capaLen;
                continue;
            }
            offset += len;
            /
        } else if (peek < 0x80) {
            expert_add_info_format(pinfo, cap_item, &ei_wsp_capability_invalid,
                    "Invalid well-known capability: 0x%02X", peek);
            /
            return;
        }
        if (peek & 0x80) { /
            peek &= 0x7F;
            len = 1;
            offset++;
            /
        }

        proto_item_append_text(cap_item, ": %s", val_to_str_const(peek, wsp_capability_vals, "Invalid capabiliity"));
        /
        switch (peek) {
            case WSP_CAPA_CLIENT_SDU_SIZE:
                value = tvb_get_guintvar(tvb, offset, &len);
                proto_tree_add_uint(cap_subtree, hf_capa_client_sdu_size,
                        tvb, offset, len, value);
                break;
            case WSP_CAPA_SERVER_SDU_SIZE:
                value = tvb_get_guintvar(tvb, offset, &len);
                proto_tree_add_uint(cap_subtree, hf_capa_server_sdu_size,
                        tvb, offset, len, value);
                break;
            case WSP_CAPA_PROTOCOL_OPTIONS:
                /
                if (capaValueLen - len == 1) {
                    static const int * capabilities[] = {
                        &hf_capa_protocol_option_confirmed_push,
                        &hf_capa_protocol_option_push,
                        &hf_capa_protocol_option_session_resume,
                        &hf_capa_protocol_option_ack_headers,
                        &hf_capa_protocol_option_large_data_transfer,
                        NULL
                    };

                    proto_tree_add_bitmask_with_flags(cap_subtree, tvb, offset, hf_capa_protocol_options,
                                   ett_proto_option_capability, capabilities, ENC_NA, BMT_NO_FALSE);
                }
                else
                {
                    /
                    proto_item_append_text(cap_item,
                            " <warning: bit field too large>");
                    offset = capaStart + capaLen;
                    continue;
                }
                break;
            case WSP_CAPA_METHOD_MOR:
                proto_tree_add_item(cap_subtree, hf_capa_method_mor, tvb, offset, len, ENC_NA);
                break;
            case WSP_CAPA_PUSH_MOR:
                proto_tree_add_item(cap_subtree, hf_capa_push_mor, tvb, offset, len, ENC_NA);
               break;
            case WSP_CAPA_EXTENDED_METHODS:
                /
                cap_subtree2 = proto_tree_add_subtree(cap_subtree, tvb, capaStart, capaLen, ett_capabilities_extended_methods, &cap_item2, "Extended Methods");
                if (pdu_type == WSP_PDU_CONNECT) {
                    while (offset < capaStart + capaLen) {
                        ti = proto_tree_add_item(cap_subtree2, hf_capa_extended_method, tvb, offset, 1, ENC_NA);
                        offset++;

                        get_text_string(str, tvb, offset, len, ok);
                        if (! ok) {
                            expert_add_info(pinfo, ti, &ei_wsp_capability_encoding_invalid);
                            return;
                        }
                        proto_item_append_text(ti, " = %s", str);
                        proto_item_set_len(ti, len+1);
                        offset += len;
                    }
                } else {
                    while (offset < capaStart + capaLen) {
                        proto_tree_add_item(cap_subtree2, hf_capa_extended_method, tvb, offset, 1, ENC_NA);
                        offset++;
                    }
                }
                break;
            case WSP_CAPA_HEADER_CODE_PAGES:
                /
                cap_subtree2 = proto_tree_add_subtree(cap_subtree, tvb, capaStart, capaLen, ett_capabilities_header_code_pages, &cap_item2, "Header Code Pages");
                if (pdu_type == WSP_PDU_CONNECT) {
                    while (offset < capaStart + capaLen) {
                        ti = proto_tree_add_item(cap_subtree2, hf_capa_header_code_page, tvb, offset, 1, ENC_NA);
                        offset++;

                        get_text_string(str, tvb, offset, len, ok);
                        if (! ok) {
                            expert_add_info(pinfo, ti, &ei_wsp_capability_encoding_invalid);
                            return;
                        }
                        proto_item_append_text(ti, " = %s", str);
                        proto_item_set_len(ti, len+1);
                        offset += len;
                    }
                } else {
                    while (offset < capaStart + capaLen) {
                        proto_tree_add_item(cap_subtree2, hf_capa_header_code_page, tvb, offset, 1, ENC_NA);
                        offset++;
                    }
                }
                break;
            case WSP_CAPA_ALIASES:
                /
                proto_tree_add_item(cap_subtree, hf_capa_aliases,
                        tvb, capaStart, capaLen, ENC_NA);
                break;
            case WSP_CAPA_CLIENT_MESSAGE_SIZE:
                value = tvb_get_guintvar(tvb, offset, &len);
                proto_tree_add_uint(cap_subtree, hf_capa_client_message_size,
                        tvb, offset, len, value);
                break;
            case WSP_CAPA_SERVER_MESSAGE_SIZE:
                value = tvb_get_guintvar(tvb, offset, &len);
                proto_tree_add_uint(cap_subtree, hf_capa_server_message_size,
                        tvb, offset, len, value);
                break;
            default:
                expert_add_info_format(pinfo, cap_item, &ei_wsp_capability_invalid,
                        "Unknown well-known capability: 0x%02X", peek);
                break;
        }
        offset = capaStart + capaLen;
    }
}
