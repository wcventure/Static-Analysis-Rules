static gboolean
dissect_sip_common(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
    gboolean dissect_other_as_continuation)
{
        int offset;
        gint next_offset, linelen;
	line_type_t line_type;
        tvbuff_t *next_tvb;
        gboolean is_known_request;
	gboolean found_match = FALSE;
        char *descr;
        guint token_1_len;
	guint current_method_idx = 0;
        proto_item *ts = NULL, *ti = NULL, *th = NULL, *sip_element_item = NULL;
        proto_tree *sip_tree = NULL, *reqresp_tree = NULL , *hdr_tree = NULL, *sip_element_tree = NULL, *message_body_tree = NULL;
	guchar contacts = 0, contact_is_star = 0, expires_is_0 = 0;
	guint32 cseq_number = 0;
	guchar  cseq_number_set = 0;
	char    cseq_method[16] = "";
	char	call_id[MAX_CALL_ID_SIZE] = "";
	char *media_type_str = NULL;
	char *media_type_str_lower_case = NULL;
	char *content_type_parameter_str = NULL;
	guint resend_for_packet = 0;
	char  *string;

	/
	stat_info = g_malloc(sizeof(sip_info_value_t));
	stat_info->response_code = 0;
	stat_info->request_method = NULL;
	stat_info->reason_phrase = NULL;
	stat_info->resend = 0;
	stat_info->tap_call_id = NULL;
	stat_info->tap_from_addr = NULL;
	stat_info->tap_to_addr = NULL;

        /
        offset = 0;
        linelen = tvb_find_line_end(tvb, 0, -1, &next_offset, FALSE);
        line_type = sip_parse_line(tvb, linelen, &token_1_len);
        if (line_type == OTHER_LINE) {
        	/
                if (!dissect_other_as_continuation) {
                        /
                	return FALSE;
                }

                /
        }

        if (check_col(pinfo->cinfo, COL_PROTOCOL))
                col_set_str(pinfo->cinfo, COL_PROTOCOL, "SIP");

        switch (line_type) {

        case REQUEST_LINE:
                is_known_request = sip_is_known_request(tvb, 0, token_1_len, &current_method_idx);
                descr = is_known_request ? "Request" : "Unknown request";
                if (check_col(pinfo->cinfo, COL_INFO)) {
                        col_add_fstr(pinfo->cinfo, COL_INFO, "%s: %s",
                             descr,
                             tvb_format_text(tvb, 0, linelen - SIP2_HDR_LEN - 1));
                }
		break;

        case STATUS_LINE:
                descr = "Status";
                if (check_col(pinfo->cinfo, COL_INFO)) {
                        col_add_fstr(pinfo->cinfo, COL_INFO, "Status: %s",
                             tvb_format_text(tvb, SIP2_HDR_LEN + 1, linelen - SIP2_HDR_LEN - 1));
                }
                string = tvb_get_string(tvb, SIP2_HDR_LEN + 5, linelen - (SIP2_HDR_LEN + 5));
                stat_info->reason_phrase = g_malloc(linelen - (SIP2_HDR_LEN + 5) + 1);
                strncpy(stat_info->reason_phrase, string, linelen - (SIP2_HDR_LEN + 5) + 1);
                /
                g_free(string);
		break;

	case OTHER_LINE:
	default: /
	        descr = "Continuation";
                if (check_col(pinfo->cinfo, COL_INFO))
                        col_set_str(pinfo->cinfo, COL_INFO, "Continuation");
                break;
        }

        if (tree) {
                ts = proto_tree_add_item(tree, proto_sip, tvb, 0, -1, FALSE);
                sip_tree = proto_item_add_subtree(ts, ett_sip);
	}

	switch (line_type) {

	case REQUEST_LINE:
		if (sip_tree) {
	        	ti = proto_tree_add_string(sip_tree, hf_Request_Line, tvb, 0, linelen,
	                              tvb_format_text(tvb, 0, linelen));
	        	reqresp_tree = proto_item_add_subtree(ti, ett_sip_reqresp);
	        }
	        dfilter_sip_request_line(tvb, reqresp_tree, token_1_len);
	        break;

	case STATUS_LINE:
		if (sip_tree) {
	        	ti = proto_tree_add_string(sip_tree, hf_Status_Line, tvb, 0, linelen,
	       	                      tvb_format_text(tvb, 0, linelen));
	        	reqresp_tree = proto_item_add_subtree(ti, ett_sip_reqresp);
	        }
	        dfilter_sip_status_line(tvb, reqresp_tree);
	        break;

	case OTHER_LINE:
		if (sip_tree) {
	        	ti = proto_tree_add_text(sip_tree, tvb, 0, next_offset,
		                         "%s line: %s", descr,
	                                 tvb_format_text(tvb, 0, linelen));
	        	reqresp_tree = proto_item_add_subtree(ti, ett_sip_reqresp);
	        	proto_tree_add_text(sip_tree, tvb, 0, -1,
	            		"Continuation data");
	        }
	        return TRUE;
	}

	offset = next_offset;
	if (sip_tree) {
		th = proto_tree_add_item(sip_tree, hf_msg_hdr, tvb, offset, -1, FALSE);
		hdr_tree = proto_item_add_subtree(th, ett_sip_hdr);
	}

        /
        next_offset = offset;
        while (tvb_reported_length_remaining(tvb, offset) > 0) {
                gint line_end_offset;
                gint colon_offset;
                gint semi_colon_offset;
				gint len;
                gint parameter_offset;
				gint parameter_end_offset;
				gint parameter_len;
		gint content_type_len, content_type_parameter_str_len;
                gint header_len;
                gint hf_index;
                gint value_offset;
                gint comma_offset;
                gint comma_next_offset;
                gint sip_offset;
                gint con_offset;
                guchar c;
		size_t value_len;
                char *value;

                linelen = tvb_find_line_end(tvb, offset, -1, &next_offset,
                    FALSE);
                if (linelen == 0) {
                        /
                        break;
                }
                line_end_offset = offset + linelen;
		colon_offset = tvb_find_guint8(tvb, offset, linelen, ':');
		if (colon_offset == -1) {
			/
			if(hdr_tree) {
				proto_tree_add_text(hdr_tree, tvb, offset,
				    next_offset - offset, "%s",
				    tvb_format_text(tvb, offset, linelen));
			}
		} else {
			header_len = colon_offset - offset;
			hf_index = sip_is_known_sip_header(tvb,
			    offset, header_len);

			if (hf_index == -1) {
				if(hdr_tree) {
					proto_tree_add_text(hdr_tree, tvb,
					    offset, next_offset - offset, "%s",
					    tvb_format_text(tvb, offset, linelen));
				}
			} else {
				/
				value_offset = colon_offset + 1;
				while (value_offset < line_end_offset
				    && ((c = tvb_get_guint8(tvb,
					    value_offset)) == ' '
				      || c == '\t'))
					value_offset++;
				/
				value_len = line_end_offset - value_offset;
				value = tvb_get_string(tvb, value_offset,
				    value_len);

				/
				switch ( hf_index ) {

				case POS_TO :
					if(hdr_tree) {
						sip_element_item = proto_tree_add_string_format(hdr_tree,
						    hf_header_array[hf_index], tvb,
						    offset, next_offset - offset,
						    value, "%s",
						    tvb_format_text(tvb, offset, linelen));
						sip_element_tree = proto_item_add_subtree( sip_element_item,
							 ett_sip_element);
					}
					/
					parameter_offset = tvb_find_guint8(tvb, value_offset,value_len, '<');
					if ( parameter_offset != -1){
						len = parameter_offset - value_offset;
						if ( len > 1){
							/
							proto_tree_add_item(sip_element_tree, hf_sip_display, tvb, value_offset,
								len, FALSE);
						}
						parameter_offset ++;
						parameter_end_offset = parameter_offset;
						/
						while (parameter_end_offset < line_end_offset){
							parameter_end_offset++;
							c = tvb_get_guint8(tvb, parameter_end_offset);
							switch (c) {
								case '>':
								case ',':
								case ';':
								case '?':
									goto separator_found;
								default :
									break;
							}
						}
	separator_found:
						parameter_len = parameter_end_offset - parameter_offset;
						proto_tree_add_item(sip_element_tree, hf_sip_to_addr, tvb, parameter_offset,
							parameter_len, FALSE);
						/
						stat_info->tap_to_addr=tvb_get_string(tvb, parameter_offset, parameter_len);

						parameter_offset = parameter_end_offset + 1;
						/
						parameter_end_offset = tvb_find_guint8(tvb, parameter_offset,( line_end_offset - parameter_offset), ';');
						if ( parameter_end_offset == -1)
							parameter_end_offset = line_end_offset;

						offset = parameter_end_offset;
					}
					else
					{
					/
					parameter_offset = value_offset;
					while (parameter_offset < line_end_offset
						&& (tvb_strneql(tvb, parameter_offset, "sip", 3) != 0))
						parameter_offset++;
					len = parameter_offset - value_offset;
					if ( len > 1){
						/
						proto_tree_add_item(sip_element_tree, hf_sip_display, tvb, value_offset,
							len, FALSE);
					}
					parameter_end_offset = tvb_find_guint8(tvb, parameter_offset,
						( line_end_offset - parameter_offset), ';');
					if ( parameter_end_offset == -1)
						parameter_end_offset = line_end_offset;
					parameter_len = parameter_end_offset - parameter_offset;
					proto_tree_add_item(sip_element_tree, hf_sip_to_addr, tvb, parameter_offset,
						parameter_len, FALSE);
					/
					stat_info->tap_to_addr=tvb_get_string(tvb, parameter_offset, parameter_len);
					offset = parameter_end_offset;
					}
					/

					parameter_offset = offset;
					while (parameter_offset < line_end_offset
						&& (tvb_strneql(tvb, parameter_offset, "tag=", 4) != 0))
						parameter_offset++;
					if ( parameter_offset < line_end_offset ){ /
						parameter_offset = parameter_offset + 4;
						parameter_end_offset = tvb_find_guint8(tvb, parameter_offset,
							( line_end_offset - parameter_offset), ';');
						if ( parameter_end_offset == -1)
							parameter_end_offset = line_end_offset;
						parameter_len = parameter_end_offset - parameter_offset;
						proto_tree_add_item(sip_element_tree, hf_sip_tag, tvb, parameter_offset,
							parameter_len, FALSE);

					}
					break;

				case POS_FROM :
					if(hdr_tree) {
						sip_element_item = proto_tree_add_string_format(hdr_tree,
						    hf_header_array[hf_index], tvb,
						    offset, next_offset - offset,
						    value, "%s",
						    tvb_format_text(tvb, offset, linelen));
						sip_element_tree = proto_item_add_subtree( sip_element_item, ett_sip_element);
					}
					/
					parameter_offset = tvb_find_guint8(tvb, value_offset,value_len, '<');
					if ( parameter_offset != -1){
						len = parameter_offset - value_offset;
						if ( len > 1){
							/
							proto_tree_add_item(sip_element_tree, hf_sip_display, tvb, value_offset,
								len, FALSE);
						}
						parameter_offset ++;
						parameter_end_offset = parameter_offset;
						/
						while (parameter_end_offset < line_end_offset){
							parameter_end_offset++;
							c = tvb_get_guint8(tvb, parameter_end_offset);
							switch (c) {
								case '>':
								case ',':
								case ';':
								case '?':
									goto separator_found2;
								default :
									break;
							}
						}
	separator_found2:
						parameter_len = parameter_end_offset - parameter_offset;
						dfilter_store_sip_from_addr(tvb, sip_element_tree, 
							parameter_offset, parameter_len);
						/
						stat_info->tap_from_addr=tvb_get_string(tvb, parameter_offset, parameter_len);
						parameter_offset = parameter_end_offset + 1;
						/
						parameter_end_offset = tvb_find_guint8(tvb, parameter_offset,( line_end_offset - parameter_offset), ';');
						if ( parameter_end_offset == -1)
							parameter_end_offset = line_end_offset;

						offset = parameter_end_offset;
					}
					else
					{
					/
					parameter_offset = value_offset;
					while (parameter_offset < line_end_offset
						&& (tvb_strneql(tvb, parameter_offset, "sip", 3) != 0))
						parameter_offset++;
					len = parameter_offset - value_offset;
					if ( len > 1){
						/
						proto_tree_add_item(sip_element_tree, hf_sip_display, tvb, value_offset,
							len, FALSE);
					}
					parameter_end_offset = tvb_find_guint8(tvb, parameter_offset,
						( line_end_offset - parameter_offset), ';');
					if ( parameter_end_offset == -1)
						parameter_end_offset = line_end_offset;
					parameter_len = parameter_end_offset - parameter_offset;
					proto_tree_add_item(sip_element_tree, hf_sip_from_addr, tvb, parameter_offset,
						parameter_len, FALSE);
					/
					stat_info->tap_from_addr=tvb_get_string(tvb, parameter_offset, parameter_len);
					offset = parameter_end_offset;
					}
					/

					parameter_offset = offset;
					while (parameter_offset < line_end_offset
						&& (tvb_strneql(tvb, parameter_offset, "tag=", 4) != 0))
						parameter_offset++;
					if ( parameter_offset < line_end_offset ){ /
						parameter_offset = parameter_offset + 4;
						parameter_end_offset = tvb_find_guint8(tvb, parameter_offset,
							( line_end_offset - parameter_offset), ';');
						if ( parameter_end_offset == -1)
							parameter_end_offset = line_end_offset;
						parameter_len = parameter_end_offset - parameter_offset;
						proto_tree_add_item(sip_element_tree, hf_sip_tag, tvb, parameter_offset,
							parameter_len, FALSE);

					}
					break;

				case POS_CSEQ :
					/
					cseq_number = atoi(value);
					cseq_number_set = 1;
					stat_info->tap_cseq_number=cseq_number;

					/
					for (value_offset = 0; value_offset < (gint)strlen(value); value_offset++)
					{
						if (isalpha((guchar)value[value_offset]))
						{
							strcpy(cseq_method,value+value_offset);
							break;
						}
					}
					/
					if(hdr_tree) {
						proto_tree_add_string_format(hdr_tree,
						    hf_header_array[hf_index], tvb,
						    offset, next_offset - offset,
						    value, "%s",
						    tvb_format_text(tvb, offset, linelen));
					}
					break;

				case POS_CALL_ID :
					/
					strncpy(call_id, value,
						strlen(value)+1 < MAX_CALL_ID_SIZE ?
							strlen(value)+1 :
							MAX_CALL_ID_SIZE);
					stat_info->tap_call_id = g_strdup(call_id);
					
					/
					if(hdr_tree) {
						proto_tree_add_string_format(hdr_tree,
						    hf_header_array[hf_index], tvb,
						    offset, next_offset - offset,
						    value, "%s",
						    tvb_format_text(tvb, offset, linelen));
					}
					break;

				case POS_EXPIRES :
					if (strcmp(value, "0") == 0)
					{
						expires_is_0 = 1;
					}
					/
					if(hdr_tree) {
						proto_tree_add_string_format(hdr_tree,
						    hf_header_array[hf_index], tvb,
						    offset, next_offset - offset,
						    value, "%s",
						    tvb_format_text(tvb, offset, linelen));
					}
					break;

				/
				case POS_CONTENT_TYPE :
					if(hdr_tree) {
						proto_tree_add_string_format(hdr_tree,
						    hf_header_array[hf_index], tvb,
						    offset, next_offset - offset,
						    value, "%s",
						    tvb_format_text(tvb, offset, linelen));
					}
					content_type_len = value_len;
					semi_colon_offset = tvb_find_guint8(tvb, value_offset,linelen, ';');
					if ( semi_colon_offset != -1) {
						parameter_offset = semi_colon_offset +1;
						/
						while (parameter_offset < line_end_offset
					    && ((c = tvb_get_guint8(tvb,
						    parameter_offset)) == ' '
					      || c == '\t'))
						parameter_offset++;
						content_type_len = semi_colon_offset - value_offset;
						content_type_parameter_str_len = line_end_offset - parameter_offset;
						content_type_parameter_str = tvb_get_string(tvb, parameter_offset,
								content_type_parameter_str_len);
					}
					media_type_str = tvb_get_string(tvb, value_offset, content_type_len);
#if GLIB_MAJOR_VERSION < 2
					media_type_str_lower_case = g_strdup(media_type_str);
					g_strdown(media_type_str_lower_case);
#else
					media_type_str_lower_case = g_ascii_strdown(media_type_str, -1);
#endif
					g_free(media_type_str);
					break;

				case POS_CONTACT :
					comma_offset = tvb_find_guint8(tvb, value_offset,value_len, ',');
					if (comma_offset != -1) {
						con_offset = value_offset + 1;
						while (comma_offset != -1) {
							sip_offset = tvb_find_guint8(tvb, con_offset, (comma_offset - con_offset), 's');
							if (sip_offset != -1 && tvb_get_guint8(tvb, sip_offset+1) == 'i' && tvb_get_guint8(tvb, sip_offset+2) == 'p')
								contacts++;
							comma_next_offset = tvb_find_guint8(tvb, comma_offset + 1, value_len - (comma_offset - value_offset), ',');
							if (comma_next_offset == -1)
								con_offset = comma_offset + 1;
							else
								con_offset = comma_next_offset + 1;
							comma_offset = comma_next_offset;
						}
						sip_offset = tvb_find_guint8(tvb, con_offset, value_len - (con_offset - value_offset), 's');
						if (sip_offset != -1 && tvb_get_guint8(tvb, sip_offset+1) == 'i' && tvb_get_guint8(tvb, sip_offset+2) == 'p')
							contacts++;
					}
					else {
						contacts++;
						if (strcmp(value, "*") == 0)
						{
							contact_is_star = 1;
						}
					}
					/

				default :
					if(hdr_tree) {
						proto_tree_add_string_format(hdr_tree,
						    hf_header_array[hf_index], tvb,
						    offset, next_offset - offset,
						    value, "%s",
						    tvb_format_text(tvb, offset, linelen));
					}
				break;
				}/
				g_free(value);
			}/
		}/
                offset = next_offset;
        }/

        if (tvb_offset_exists(tvb, next_offset)) {

                /
                proto_item_set_end(th, tvb, next_offset);
                next_tvb = tvb_new_subset(tvb, next_offset, -1, -1);
                if(sip_tree) {
               		ti = proto_tree_add_text(sip_tree, next_tvb, 0, -1,
                	                         "Message body");
               		message_body_tree = proto_item_add_subtree(ti, ett_sip_message_body);
		}

		/

		if ( media_type_str_lower_case != NULL ) {
			void *save_private_data = pinfo->private_data;
			pinfo->private_data = content_type_parameter_str;
			found_match = dissector_try_string(media_type_dissector_table,
							   media_type_str_lower_case,
							   next_tvb, pinfo,
							   message_body_tree);
			g_free(media_type_str_lower_case);
			pinfo->private_data = save_private_data;
			/
		}
		g_free(content_type_parameter_str);
		if ( found_match != TRUE )
		{
			offset = 0;
	        	while (tvb_offset_exists(next_tvb, offset)) {
	                	tvb_find_line_end(next_tvb, offset, -1, &next_offset, FALSE);
	                	linelen = next_offset - offset;
	                	if(message_body_tree) {
	                		proto_tree_add_text(message_body_tree, next_tvb, offset, linelen,
	                   			"%s", tvb_format_text(next_tvb, offset, linelen));
	                   	}
 	        		offset = next_offset;
 	 		}/
		}
        }


	/
	if (check_col(pinfo->cinfo, COL_INFO))
	{
		/
		if (strcmp(sip_methods[current_method_idx], "REGISTER") == 0)
		{
			if (contact_is_star && expires_is_0)
			{
				col_append_str(pinfo->cinfo, COL_INFO, "    (remove all bindings)");
			}
			else
			if (!contacts)
			{
				col_append_str(pinfo->cinfo, COL_INFO, "    (fetch bindings)");
			}
		}

		/
		if (line_type == STATUS_LINE && (strcmp(cseq_method, "REGISTER") == 0))
		{
			col_append_fstr(pinfo->cinfo, COL_INFO, "    (%d bindings)", contacts);
		}
	}

	/
	resend_for_packet = sip_is_packet_resend(pinfo, cseq_method, call_id,
						cseq_number_set, cseq_number,
						line_type);
	/
	stat_info->resend = (resend_for_packet > 0);

	/
	if (reqresp_tree)
	{
		proto_item *item;
		item = proto_tree_add_boolean(reqresp_tree, hf_sip_resend, tvb, 0, 0,
						resend_for_packet > 0);
		PROTO_ITEM_SET_GENERATED(item);
		if (resend_for_packet > 0)
		{
			item = proto_tree_add_uint(reqresp_tree, hf_sip_original_frame,
						tvb, 0, 0, resend_for_packet);
			PROTO_ITEM_SET_GENERATED(item);
		}
	}


        if (global_sip_raw_text)
                tvb_raw_text_add(tvb, tree);

	/
	if (!pinfo->in_error_pkt)
	{
		tap_queue_packet(sip_tap, pinfo, stat_info);
	}

        return TRUE;
}
