static int
dissect_msrp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{
    gint offset = 0;
    gint next_offset = 0;
    proto_item *ti, *th, *msrp_headers_item;
    proto_tree *msrp_tree, *reqresp_tree, *raw_tree, *msrp_hdr_tree, *msrp_end_tree;
    proto_tree *msrp_data_tree;
    gint linelen;
    gint space_offset;
    gint token_2_start;
    guint token_2_len;
    gint token_3_start;
    guint token_3_len;
    gint token_4_start = 0;
    guint token_4_len = 0;
    gboolean is_msrp_response;
    gint end_line_offset;
    gint end_line_len;
    gint line_end_offset;
    gint message_end_offset;
    gint colon_offset;
    gint header_len;
    gint hf_index;
    gint value_offset;
    guchar c;
    gint value_len;
    char *value;
    gboolean have_body = FALSE;
    int found_match = 0;
    gint content_type_len, content_type_parameter_str_len;
    gchar *media_type_str_lower_case = NULL;
    char *content_type_parameter_str = NULL;
    tvbuff_t *next_tvb;
    gint parameter_offset;
    gint semi_colon_offset;
    gchar* hdr_str;

    if ( !check_msrp_header(tvb)){
        return 0;
    }
    /
    linelen = tvb_find_line_end(tvb, 0, -1, &next_offset, FALSE);

    /
    token_2_start = tvb_find_guint8(tvb, 0, linelen, ' ') + 1;

    /
    space_offset = tvb_find_guint8(tvb, token_2_start, linelen-token_2_start, ' ');
    token_2_len = space_offset - token_2_start;

    /
    token_3_start = space_offset + 1;
    space_offset = tvb_find_guint8(tvb, token_3_start,linelen-token_3_start, ' ');
    if ( space_offset == -1){
        /
        token_3_len = linelen - token_3_start;
    }else{
        /
        token_3_len = space_offset - token_3_start;
        token_4_start = space_offset + 1;
        token_4_len = linelen - token_4_start;
    }

    /
    is_msrp_response = FALSE;
    if (token_3_len == 3) {
            if (g_ascii_isdigit(tvb_get_guint8(tvb, token_3_start)) &&
                g_ascii_isdigit(tvb_get_guint8(tvb, token_3_start + 1)) &&
                g_ascii_isdigit(tvb_get_guint8(tvb, token_3_start + 2))) {
                is_msrp_response = TRUE;
            }
    }

    /
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "MSRP");
    if (is_msrp_response){
        col_add_fstr(pinfo->cinfo, COL_INFO, "Response: %s ",
                tvb_format_text(tvb, token_3_start, token_3_len));

        if (token_4_len )
            col_append_fstr(pinfo->cinfo, COL_INFO, "%s ",
                tvb_format_text(tvb, token_4_start, token_4_len));

        col_append_fstr(pinfo->cinfo, COL_INFO, "Transaction ID: %s",
                tvb_format_text(tvb, token_2_start, token_2_len));
    }else{
        col_add_fstr(pinfo->cinfo, COL_INFO, "Request: %s ",
                tvb_format_text(tvb, token_3_start, token_3_len));

        col_append_fstr(pinfo->cinfo, COL_INFO, "Transaction ID: %s",
                tvb_format_text(tvb, token_2_start, token_2_len));
    }

    /

    offset = next_offset;
    end_line_offset = find_end_line(tvb,offset);
    /
    end_line_len =  tvb_find_line_end(tvb, end_line_offset, -1, &next_offset, FALSE);
    message_end_offset = end_line_offset + end_line_len + 2;


    if (tree) {
        ti = proto_tree_add_item(tree, proto_msrp, tvb, 0, message_end_offset, ENC_NA);
        msrp_tree = proto_item_add_subtree(ti, ett_msrp);

        if (is_msrp_response){
            th = proto_tree_add_item(msrp_tree,hf_msrp_response_line,tvb,0,linelen,ENC_UTF_8|ENC_NA);
            reqresp_tree = proto_item_add_subtree(th, ett_msrp_reqresp);
            proto_tree_add_item(reqresp_tree,hf_msrp_transactionID,tvb,token_2_start,token_2_len,ENC_UTF_8|ENC_NA);
            proto_tree_add_uint(reqresp_tree,hf_msrp_status_code,tvb,token_3_start,token_3_len,
                                atoi(tvb_get_string_enc(wmem_packet_scope(), tvb, token_3_start, token_3_len, ENC_UTF_8|ENC_NA)));

        }else{
            th = proto_tree_add_item(msrp_tree,hf_msrp_request_line,tvb,0,linelen,ENC_UTF_8|ENC_NA);
            reqresp_tree = proto_item_add_subtree(th, ett_msrp_reqresp);
            proto_tree_add_item(reqresp_tree,hf_msrp_transactionID,tvb,token_2_start,token_2_len,ENC_UTF_8|ENC_NA);
            proto_tree_add_item(reqresp_tree,hf_msrp_method,tvb,token_3_start,token_3_len,ENC_UTF_8|ENC_NA);
        }

        /
        if (global_msrp_show_setup_info)
        {
            show_setup_info(tvb, pinfo, msrp_tree);
        }

        /
        msrp_headers_item = proto_tree_add_item(msrp_tree, hf_msrp_msg_hdr, tvb, offset,(end_line_offset - offset), ENC_NA);
        msrp_hdr_tree = proto_item_add_subtree(msrp_headers_item, ett_msrp_hdr);

        /
        while (tvb_offset_exists(tvb, offset) && offset < end_line_offset  ) {
            /
            linelen = tvb_find_line_end(tvb, offset, -1, &next_offset, FALSE);
            if (linelen == 0) {
                /
                have_body = TRUE;
                break;
            }
            line_end_offset = offset + linelen;
            colon_offset = tvb_find_guint8(tvb, offset, linelen, ':');
            if (colon_offset == -1) {
                /
                hdr_str = tvb_format_text(tvb, offset, linelen);
                proto_tree_add_string_format(msrp_hdr_tree, hf_msrp_hdr, tvb, offset,
                                    next_offset - offset, hdr_str, "%s", hdr_str);
            } else {
                header_len = colon_offset - offset;
                hf_index = msrp_is_known_msrp_header(tvb, offset, header_len);

                if (hf_index == -1) {
                    hdr_str = tvb_format_text(tvb, offset, linelen);
                    proto_tree_add_string_format(msrp_hdr_tree, hf_msrp_hdr, tvb,
                                    offset, next_offset - offset, hdr_str, "%s", hdr_str);
                } else {
                    /
                    value_offset = colon_offset + 1;
                    while (value_offset < line_end_offset &&
                           ((c = tvb_get_guint8(tvb, value_offset)) == ' ' ||
                             c == '\t'))
                        value_offset++;
                    /
                    value_len = line_end_offset - value_offset;
                    value = tvb_get_string_enc(wmem_packet_scope(), tvb, value_offset,
                                       value_len, ENC_UTF_8|ENC_NA);

                    /
                    proto_tree_add_string_format(msrp_hdr_tree,
                                   hf_header_array[hf_index], tvb,
                                   offset, next_offset - offset,
                                   value, "%s",
                                   tvb_format_text(tvb, offset, linelen));

                    switch ( hf_index ) {

                        case MSRP_CONTENT_TYPE :
                            content_type_len = value_len;
                            semi_colon_offset = tvb_find_guint8(tvb, value_offset,linelen, ';');
                            if ( semi_colon_offset != -1) {
                                parameter_offset = semi_colon_offset +1;
                                /
                                while (parameter_offset < line_end_offset
                                       && ((c = tvb_get_guint8(tvb, parameter_offset)) == ' '
                                         || c == '\t'))
                                    parameter_offset++;
                                content_type_len = semi_colon_offset - value_offset;
                                content_type_parameter_str_len = line_end_offset - parameter_offset;
                                content_type_parameter_str = tvb_get_string_enc(wmem_packet_scope(), tvb,
                                             parameter_offset, content_type_parameter_str_len, ENC_UTF_8|ENC_NA);
                            }
                            media_type_str_lower_case = ascii_strdown_inplace(
                                                            (gchar *)tvb_get_string_enc(wmem_packet_scope(), tvb, value_offset, content_type_len, ENC_UTF_8|ENC_NA));
                            break;

                        default:
                            break;
                    }
                }
            }
            offset = next_offset;
        }/

        if ( have_body ){
            /
            proto_item_set_end(msrp_headers_item, tvb, next_offset);

            /
            next_tvb = tvb_new_subset_remaining(tvb, next_offset);
            ti = proto_tree_add_item(msrp_tree, hf_msrp_data, tvb,
                                     next_offset, -1, ENC_UTF_8|ENC_NA);
            msrp_data_tree = proto_item_add_subtree(ti, ett_msrp_data);

            /

            if ( media_type_str_lower_case != NULL ) {
                found_match = dissector_try_string(media_type_dissector_table,
                                               media_type_str_lower_case,
                                               next_tvb, pinfo,
                                               msrp_data_tree, content_type_parameter_str);
                /
            }
            if ( found_match == 0 )
            {
                offset = 0;
                while (tvb_offset_exists(next_tvb, offset)) {
                    tvb_find_line_end(next_tvb, offset, -1, &next_offset, FALSE);
                    linelen = next_offset - offset;
                    proto_tree_add_format_text(msrp_data_tree, next_tvb, offset, linelen);
                    offset = next_offset;
                }/
            }

        }



        /
        ti = proto_tree_add_item(msrp_tree,hf_msrp_end_line,tvb,end_line_offset,end_line_len,ENC_UTF_8|ENC_NA);
        msrp_end_tree = proto_item_add_subtree(ti, ett_msrp_end_line);

        proto_tree_add_item(msrp_end_tree,hf_msrp_transactionID,tvb,end_line_offset + 7,token_2_len,ENC_UTF_8|ENC_NA);
        /
        proto_tree_add_item(msrp_end_tree,hf_msrp_cnt_flg,tvb,end_line_offset+end_line_len-1,1,ENC_UTF_8|ENC_NA);

        if (global_msrp_raw_text && tree) {
            raw_tree = proto_tree_add_subtree(tree, tvb, 0, -1, ett_msrp, NULL, "Message Session Relay Protocol(as raw text)");
            tvb_raw_text_add(tvb,raw_tree);
        }

    }/
    return message_end_offset;
    /

/
}
