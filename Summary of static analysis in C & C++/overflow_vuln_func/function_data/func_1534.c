static void
dissect_wsp_common(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
    dissector_handle_t dissector_handle, gboolean is_connectionless)
{
    int offset = 0;

    guint8      pdut;
    guint       count            = 0;
    guint       value            = 0;
    guint       uriLength        = 0;
    guint       uriStart         = 0;
    guint       capabilityLength = 0;
    guint       headersLength    = 0;
    guint       headerLength     = 0;
    guint       headerStart      = 0;
    guint       nextOffset       = 0;
    guint       contentTypeStart = 0;
    guint       contentType      = 0;
    const char *contentTypeStr;
    tvbuff_t   *tmp_tvb;
    int         found_match;
    heur_dtbl_entry_t *hdtbl_entry;

/
    proto_item *proto_ti = NULL; /
    proto_tree *wsp_tree = NULL;

    wsp_info_value_t *stat_info;
    stat_info = (wsp_info_value_t *)wmem_alloc(wmem_packet_scope(), sizeof(wsp_info_value_t));
    stat_info->status_code = 0;

/

    /
    if (is_connectionless)
    {
        offset++; /
    };

    /
    pdut = tvb_get_guint8 (tvb, offset);

    /
    col_append_fstr(pinfo->cinfo, COL_INFO, "WSP %s (0x%02x)",
            val_to_str_ext (pdut, &wsp_vals_pdu_type_ext, "Unknown PDU type (0x%02x)"),
            pdut);

    /
    if (tree) {
        proto_ti = proto_tree_add_item(tree, proto_wsp,
                tvb, 0, -1, ENC_NA);
        wsp_tree = proto_item_add_subtree(proto_ti, ett_wsp);
        proto_item_append_text(proto_ti, ", Method: %s (0x%02x)",
                val_to_str_ext (pdut, &wsp_vals_pdu_type_ext, "Unknown (0x%02x)"),
                pdut);

        /

        /
        if (is_connectionless)
        {
            proto_tree_add_item (wsp_tree, hf_wsp_header_tid,
                    tvb, 0, 1, ENC_LITTLE_ENDIAN);
        }
        proto_tree_add_item( wsp_tree, hf_wsp_header_pdu_type,
                tvb, offset, 1, ENC_LITTLE_ENDIAN);
    }
    offset++;

    /
    if ((pdut >= 0x50) && (pdut <= 0x5F)) /
        pdut = WSP_PDU_GET;
    else if ((pdut >= 0x70) && (pdut <= 0x7F)) /
        pdut = WSP_PDU_POST;

    switch (pdut)
    {
        case WSP_PDU_CONNECT:
        case WSP_PDU_CONNECTREPLY:
        case WSP_PDU_RESUME:
            if (pdut == WSP_PDU_CONNECT)
            {
                proto_tree_add_item (wsp_tree, hf_wsp_version_major,
                        tvb, offset, 1, ENC_LITTLE_ENDIAN);
                proto_tree_add_item (wsp_tree, hf_wsp_version_minor,
                        tvb, offset, 1, ENC_LITTLE_ENDIAN);
                {
                    guint8 ver = tvb_get_guint8(tvb, offset);
                    proto_item_append_text(proto_ti, ", Version: %u.%u",
                            ver >> 4, ver & 0x0F);
                }
                offset++;
            } else {
                count = 0;  /
                value = tvb_get_guintvar (tvb, offset, &count);
                proto_tree_add_uint (wsp_tree,
                        hf_wsp_server_session_id,
                        tvb, offset, count, value);
                proto_item_append_text(proto_ti, ", Session ID: %u", value);
                offset += count;
            }
            count = 0;  /
            capabilityLength = tvb_get_guintvar (tvb, offset, &count);
            proto_tree_add_uint (wsp_tree, hf_capabilities_length,
                    tvb, offset, count, capabilityLength);
            offset += count;

            if (pdut != WSP_PDU_RESUME)
            {
                count = 0;  /
                headerLength = tvb_get_guintvar (tvb, offset, &count);
                proto_tree_add_uint (wsp_tree, hf_wsp_header_length,
                        tvb, offset, count, headerLength);
                offset += count;

            } else {
                    /
                headerStart = offset + capabilityLength;
                headerLength = tvb_reported_length_remaining (tvb,
                        headerStart);
            }
            if (capabilityLength > 0)
            {
                tmp_tvb = tvb_new_subset_length (tvb, offset,
                        capabilityLength);
                add_capabilities (wsp_tree, pinfo, tmp_tvb, pdut);
                offset += capabilityLength;
            }

            if (headerLength > 0)
            {
                tmp_tvb = tvb_new_subset_length (tvb, offset,
                        headerLength);
                add_headers (wsp_tree, tmp_tvb, hf_wsp_headers_section, pinfo);
            }

            break;

        case WSP_PDU_REDIRECT:
            dissect_redirect(tvb, offset, pinfo, wsp_tree, dissector_handle);
            break;

        case WSP_PDU_DISCONNECT:
        case WSP_PDU_SUSPEND:
            if (tree) {
                count = 0;  /
                value = tvb_get_guintvar (tvb, offset, &count);
                proto_tree_add_uint (wsp_tree,
                        hf_wsp_server_session_id,
                        tvb, offset, count, value);
                proto_item_append_text(proto_ti, ", Session ID: %u", value);
            }
            break;

        case WSP_PDU_GET:
        case WSP_PDU_OPTIONS:
        case WSP_PDU_HEAD:
        case WSP_PDU_DELETE:
        case WSP_PDU_TRACE:
            count = 0;  /
            /
            value = tvb_get_guintvar (tvb, offset, &count);
            nextOffset = offset + count;
            add_uri (wsp_tree, pinfo, tvb, offset, nextOffset, proto_ti);
            if (tree) {
                offset += value + count; /
                tmp_tvb = tvb_new_subset_remaining (tvb, offset);
                add_headers (wsp_tree, tmp_tvb, hf_wsp_headers_section, pinfo);
            }
            break;

        case WSP_PDU_POST:
        case WSP_PDU_PUT:
            uriStart = offset;
            count = 0;  /
            uriLength = tvb_get_guintvar (tvb, offset, &count);
            headerStart = uriStart+count;
            count = 0;  /
            headersLength = tvb_get_guintvar (tvb, headerStart, &count);
            offset = headerStart + count;

            add_uri (wsp_tree, pinfo, tvb, uriStart, offset, proto_ti);
            offset += uriLength;

            if (tree)
                proto_tree_add_uint (wsp_tree, hf_wsp_header_length,
                        tvb, headerStart, count, headersLength);

            /
            if (headersLength == 0)
                break;

            contentTypeStart = offset;
            nextOffset = add_content_type (wsp_tree, pinfo,
                    tvb, offset, &contentType, &contentTypeStr);

            /
            if (contentTypeStr) {
                proto_item_append_text(proto_ti, ", Content-Type: %s",
                        contentTypeStr);
            } else {
                proto_item_append_text(proto_ti, ", Content-Type: 0x%X",
                        contentType);
            }

            /
            /
            headerLength = headersLength - (nextOffset - contentTypeStart);
            if (headerLength > 0)
            {
                tmp_tvb = tvb_new_subset_length (tvb, nextOffset,
                        headerLength);
                add_headers (wsp_tree, tmp_tvb, hf_wsp_headers_section, pinfo);
            }
            /
            /

            /
            if (tvb_reported_length_remaining(tvb,
                        headerStart + count + uriLength + headersLength) > 0)
            {
                tmp_tvb = tvb_new_subset_remaining (tvb,
                        headerStart + count + uriLength + headersLength);
                /
                found_match = 0;
                if (contentTypeStr) {
                    /
                    found_match = dissector_try_string(media_type_table,
                            contentTypeStr, tmp_tvb, pinfo, tree, NULL);
                }
                if (! found_match) {
                    if (! dissector_try_heuristic(heur_subdissector_list,
                                tmp_tvb, pinfo, tree, &hdtbl_entry, NULL)) {

                        pinfo->match_string = contentTypeStr;
                        call_dissector_with_data(media_handle, tmp_tvb, pinfo, tree, NULL /);
#if 0
                        if (tree) /
                            add_post_data (wsp_tree, tmp_tvb,
                                    contentType, contentTypeStr, pinfo);
#endif
                    }
                }
            }
            break;

        case WSP_PDU_REPLY:
            count = 0;  /
            headersLength = tvb_get_guintvar (tvb, offset+1, &count);
            headerStart = offset + count + 1;
            {
                guint8 reply_status = tvb_get_guint8(tvb, offset);
                const char *reply_status_str;

                reply_status_str = val_to_str_ext_const (reply_status, &wsp_vals_status_ext, "(Unknown response status)");
                if (tree) {
                    proto_tree_add_item (wsp_tree, hf_wsp_header_status,
                            tvb, offset, 1, ENC_LITTLE_ENDIAN);
                    proto_item_append_text(proto_ti, ", Status: %s (0x%02x)",
                            reply_status_str, reply_status);
                }
                stat_info->status_code = (gint) reply_status;
                /
                col_append_fstr(pinfo->cinfo, COL_INFO,
                            ": %s (0x%02x)",
                            reply_status_str, reply_status);
            }
            nextOffset = offset + 1 + count;
            if (tree)
                proto_tree_add_uint (wsp_tree, hf_wsp_header_length,
                        tvb, offset + 1, count, headersLength);

            if (headersLength == 0)
                break;

            contentTypeStart = nextOffset;
            nextOffset = add_content_type (wsp_tree, pinfo, tvb,
                    nextOffset, &contentType, &contentTypeStr);

            /
            if (contentTypeStr) {
                proto_item_append_text(proto_ti, ", Content-Type: %s",
                        contentTypeStr);
            } else {
                proto_item_append_text(proto_ti, ", Content-Type: 0x%X",
                        contentType);
            }

            /
            /
            headerLength = headersLength - (nextOffset - contentTypeStart);
            if (headerLength > 0)
            {
                tmp_tvb = tvb_new_subset_length (tvb, nextOffset,
                        headerLength);
                add_headers (wsp_tree, tmp_tvb, hf_wsp_headers_section, pinfo);
            }
            /
            /

            /
            if (tvb_reported_length_remaining(tvb, headerStart + headersLength)
                    > 0)
            {
                tmp_tvb = tvb_new_subset_remaining (tvb, headerStart + headersLength);
                /
                found_match = 0;
                if (contentTypeStr) {
                    /
                    found_match = dissector_try_string(media_type_table,
                            contentTypeStr, tmp_tvb, pinfo, tree, NULL);
                }
                if (! found_match) {
                    if (! dissector_try_heuristic(heur_subdissector_list,
                                tmp_tvb, pinfo, tree, &hdtbl_entry, NULL)) {

                        pinfo->match_string = contentTypeStr;
                        call_dissector_with_data(media_handle, tmp_tvb, pinfo, tree, NULL /);
#if 0
                        if (tree) / * Only display if needed * /
                            proto_tree_add_item (wsp_tree,
                                hf_wsp_reply_data,
                                tmp_tvb, 0, -1, ENC_NA);
#endif
                    }
                }
            }
            break;

        case WSP_PDU_PUSH:
        case WSP_PDU_CONFIRMEDPUSH:
            count = 0;  /
            headersLength = tvb_get_guintvar (tvb, offset, &count);
            headerStart = offset + count;

            proto_tree_add_uint (wsp_tree, hf_wsp_header_length,
                        tvb, offset, count, headersLength);

            if (headersLength == 0)
                break;

            offset += count;
            contentTypeStart = offset;
            nextOffset = add_content_type (wsp_tree, pinfo,
                    tvb, offset, &contentType, &contentTypeStr);

            /
            if (contentTypeStr) {
                proto_item_append_text(proto_ti, ", Content-Type: %s",
                        contentTypeStr);
            } else {
                proto_item_append_text(proto_ti, ", Content-Type: 0x%X",
                        contentType);
            }

            /
            /
            headerLength = headersLength-(nextOffset-contentTypeStart);
            if (headerLength > 0)
            {
                tmp_tvb = tvb_new_subset_length (tvb, nextOffset,
                        headerLength);
                add_headers (wsp_tree, tmp_tvb, hf_wsp_headers_section, pinfo);
            }
            /
            /

            /
            if (tvb_reported_length_remaining(tvb, headerStart + headersLength)
                    > 0)
            {
                tmp_tvb = tvb_new_subset_remaining (tvb, headerStart + headersLength);
                /
                found_match = 0;
                if (contentTypeStr) {
                    /
                    /
                    found_match = dissector_try_string(media_type_table,
                            contentTypeStr, tmp_tvb, pinfo, tree, NULL);
                }
                if (! found_match) {
                    if (! dissector_try_heuristic(heur_subdissector_list,
                                tmp_tvb, pinfo, tree, &hdtbl_entry, NULL)) {

                        pinfo->match_string = contentTypeStr;
                        call_dissector_with_data(media_handle, tmp_tvb, pinfo, tree, NULL /);
#if 0
                        if (tree) /
                            proto_tree_add_item (wsp_tree,
                                    hf_wsp_push_data,
                                    tmp_tvb, 0, -1, ENC_NA);
#endif
                    }
                }
            }
            break;

    }
    stat_info->pdut = pdut;
    tap_queue_packet (wsp_tap, pinfo, stat_info);
}
