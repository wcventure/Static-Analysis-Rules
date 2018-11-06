static int
dissect_sip_common(tvbuff_t *tvb, int offset, int remaining_length, packet_info *pinfo, proto_tree *tree,
    gboolean dissect_other_as_continuation, gboolean use_reassembly)
{
    int orig_offset;
    gint next_offset, linelen;
    int content_length, datalen, reported_datalen;
    line_type_t line_type;
    tvbuff_t *next_tvb;
    gboolean is_known_request;
    int found_match = 0;
    const char *descr;
    guint token_1_len = 0;
    guint current_method_idx = SIP_METHOD_INVALID;
    proto_item *ts, *ti_a = NULL, *th = NULL;
    proto_tree *sip_tree, *reqresp_tree      = NULL, *hdr_tree  = NULL,
        *message_body_tree = NULL, *cseq_tree = NULL,
        *via_tree         = NULL, *reason_tree       = NULL, *rack_tree = NULL,
        *route_tree       = NULL, *security_client_tree = NULL, *session_id_tree = NULL,
        *p_access_net_info_tree = NULL;
    guchar contacts = 0, contact_is_star = 0, expires_is_0 = 0, contacts_expires_0 = 0, contacts_expires_unknown = 0;
    guint32 cseq_number = 0;
    guchar  cseq_number_set = 0;
    char    cseq_method[MAX_CSEQ_METHOD_SIZE] = "";
    char    call_id[MAX_CALL_ID_SIZE] = "";
    gchar  *media_type_str_lower_case = NULL;
    char   *content_type_parameter_str = NULL;
    char   *content_encoding_parameter_str = NULL;
    guint   resend_for_packet = 0;
    guint   request_for_response = 0;
    guint32 response_time = 0;
    int     strlen_to_copy;
    heur_dtbl_entry_t *hdtbl_entry;

    /

    if (!dissect_other_as_continuation &&
        ((remaining_length < 1) || !g_ascii_isprint(tvb_get_guint8(tvb, offset))))
    {
        return -2;
    }

    /
    orig_offset = offset;
    linelen = tvb_find_line_end(tvb, offset, remaining_length, &next_offset, FALSE);
    if(linelen==0){
        return -2;
    }

    if (tvb_strnlen(tvb, offset, linelen) > -1)
    {
        /
        return -2;
    }
    line_type = sip_parse_line(tvb, offset, linelen, &token_1_len);

    if (line_type == OTHER_LINE) {
        /
        if (!dissect_other_as_continuation) {
            /
            return -2;
        }

        /
    } else if ((use_reassembly)&&( pinfo->ptype == PT_TCP)) {
        /
        if (!req_resp_hdrs_do_reassembly(tvb, offset, pinfo,
            sip_desegment_headers, sip_desegment_body)) {
            /
            return -1;
        }
    }

    /
    stat_info = wmem_new0(wmem_packet_scope(), sip_info_value_t);

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "SIP");

    if (!pinfo->flags.in_error_pkt && have_tap_listener(exported_pdu_tap)) {
        export_sip_pdu(pinfo,tvb);
    }

    DPRINT2(("------------------------------ dissect_sip_common ------------------------------"));

    switch (line_type) {

    case REQUEST_LINE:
        is_known_request = sip_is_known_request(tvb, offset, token_1_len, &current_method_idx);
        descr = is_known_request ? "Request" : "Unknown request";
        col_add_lstr(pinfo->cinfo, COL_INFO,
                     descr, ": ",
                     tvb_format_text(tvb, offset, linelen - SIP2_HDR_LEN - 1),
                     COL_ADD_LSTR_TERMINATOR);
        DPRINT(("got %s: %s", descr,
                tvb_format_text(tvb, offset, linelen - SIP2_HDR_LEN - 1)));
        break;

    case STATUS_LINE:
        descr = "Status";
        col_add_lstr(pinfo->cinfo, COL_INFO,
                     "Status: ",
                     tvb_format_text(tvb, offset + SIP2_HDR_LEN + 1, linelen - SIP2_HDR_LEN - 1),
                     COL_ADD_LSTR_TERMINATOR);
        stat_info->reason_phrase = tvb_get_string_enc(wmem_packet_scope(), tvb, offset + SIP2_HDR_LEN + 5,
                                                      linelen - (SIP2_HDR_LEN + 5),ENC_UTF_8|ENC_NA);
        DPRINT(("got Response: %s",
                tvb_format_text(tvb, offset + SIP2_HDR_LEN + 1, linelen - SIP2_HDR_LEN - 1)));
        break;

    case OTHER_LINE:
    default: /
        descr = "Continuation";
        col_set_str(pinfo->cinfo, COL_INFO, "Continuation");
        DPRINT(("got continuation"));
        break;
    }

    ts = proto_tree_add_item(tree, proto_sip, tvb, offset, -1, ENC_NA);
    sip_tree = proto_item_add_subtree(ts, ett_sip);

    switch (line_type) {

    case REQUEST_LINE:
        if (sip_tree) {
            ti_a = proto_tree_add_item(sip_tree, hf_Request_Line, tvb,
                        offset, linelen, ENC_UTF_8|ENC_NA);

            reqresp_tree = proto_item_add_subtree(ti_a, ett_sip_reqresp);
        }
        dfilter_sip_request_line(tvb, reqresp_tree, pinfo, offset, token_1_len, linelen);
        break;

    case STATUS_LINE:
        if (sip_tree) {
            ti_a = proto_tree_add_item(sip_tree, hf_sip_Status_Line, tvb,
                        offset, linelen, ENC_UTF_8|ENC_NA);
            reqresp_tree = proto_item_add_subtree(ti_a, ett_sip_reqresp);
        }
        dfilter_sip_status_line(tvb, reqresp_tree, pinfo, linelen, offset);
        break;

    case OTHER_LINE:
        if (sip_tree) {
            reqresp_tree = proto_tree_add_subtree_format(sip_tree, tvb, offset, next_offset,
                                     ett_sip_reqresp, NULL, "%s line: %s", descr,
                                     tvb_format_text(tvb, offset, linelen));
            /
            proto_tree_add_item(reqresp_tree, hf_sip_continuation, tvb, offset, -1, ENC_NA);
        }
        return remaining_length;
    }

    remaining_length = remaining_length - (next_offset - offset);
    offset = next_offset;

    th = proto_tree_add_item(sip_tree, hf_sip_msg_hdr, tvb, offset,
                                 remaining_length, ENC_UTF_8|ENC_NA);
    proto_item_set_text(th, "Message Header");
    hdr_tree = proto_item_add_subtree(th, ett_sip_hdr);

    /
    next_offset = offset;
    content_length = -1;
    while (remaining_length > 0) {
        gint line_end_offset;
        gint colon_offset;
        gint semi_colon_offset;
        gint parameter_offset;
        gint parameter_end_offset;
        gint parameter_len;
        gint content_type_len, content_type_parameter_str_len;
        gint header_len;
        gchar *header_name;
        dissector_handle_t ext_hdr_handle;
        gint hf_index;
        gint value_offset;
        gint sub_value_offset;
        gint comma_offset;
        guchar c;
        gint value_len;
        gboolean is_no_header_termination = FALSE;
        proto_tree *tc_uri_item_tree = NULL;
        uri_offset_info uri_offsets;

        linelen = tvb_find_line_end(tvb, offset, -1, &next_offset, FALSE);
        if (linelen == 0) {
            /
            offset = next_offset;
            break;
        }

        line_end_offset = offset + linelen;
        if(tvb_reported_length_remaining(tvb, next_offset) <= 0){
            is_no_header_termination = TRUE;
        }else{
            while (tvb_offset_exists(tvb, next_offset) && ((c = tvb_get_guint8(tvb, next_offset)) == ' ' || c == '\t'))
            {
                /
                linelen += (next_offset - line_end_offset);
                linelen += tvb_find_line_end(tvb, next_offset, -1, &next_offset, FALSE);
                line_end_offset = offset + linelen;
            }
        }
        colon_offset = tvb_find_guint8(tvb, offset, linelen, ':');
        if (colon_offset == -1) {
            /
            expert_add_info(pinfo, th, &ei_sip_header_no_colon);
        } else {
            header_len = colon_offset - offset;
            header_name = (gchar*)tvb_get_string_enc(wmem_packet_scope(), tvb, offset, header_len, ENC_UTF_8|ENC_NA);
            ascii_strdown_inplace(header_name);
            hf_index = sip_is_known_sip_header(header_name, header_len);

            /
            value_offset = tvb_skip_wsp(tvb, colon_offset + 1, line_end_offset - (colon_offset + 1));

            value_len = (gint) (line_end_offset - value_offset);

            if (hf_index == -1) {
                gint *hf_ptr = (gint*)wmem_map_lookup(sip_custom_header_fields_hash, header_name);
                if (hf_ptr) {
                    sip_proto_tree_add_string(hdr_tree, *hf_ptr, tvb, offset,
                                              next_offset - offset, value_offset, value_len);
                } else {
                    proto_item *ti_c;
                    proto_tree *ti_tree = proto_tree_add_subtree(hdr_tree, tvb,
                                                         offset, next_offset - offset, ett_sip_ext_hdr, &ti_c,
                                                         tvb_format_text(tvb, offset, linelen));

                    ext_hdr_handle = dissector_get_string_handle(ext_hdr_subdissector_table, header_name);
                    if (ext_hdr_handle != NULL) {
                        tvbuff_t *next_tvb2;
                        next_tvb2 = tvb_new_subset_length(tvb, value_offset, value_len);
                        dissector_try_string(ext_hdr_subdissector_table, header_name, next_tvb2, pinfo, ti_tree, NULL);
                    } else {
                        expert_add_info_format(pinfo, ti_c, &ei_sip_unrecognized_header,
                                               "Unrecognised SIP header (%s)",
                                               header_name);
                    }
                }
            } else {
                proto_item *sip_element_item;
                proto_tree *sip_element_tree;

                /
                switch ( hf_index ) {

                    case POS_TO :

                        / {
                            proto_item *item;

                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            sip_element_tree = proto_item_add_subtree( sip_element_item,
                                               ett_sip_element);
                            /
                            sip_uri_offset_init(&uri_offsets);
                            if((dissect_sip_name_addr_or_addr_spec(tvb, pinfo, value_offset, line_end_offset+2, &uri_offsets)) != -1){
                                display_sip_uri(tvb, sip_element_tree, pinfo, &uri_offsets, &sip_to_uri);
                                if((uri_offsets.name_addr_start != -1) && (uri_offsets.name_addr_end != -1)){
                                    stat_info->tap_to_addr=tvb_get_string_enc(wmem_packet_scope(), tvb, uri_offsets.name_addr_start,
                                        uri_offsets.name_addr_end - uri_offsets.name_addr_start, ENC_UTF_8|ENC_NA);
                                }
                                offset = uri_offsets.name_addr_end +1;
                            }

                            /

                            parameter_offset = offset;
                            while (parameter_offset < line_end_offset
                                   && (tvb_strneql(tvb, parameter_offset, "tag=", 4) != 0))
                                parameter_offset++;

                            if ( parameter_offset < line_end_offset ){ /
                                parameter_offset = parameter_offset + 4;
                                parameter_end_offset = tvb_find_guint8(tvb, parameter_offset,
                                                                       (line_end_offset - parameter_offset), ';');
                                if ( parameter_end_offset == -1)
                                    parameter_end_offset = line_end_offset;
                                parameter_len = parameter_end_offset - parameter_offset;
                                proto_tree_add_item(sip_element_tree, hf_sip_to_tag, tvb, parameter_offset,
                                                    parameter_len, ENC_UTF_8|ENC_NA);
                                item = proto_tree_add_item(sip_element_tree, hf_sip_tag, tvb, parameter_offset,
                                                           parameter_len, ENC_UTF_8|ENC_NA);
                                PROTO_ITEM_SET_HIDDEN(item);

                                /
                                switch (current_method_idx) {

                                case SIP_METHOD_INVITE:
                                case SIP_METHOD_SUBSCRIBE:
                                case SIP_METHOD_REFER:
                                    col_append_str(pinfo->cinfo, COL_INFO, ", in-dialog");
                                    break;
                                }
                            }
                        } /
                    break;

                    case POS_FROM :
                        if(hdr_tree) {
                            proto_item *item;

                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            sip_element_tree = proto_item_add_subtree( sip_element_item, ett_sip_element);
                            /

                            sip_uri_offset_init(&uri_offsets);
                            if((dissect_sip_name_addr_or_addr_spec(tvb, pinfo, value_offset, line_end_offset+2, &uri_offsets)) != -1){
                                display_sip_uri(tvb, sip_element_tree, pinfo, &uri_offsets, &sip_from_uri);
                                if((uri_offsets.name_addr_start != -1) && (uri_offsets.name_addr_end != -1)){
                                    stat_info->tap_from_addr=tvb_get_string_enc(wmem_packet_scope(), tvb, uri_offsets.name_addr_start,
                                        uri_offsets.name_addr_end - uri_offsets.name_addr_start, ENC_UTF_8|ENC_NA);
                                }
                                offset = uri_offsets.name_addr_end +1;
                            }

                            /

                            parameter_offset = offset;
                            while (parameter_offset < line_end_offset
                                   && (tvb_strneql(tvb, parameter_offset, "tag=", 4) != 0))
                                parameter_offset++;
                            if ( parameter_offset < line_end_offset ){ /
                                parameter_offset = parameter_offset + 4;
                                parameter_end_offset = tvb_find_guint8(tvb, parameter_offset,
                                                                       (line_end_offset - parameter_offset), ';');
                                if ( parameter_end_offset == -1)
                                    parameter_end_offset = line_end_offset;
                                parameter_len = parameter_end_offset - parameter_offset;
                                proto_tree_add_item(sip_element_tree, hf_sip_from_tag, tvb, parameter_offset,
                                                    parameter_len, ENC_UTF_8|ENC_NA);
                                item = proto_tree_add_item(sip_element_tree, hf_sip_tag, tvb, parameter_offset,
                                                           parameter_len, ENC_UTF_8|ENC_NA);
                                PROTO_ITEM_SET_HIDDEN(item);
                            }
                        }/
                    break;

                    case POS_P_ASSERTED_IDENTITY :
                        if(hdr_tree)
                        {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            sip_element_tree = proto_item_add_subtree( sip_element_item,
                                               ett_sip_element);

                            /
                            sip_uri_offset_init(&uri_offsets);
                            if((dissect_sip_name_addr_or_addr_spec(tvb, pinfo, value_offset, line_end_offset+2, &uri_offsets)) != -1)
                                 display_sip_uri(tvb, sip_element_tree, pinfo, &uri_offsets, &sip_pai_uri);
                        }
                        break;
                    case POS_HISTORY_INFO:
                        if(hdr_tree)
                        {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            sip_element_tree = proto_item_add_subtree( sip_element_item,
                                               ett_sip_hist);
                            dissect_sip_history_info(tvb, sip_element_tree, pinfo, value_offset, line_end_offset);
                        }
                        break;

                    case POS_P_CHARGING_FUNC_ADDRESSES:
                        if(hdr_tree)
                        {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            sip_element_tree = proto_item_add_subtree( sip_element_item,
                                               ett_sip_element);
                            dissect_sip_p_charging_func_addresses(tvb, sip_element_tree, pinfo, value_offset, line_end_offset);
                        }
                        break;

                    case POS_P_PREFERRED_IDENTITY :
                        if(hdr_tree)
                        {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            sip_element_tree = proto_item_add_subtree( sip_element_item,
                                               ett_sip_element);
                            /
                            sip_uri_offset_init(&uri_offsets);
                            if((dissect_sip_name_addr_or_addr_spec(tvb, pinfo, value_offset, line_end_offset+2, &uri_offsets)) != -1)
                                 display_sip_uri(tvb, sip_element_tree, pinfo, &uri_offsets, &sip_ppi_uri);
                        }
                        break;

                    case POS_PERMISSION_MISSING :
                        if(hdr_tree)
                        {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            sip_element_tree = proto_item_add_subtree( sip_element_item,
                                                                   ett_sip_element);
                            /
                            sip_uri_offset_init(&uri_offsets);
                            if((dissect_sip_name_addr_or_addr_spec(tvb, pinfo, value_offset, line_end_offset+2, &uri_offsets)) != -1)
                                 display_sip_uri(tvb, sip_element_tree, pinfo, &uri_offsets, &sip_pmiss_uri);
                        }
                        break;


                    case POS_TRIGGER_CONSENT :
                        if(hdr_tree)
                        {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            sip_element_tree = proto_item_add_subtree( sip_element_item,
                                                                        ett_sip_element);
                            /
                            sip_uri_offset_init(&uri_offsets);
                            if((dissect_sip_uri(tvb, pinfo, value_offset, line_end_offset+2, &uri_offsets)) != -1) {

                                tc_uri_item_tree = display_sip_uri(tvb, sip_element_tree, pinfo, &uri_offsets, &sip_tc_uri);
                                if (line_end_offset > uri_offsets.uri_end) {
                                    gint hparam_offset = uri_offsets.uri_end + 1;
                                    /
                                    if (tvb_find_guint8(tvb, hparam_offset, 1,';')) {
                                        while ((hparam_offset != -1 && hparam_offset < line_end_offset) )  {
                                            /
                                            hparam_offset = hparam_offset + 1;
                                            if (tvb_strncaseeql(tvb, hparam_offset, "target-uri=\"", 12) == 0) {
                                                gint turi_start_offset = hparam_offset + 12;
                                                gint turi_end_offset   = tvb_find_guint8(tvb, turi_start_offset, -1,'\"');
                                                if (turi_end_offset != -1)
                                                    proto_tree_add_item(tc_uri_item_tree, hf_sip_tc_turi, tvb, turi_start_offset,(turi_end_offset - turi_start_offset),ENC_UTF_8|ENC_NA);
                                                else
                                                    break; /
                                            }
                                            hparam_offset = tvb_find_guint8(tvb, hparam_offset, -1,';');
                                        }
                                    }
                                }
                            }
                        }/
                        break;

                    case POS_CSEQ :
                    {
                        /
                        char *value = tvb_get_string_enc(wmem_packet_scope(), tvb, value_offset, value_len, ENC_UTF_8|ENC_NA);

                        cseq_number = (guint32)strtoul(value, NULL, 10);
                        cseq_number_set = 1;
                        stat_info->tap_cseq_number=cseq_number;

                        /
                        if (hdr_tree) {
                            sip_element_item = proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            cseq_tree = proto_item_add_subtree(sip_element_item, ett_sip_cseq);
                        }

                        /
                        for (sub_value_offset=0; sub_value_offset < value_len; sub_value_offset++)
                        {
                            if (!g_ascii_isdigit(value[sub_value_offset]))
                            {
                                proto_tree_add_uint(cseq_tree, hf_sip_cseq_seq_no,
                                                    tvb, value_offset, sub_value_offset,
                                                    cseq_number);
                                break;
                            }
                        }

                        for (; sub_value_offset < value_len; sub_value_offset++)
                        {
                            if (g_ascii_isalpha(value[sub_value_offset]))
                            {
                                /
                                break;
                            }
                        }

                        if (sub_value_offset == value_len)
                        {
                            /
                            return offset - orig_offset;
                        }

                        /
                        strlen_to_copy = (int)value_len-sub_value_offset;
                        if (strlen_to_copy > MAX_CSEQ_METHOD_SIZE) {
                            /
                            if (hdr_tree) {
                                proto_tree_add_string_format(hdr_tree,
                                                             hf_header_array[hf_index], tvb,
                                                             offset, next_offset - offset,
                                                             value+sub_value_offset, "%s String too big: %d bytes",
                                                             sip_headers[POS_CSEQ].name,
                                                             strlen_to_copy);
                            }
                            return offset - orig_offset;
                        }
                        else {
                            g_strlcpy(cseq_method, value+sub_value_offset, MAX_CSEQ_METHOD_SIZE);

                            /
                            if (cseq_tree)
                            {
                                proto_tree_add_item(cseq_tree, hf_sip_cseq_method, tvb,
                                                    value_offset + sub_value_offset, strlen_to_copy, ENC_UTF_8|ENC_NA);
                            }
                        }
                    }
                    break;

                    case POS_RACK :
                    {
                        char *value = tvb_get_string_enc(wmem_packet_scope(), tvb, value_offset, value_len, ENC_UTF_8|ENC_NA);
                        int cseq_no_offset;
                        /

                        /
                        if (hdr_tree) {
                            sip_element_item = proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            rack_tree = proto_item_add_subtree(sip_element_item, ett_sip_rack);
                        }

                        /
                        for (sub_value_offset=0; sub_value_offset < value_len; sub_value_offset++)
                        {
                            if (!g_ascii_isdigit(value[sub_value_offset]))
                            {
                                proto_tree_add_uint(rack_tree, hf_sip_rack_rseq_no,
                                                    tvb, value_offset, sub_value_offset,
                                                    (guint32)strtoul(value, NULL, 10));
                                break;
                            }
                        }

                        /
                        for ( ; sub_value_offset < value_len; sub_value_offset++)
                        {
                            if (value[sub_value_offset] != ' ' &&
                                value[sub_value_offset] != '\t')
                            {
                                break;
                            }
                        }
                        cseq_no_offset = sub_value_offset;

                        /
                        for ( ; sub_value_offset < value_len; sub_value_offset++)
                        {
                            if (!g_ascii_isdigit(value[sub_value_offset]))
                            {
                                proto_tree_add_uint(rack_tree, hf_sip_rack_cseq_no,
                                                    tvb, value_offset+cseq_no_offset,
                                                    sub_value_offset-cseq_no_offset,
                                                    (guint32)strtoul(value+cseq_no_offset, NULL, 10));
                                break;
                            }
                        }

                        /
                        for ( ; sub_value_offset < value_len; sub_value_offset++)
                        {
                            if (g_ascii_isalpha(value[sub_value_offset]))
                            {
                                /
                                break;
                            }
                        }
                        /

                        if (sub_value_offset == linelen)
                        {
                            /
                            return offset - orig_offset;
                        }

                        /
                        if (cseq_tree)
                        {
                            proto_tree_add_item(rack_tree, hf_sip_rack_cseq_method, tvb,
                                                value_offset + sub_value_offset,
                                                (int)value_len-sub_value_offset, ENC_UTF_8|ENC_NA);
                        }

                        break;
                    }

                    case POS_CALL_ID :
                    {
                        char *value = tvb_get_string_enc(wmem_packet_scope(), tvb, value_offset, value_len, ENC_UTF_8|ENC_NA);

                        /
                        g_strlcpy(call_id, value, MAX_CALL_ID_SIZE);
                        stat_info->tap_call_id = wmem_strdup(wmem_packet_scope(), call_id);

                        /
                        sip_element_item = proto_tree_add_string(hdr_tree,
                                                    hf_header_array[hf_index], tvb,
                                                    offset, next_offset - offset,
                                                    value);
                        sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);
                    }
                    break;

                    case POS_EXPIRES :
                        if (tvb_strneql(tvb, value_offset, "0", value_len) == 0)
                        {
                            expires_is_0 = 1;
                        }

                        /
                        sip_proto_tree_add_uint(hdr_tree,
                                                hf_header_array[hf_index], tvb,
                                                offset, next_offset - offset,
                                                value_offset, value_len);
                    break;

                    /
                    case POS_CONTENT_TYPE :
                        sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                        sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                        content_type_len = value_len;
                        semi_colon_offset = tvb_find_guint8(tvb, value_offset, value_len, ';');
                        /
                        if ( semi_colon_offset != -1) {
                            gint content_type_end;
                            /
                            parameter_offset = tvb_skip_wsp(tvb, semi_colon_offset +1, value_offset + value_len - (semi_colon_offset +1));
                            content_type_end = tvb_skip_wsp_return(tvb, semi_colon_offset-1);
                            content_type_len = content_type_end - value_offset;
                            content_type_parameter_str_len = value_offset + value_len - parameter_offset;
                            content_type_parameter_str = tvb_get_string_enc(wmem_packet_scope(), tvb, parameter_offset,
                                                         content_type_parameter_str_len, ENC_UTF_8|ENC_NA);
                        }
                        media_type_str_lower_case = ascii_strdown_inplace(
                            (gchar *)tvb_get_string_enc(wmem_packet_scope(), tvb, value_offset, content_type_len, ENC_UTF_8|ENC_NA));

                        /
                    break;

                    case POS_CONTENT_LENGTH :
                    {
                        char *value = tvb_get_string_enc(wmem_packet_scope(), tvb, value_offset, value_len, ENC_UTF_8|ENC_NA);
                        content_length = atoi(value);

                        sip_element_item = proto_tree_add_uint(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               content_length);
                        sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                        break;
                    }

                    case POS_MAX_BREADTH :
                    case POS_MAX_FORWARDS :
                    case POS_RSEQ :
                        sip_proto_tree_add_uint(hdr_tree,
                                                hf_header_array[hf_index], tvb,
                                                offset, next_offset - offset,
                                                value_offset, value_len);
                        break;

                    case POS_CONTACT :
                        /
                        sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               value_offset, value_len);
                        sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                        sip_element_tree = proto_item_add_subtree( sip_element_item,
                                               ett_sip_element);

                        /
                        c = tvb_get_guint8(tvb, value_offset);
                        if (c =='*'){
                            contact_is_star = 1;
                            break;
                        }

                        / {
                            comma_offset = value_offset;
                            while((comma_offset = dissect_sip_contact_item(tvb, pinfo, sip_element_tree, comma_offset,
                                    next_offset, &contacts_expires_0, &contacts_expires_unknown)) != -1)
                            {
                                contacts++;
                                if(comma_offset == next_offset)
                                {
                                    /
                                    break;
                                }

                                if(tvb_get_guint8(tvb, comma_offset) != ',')
                                {
                                    /
                                    break;
                                }
                                comma_offset++; /
                            }
                        }
                    break;

                    case POS_AUTHORIZATION:
                        /
                    case POS_WWW_AUTHENTICATE:
                        /
                    case POS_PROXY_AUTHENTICATE:
                        /
                    case POS_PROXY_AUTHORIZATION:
                        /
                    case POS_AUTHENTICATION_INFO:
                        /
                        /
                        if (hdr_tree) {
                            proto_item *ti_c;
                            /
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                               hf_header_array[hf_index], tvb,
                                               offset, next_offset - offset,
                                               value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            sip_element_tree = proto_item_add_subtree( sip_element_item,
                                               ett_sip_element);

                            /
                            ti_c = proto_tree_add_item(hdr_tree, hf_sip_auth, tvb,
                                                     offset, next_offset-offset,
                                                     ENC_UTF_8|ENC_NA);
                            PROTO_ITEM_SET_HIDDEN(ti_c);

                            /
                            if (hf_index != POS_AUTHENTICATION_INFO)
                            {
                                /
                                comma_offset = tvb_ws_mempbrk_pattern_guint8(tvb, value_offset, line_end_offset - value_offset, &pbrk_whitespace, NULL);
                                proto_tree_add_item(sip_element_tree, hf_sip_auth_scheme,
                                                    tvb, value_offset, comma_offset - value_offset,
                                                    ENC_UTF_8|ENC_NA);
                            }else{
                                /
                                comma_offset = value_offset;
                            }

                            /
                            while ((comma_offset = dissect_sip_authorization_item(tvb, sip_element_tree, comma_offset, line_end_offset)) != -1)
                            {
                                if(comma_offset == line_end_offset)
                                {
                                    /
                                    break;
                                }

                                if(tvb_get_guint8(tvb, comma_offset) != ',')
                                {
                                    /
                                    break;
                                }
                                comma_offset++; /
                            }
                        }/
                    break;

                    case POS_ROUTE:
                        /
                        if (hdr_tree) {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            route_tree = proto_item_add_subtree(sip_element_item, ett_sip_route);
                            dissect_sip_route_header(tvb, route_tree, pinfo, &sip_route_uri, value_offset, line_end_offset);
                        }
                        break;
                    case POS_RECORD_ROUTE:
                        /
                        if (hdr_tree) {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            route_tree = proto_item_add_subtree(sip_element_item, ett_sip_route);
                            dissect_sip_route_header(tvb, route_tree, pinfo, &sip_record_route_uri, value_offset, line_end_offset);
                        }
                        break;
                    case POS_SERVICE_ROUTE:
                        /
                        if (hdr_tree) {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            route_tree = proto_item_add_subtree(sip_element_item, ett_sip_route);
                            dissect_sip_route_header(tvb, route_tree, pinfo, &sip_service_route_uri, value_offset, line_end_offset);
                        }
                        break;
                    case POS_PATH:
                        /
                        if (hdr_tree) {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            route_tree = proto_item_add_subtree(sip_element_item, ett_sip_route);
                            dissect_sip_route_header(tvb, route_tree, pinfo, &sip_path_uri, value_offset, line_end_offset);
                        }
                        break;
                    case POS_VIA:
                        /
                        if (hdr_tree) {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            via_tree = proto_item_add_subtree(sip_element_item, ett_sip_via);
                            dissect_sip_via_header(tvb, via_tree, value_offset, line_end_offset);
                        }
                        break;
                    case POS_REASON:
                        if(hdr_tree) {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                            reason_tree = proto_item_add_subtree(sip_element_item, ett_sip_reason);
                            dissect_sip_reason_header(tvb, reason_tree, value_offset, line_end_offset);
                        }
                        break;
                    case POS_CONTENT_ENCODING:
                        /
                        sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                        sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                        content_encoding_parameter_str = ascii_strdown_inplace(tvb_get_string_enc(wmem_packet_scope(), tvb, value_offset,
                                                         (line_end_offset-value_offset), ENC_UTF_8|ENC_NA));
                        break;
                    case POS_SECURITY_CLIENT:
                        /
                        sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                        sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                        comma_offset = tvb_find_guint8(tvb, value_offset, line_end_offset - value_offset, ',');
                        while(comma_offset<line_end_offset){
                            comma_offset = tvb_find_guint8(tvb, value_offset, line_end_offset - value_offset, ',');
                            if(comma_offset == -1){
                                comma_offset = line_end_offset;
                            }
                            security_client_tree = proto_item_add_subtree(sip_element_item, ett_sip_security_client);
                            dissect_sip_sec_mechanism(tvb, pinfo, security_client_tree, value_offset, comma_offset);
                            comma_offset = value_offset = comma_offset+1;
                        }

                        break;
                    case POS_SECURITY_SERVER:
                        /
                        sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                        sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                        comma_offset = tvb_find_guint8(tvb, value_offset, line_end_offset - value_offset, ',');
                        while(comma_offset<line_end_offset){
                            comma_offset = tvb_find_guint8(tvb, value_offset, line_end_offset - value_offset, ',');
                            if(comma_offset == -1){
                                comma_offset = line_end_offset;
                            }
                            security_client_tree = proto_item_add_subtree(sip_element_item, ett_sip_security_server);
                            dissect_sip_sec_mechanism(tvb, pinfo, security_client_tree, value_offset, comma_offset);
                            comma_offset = value_offset = comma_offset+1;
                        }

                        break;
                    case POS_SECURITY_VERIFY:
                        /
                        sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                        sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);

                        comma_offset = tvb_find_guint8(tvb, value_offset, line_end_offset - value_offset, ',');
                        while(comma_offset<line_end_offset){
                            comma_offset = tvb_find_guint8(tvb, value_offset, line_end_offset - value_offset, ',');
                            if(comma_offset == -1){
                                comma_offset = line_end_offset;
                            }
                            security_client_tree = proto_item_add_subtree(sip_element_item, ett_sip_security_verify);
                            dissect_sip_sec_mechanism(tvb, pinfo, security_client_tree, value_offset, comma_offset);
                            comma_offset = value_offset = comma_offset+1;
                        }

                        break;
                    case POS_SESSION_ID:
                        if(hdr_tree) {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                            hf_header_array[hf_index], tvb,
                                                            offset, next_offset - offset,
                                                            value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);
                            session_id_tree = proto_item_add_subtree(sip_element_item, ett_sip_session_id);
                            dissect_sip_session_id_header(tvb, session_id_tree, value_offset, line_end_offset);
                        }
                        break;
                    case POS_P_ACCESS_NETWORK_INFO:
                        /
                        if (hdr_tree) {
                            sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                hf_header_array[hf_index], tvb,
                                offset, next_offset - offset,
                                value_offset, value_len);
                            sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);
                            p_access_net_info_tree = proto_item_add_subtree(sip_element_item, ett_sip_p_access_net_info);
                            dissect_sip_p_access_network_info_header(tvb, p_access_net_info_tree, value_offset, line_end_offset);
                        }
                        break;
                    default :
                        /
                        sip_element_item = sip_proto_tree_add_string(hdr_tree,
                                                         hf_header_array[hf_index], tvb,
                                                         offset, next_offset - offset,
                                                         value_offset, value_len);
                        sip_proto_set_format_text(hdr_tree, sip_element_item, tvb, offset, linelen);
                        break;
                }/
            }/
        }/
        if (is_no_header_termination == TRUE){
            /
            proto_tree_add_expert(hdr_tree, pinfo, &ei_sip_header_not_terminated,
                                    tvb, line_end_offset, -1);
        }
        remaining_length = remaining_length - (next_offset - offset);
        offset = next_offset;
    }/

    datalen = tvb_captured_length_remaining(tvb, offset);
    reported_datalen = tvb_reported_length_remaining(tvb, offset);
    if (content_length != -1) {
        if (datalen > content_length)
            datalen = content_length;
        if (reported_datalen > content_length)
            reported_datalen = content_length;
    }

    /

    /
    if (expires_is_0) {
        /
        contacts_expires_0 += contacts_expires_unknown;
    }

    /
    if (current_method_idx == SIP_METHOD_REGISTER)
    {
        /
        if (contact_is_star && expires_is_0)
        {
            col_append_str(pinfo->cinfo, COL_INFO, "  (remove all bindings)");
        }
        else
        if (contacts_expires_0 > 0)
        {
            col_append_fstr(pinfo->cinfo, COL_INFO, "  (remove %d binding%s)",
                contacts_expires_0, contacts_expires_0 == 1 ? "":"s");
            if (contacts > contacts_expires_0) {
                col_append_fstr(pinfo->cinfo, COL_INFO, " (add %d binding%s)",
                    contacts - contacts_expires_0,
                    (contacts - contacts_expires_0 == 1) ? "":"s");
            }
        }
        else
        if (!contacts)
        {
            col_append_str(pinfo->cinfo, COL_INFO, "  (fetch bindings)");
        }
        else
        {
            col_append_fstr(pinfo->cinfo, COL_INFO, "  (%d binding%s)",
                contacts, contacts == 1 ? "":"s");
        }
    }

    /
    if (line_type == STATUS_LINE && (strcmp(cseq_method, "REGISTER") == 0) &&
        stat_info && stat_info->response_code > 199 && stat_info->response_code < 300)
    {
        if (contacts_expires_0 > 0) {
            col_append_fstr(pinfo->cinfo, COL_INFO, "  (removed %d binding%s)",
                contacts_expires_0, contacts_expires_0 == 1 ? "":"s");
            if (contacts > contacts_expires_0) {
                col_append_fstr(pinfo->cinfo, COL_INFO, " (%d binding%s kept)",
                    contacts - contacts_expires_0,
                    (contacts - contacts_expires_0 == 1) ? "":"s");
            }
        } else {
            col_append_fstr(pinfo->cinfo, COL_INFO, "  (%d binding%s)",
                contacts, contacts == 1 ? "":"s");
        }
    }

    /
    /
    col_append_str(pinfo->cinfo, COL_INFO, " | ");
    col_set_fence(pinfo->cinfo, COL_INFO);

    /
    if ((line_type == REQUEST_LINE)&&(strcmp(cseq_method, "ACK") == 0))
    {
        request_for_response = sip_find_invite(pinfo, cseq_method, call_id,
                                                cseq_number_set, cseq_number,
                                                &response_time);
        stat_info->setup_time = response_time;
    }

    /
    resend_for_packet = sip_is_packet_resend(pinfo, cseq_method, call_id,
                                             cseq_number_set, cseq_number,
                                             line_type);
    /
    stat_info->resend = (resend_for_packet > 0);

    /
    if (line_type == STATUS_LINE)
    {
        request_for_response = sip_find_request(pinfo, cseq_method, call_id,
                                                cseq_number_set, cseq_number,
                                                &response_time);
    }

    /
    if (!pinfo->flags.in_error_pkt)
    {
        tap_queue_packet(sip_tap, pinfo, stat_info);
    }

    if (datalen > 0) {
        /
        proto_item_set_end(th, tvb, offset);
        if(content_encoding_parameter_str != NULL &&
            (!strncmp(content_encoding_parameter_str, "gzip", 4) ||
             !strncmp(content_encoding_parameter_str,"deflate",7))){
            /
            next_tvb = tvb_uncompress(tvb, offset,  datalen);
            if (next_tvb) {
                add_new_data_source(pinfo, next_tvb, "gunziped data");
                if(sip_tree) {
                    ti_a = proto_tree_add_item(sip_tree, hf_sip_msg_body, next_tvb, 0, -1,
                                         ENC_NA);
                    message_body_tree = proto_item_add_subtree(ti_a, ett_sip_message_body);
                }
            } else {
                next_tvb = tvb_new_subset(tvb, offset, datalen, reported_datalen);
                if(sip_tree) {
                    ti_a = proto_tree_add_item(sip_tree, hf_sip_msg_body, next_tvb, 0, -1,
                                         ENC_NA);
                    message_body_tree = proto_item_add_subtree(ti_a, ett_sip_message_body);
                }
            }
        }else{
            next_tvb = tvb_new_subset(tvb, offset, datalen, reported_datalen);
            if(sip_tree) {
                ti_a = proto_tree_add_item(sip_tree, hf_sip_msg_body, next_tvb, 0, -1,
                                     ENC_NA);
                message_body_tree = proto_item_add_subtree(ti_a, ett_sip_message_body);
            }
        }

        /
        if ( media_type_str_lower_case != NULL ) {
            /
            if (!strcmp(media_type_str_lower_case, "application/sdp")) {
                /
                if (resend_for_packet == 0) {
                    if (line_type == REQUEST_LINE) {
                        DPRINT(("calling setup_sdp_transport() SDP_EXCHANGE_OFFER frame=%d",
                                pinfo->num));
                        DINDENT();
                        setup_sdp_transport(next_tvb, pinfo, SDP_EXCHANGE_OFFER, pinfo->num, sip_delay_sdp_changes);
                        DENDENT();
                    } else if (line_type == STATUS_LINE) {
                        if (stat_info->response_code >= 400) {
                            DPRINT(("calling setup_sdp_transport() SDP_EXCHANGE_ANSWER_REJECT "
                                    "request_frame=%d, this=%d",
                                    request_for_response, pinfo->num));
                            DINDENT();
                            /
                            setup_sdp_transport(next_tvb, pinfo, SDP_EXCHANGE_ANSWER_REJECT, request_for_response, sip_delay_sdp_changes);
                            DENDENT();
                        }
                        else if ((stat_info->response_code >= 200) && (stat_info->response_code <= 299)) {
                            DPRINT(("calling setup_sdp_transport() SDP_EXCHANGE_ANSWER_ACCEPT "
                                    "request_frame=%d, this=%d",
                                    request_for_response, pinfo->num));
                            DINDENT();
                            /
                            setup_sdp_transport(next_tvb, pinfo, SDP_EXCHANGE_ANSWER_ACCEPT, request_for_response, sip_delay_sdp_changes);
                            DENDENT();
                        }
                    }
                } else {
                    DPRINT(("calling setup_sdp_transport() resend_for_packet "
                            "request_frame=%d, this=%d",
                            request_for_response, pinfo->num));
                    DINDENT();
                    setup_sdp_transport_resend(pinfo->num, resend_for_packet);
                    DENDENT();
                }
            }

            /
            DPRINT(("calling dissector_try_string()"));
            DINDENT();
            found_match = dissector_try_string(media_type_dissector_table,
                                               media_type_str_lower_case,
                                               next_tvb, pinfo,
                                               message_body_tree, content_type_parameter_str);
            DENDENT();
            DPRINT(("done calling dissector_try_string() with found_match=%u", found_match));

            if (!found_match &&
                !strncmp(media_type_str_lower_case, "multipart/", sizeof("multipart/")-1)) {
                DPRINT(("calling dissector_try_string() for multipart"));
                DINDENT();
                /
                found_match = dissector_try_string(media_type_dissector_table,
                                                   "multipart/",
                                                   next_tvb, pinfo,
                                                   message_body_tree, content_type_parameter_str);
                DENDENT();
                DPRINT(("done calling dissector_try_string() with found_match=%u", found_match));
            }
            /
        }
        if ( found_match == 0 )
        {
            DPRINT(("calling dissector_try_heuristic() with found_match=0"));
            DINDENT();
            if (!(dissector_try_heuristic(heur_subdissector_list,
                              next_tvb, pinfo, message_body_tree, &hdtbl_entry, NULL))) {
                int tmp_offset = 0;
                while (tvb_offset_exists(next_tvb, tmp_offset)) {
                    tvb_find_line_end(next_tvb, tmp_offset, -1, &next_offset, FALSE);
                    linelen = next_offset - tmp_offset;
                    proto_tree_add_format_text(message_body_tree, next_tvb,
                                tmp_offset, linelen);
                    tmp_offset = next_offset;
                }/
            }
            DENDENT();
        }
        offset += datalen;
    }

    /
    if (reqresp_tree)
    {
        proto_item *item;
        item = proto_tree_add_boolean(reqresp_tree, hf_sip_resend, tvb, orig_offset, 0,
                                      resend_for_packet > 0);
        PROTO_ITEM_SET_GENERATED(item);
        if (resend_for_packet > 0)
        {
            item = proto_tree_add_uint(reqresp_tree, hf_sip_original_frame,
                                       tvb, orig_offset, 0, resend_for_packet);
            PROTO_ITEM_SET_GENERATED(item);
        }

        if (request_for_response > 0)
        {
            item = proto_tree_add_uint(reqresp_tree, hf_sip_matching_request_frame,
                                       tvb, orig_offset, 0, request_for_response);
            PROTO_ITEM_SET_GENERATED(item);
            item = proto_tree_add_uint(reqresp_tree, hf_sip_response_time,
                                       tvb, orig_offset, 0, response_time);
            PROTO_ITEM_SET_GENERATED(item);
            if ((line_type == STATUS_LINE)&&(strcmp(cseq_method, "BYE") == 0)){
                item = proto_tree_add_uint(reqresp_tree, hf_sip_release_time,
                                          tvb, orig_offset, 0, response_time);
                PROTO_ITEM_SET_GENERATED(item);
            }
        }
    }

    if (ts != NULL)
        proto_item_set_len(ts, offset - orig_offset);

    if (global_sip_raw_text)
        tvb_raw_text_add(tvb, orig_offset, offset - orig_offset, tree);

    /
    if (stat_info->request_method) {
        proto_item_append_text(ts, " (%s)", stat_info->request_method);
    }
    else {
        proto_item_append_text(ts, " (%u)", stat_info->response_code);
    }
    return offset - orig_offset;
}
