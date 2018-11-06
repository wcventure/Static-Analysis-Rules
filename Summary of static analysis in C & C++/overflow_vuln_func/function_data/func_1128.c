static int
dissect_http_message(tvbuff_t *tvb, int offset, packet_info *pinfo,
		     proto_tree *tree, http_conv_t *conv_data,
		     const char* proto_tag, int proto, gboolean end_of_stream)
{
	proto_tree	*http_tree = NULL;
	proto_item	*ti = NULL;
	proto_item	*hidden_item;
	const guchar	*line, *firstline;
	gint		next_offset;
	const guchar	*linep, *lineend;
	int		orig_offset;
	int		first_linelen, linelen;
	gboolean	is_request_or_reply, is_ssl = FALSE;
	gboolean	saw_req_resp_or_header;
	guchar		c;
	http_type_t     http_type;
	proto_item	*hdr_item = NULL;
	ReqRespDissector reqresp_dissector;
	proto_tree	*req_tree;
	int		colon_offset;
	headers_t	headers;
	int		datalen;
	int		reported_datalen = -1;
	dissector_handle_t handle;
	gboolean	dissected = FALSE;
	gboolean	first_loop = TRUE;
	gboolean	have_seen_http = FALSE;
	/
	/
	http_eo_t       *eo_info;
	heur_dtbl_entry_t *hdtbl_entry;
	int reported_length;
	guint16 word;
	gboolean	leading_crlf = FALSE;

	reported_length = tvb_reported_length_remaining(tvb, offset);
	if (reported_length < 1) {
		return -1;
	}

	/
	if (reported_length > 3) {
		word = tvb_get_ntohs(tvb,offset);
		if (word == 0x0d0a) {
			leading_crlf = TRUE;
			offset += 2;
		}
	}

	/
	if (conv_data->request_uri)
		have_seen_http = TRUE;

	/
	if (!g_ascii_isprint(tvb_get_guint8(tvb, offset))) {
		/
		if (have_seen_http) {
			col_set_str(pinfo->cinfo, COL_PROTOCOL, proto_tag);
			col_set_str(pinfo->cinfo, COL_INFO, "Continuation");
			ti = proto_tree_add_item(tree, proto, tvb, offset, -1, ENC_NA);
			http_tree = proto_item_add_subtree(ti, ett_http);

			call_data_dissector(tvb, pinfo, http_tree);
		}
		return -1;
	}

	/
	first_linelen = tvb_find_line_end(tvb, offset,
	    tvb_ensure_captured_length_remaining(tvb, offset), &next_offset,
	    TRUE);

	if (first_linelen == -1) {
		/
		if (!req_resp_hdrs_do_reassembly(tvb, offset, pinfo,
		    http_desegment_headers, http_desegment_body)) {
			/
			return -1;
		}
	}

	/
	firstline = tvb_get_ptr(tvb, offset, first_linelen);
	http_type = HTTP_OTHERS;	/
	is_request_or_reply = is_http_request_or_reply((const gchar *)firstline,
	    first_linelen, &http_type, NULL, conv_data);
	if (is_request_or_reply) {
		gboolean try_desegment_body;

		/
		col_add_fstr(pinfo->cinfo, COL_INFO, "%s ", format_text(firstline, first_linelen));

		/
		try_desegment_body = (http_desegment_body &&
			(!(conv_data->request_method && g_str_equal(conv_data->request_method, "HEAD"))) &&
			!end_of_stream);
		if (!req_resp_hdrs_do_reassembly(tvb, offset, pinfo,
		    http_desegment_headers, try_desegment_body)) {
			/
			return -1;
		}
	} else if (have_seen_http) {
		 /
		col_set_str(pinfo->cinfo, COL_INFO, "Continuation");
	}

	if (is_request_or_reply || have_seen_http) {
		/
		col_set_str(pinfo->cinfo, COL_PROTOCOL, proto_tag);
		ti = proto_tree_add_item(tree, proto, tvb, offset, -1, ENC_NA);
		http_tree = proto_item_add_subtree(ti, ett_http);

		if (leading_crlf) {
			proto_tree_add_expert(http_tree, pinfo, &ei_http_leading_crlf, tvb, offset-2, 2);
		}
	}

	is_ssl = proto_is_frame_protocol(pinfo->layers, "ssl");

	stat_info = wmem_new(wmem_packet_scope(), http_info_value_t);
	stat_info->framenum = pinfo->num;
	stat_info->response_code = 0;
	stat_info->request_method = NULL;
	stat_info->request_uri = NULL;
	stat_info->http_host = NULL;

	orig_offset = offset;

	/
	http_type = HTTP_OTHERS;	/
	headers.content_type = NULL;	/
	headers.content_type_parameters = NULL;	/
	headers.have_content_length = FALSE;	/
	headers.content_length = 0;		/
	headers.content_encoding = NULL; /
	headers.transfer_encoding_chunked = FALSE;
	headers.transfer_encoding = HTTP_TE_NONE;
	headers.upgrade = 0; /
	saw_req_resp_or_header = FALSE;	/
	while (tvb_offset_exists(tvb, offset)) {
		/
		linelen = tvb_find_line_end(tvb, offset,
		    tvb_ensure_captured_length_remaining(tvb, offset), &next_offset,
		    FALSE);
		if (linelen < 0)
			return -1;

		/
		line = tvb_get_ptr(tvb, offset, linelen);
		lineend = line + linelen;
		colon_offset = -1;

		/
		reqresp_dissector = NULL;
		is_request_or_reply =
		    is_http_request_or_reply((const gchar *)line,
		    linelen, &http_type, &reqresp_dissector, conv_data);
		if (is_request_or_reply)
			goto is_http;

		/
		if (linelen == 0)
			goto is_http;	/

		/
		linep = line;
		colon_offset = offset;
		while (linep < lineend) {
			c = *linep++;

			/
			if (!g_ascii_isprint(c))
				break;

			/
			switch (c) {

			case '(':
			case ')':
			case '<':
			case '>':
			case '@':
			case ',':
			case ';':
			case '\\':
			case '"':
			case '/':
			case '[':
			case ']':
			case '?':
			case '=':
			case '{':
			case '}':
			case ' ':
				/
				goto not_http;

			case ':':
				/
				goto is_http;

			default:
				colon_offset++;
				break;
			}
		}

		/
		if (saw_req_resp_or_header)
			tvb_ensure_bytes_exist(tvb, offset, linelen + 1);

	not_http:
		/
		break;

	is_http:
		if ((tree) && (http_tree == NULL)) {
			ti = proto_tree_add_item(tree, proto, tvb, orig_offset, -1, ENC_NA);
			http_tree = proto_item_add_subtree(ti, ett_http);
			if (leading_crlf) {
				proto_tree_add_expert(http_tree, pinfo, &ei_http_leading_crlf, tvb, orig_offset-2, 2);
			}
		}

		if (first_loop && !is_ssl && pinfo->ptype == PT_TCP &&
				(pinfo->srcport == 443 || pinfo->destport == 443)) {
			expert_add_info(pinfo, ti, &ei_http_ssl_port);
		}

		first_loop = FALSE;

		/

		if (linelen == 0) {
			/
			proto_tree_add_format_text(http_tree, tvb, offset, next_offset - offset);
			offset = next_offset;
			break;
		}

		/
		saw_req_resp_or_header = TRUE;
		if (is_request_or_reply) {
			char *text = tvb_format_text(tvb, offset, next_offset - offset);

			req_tree = proto_tree_add_subtree(http_tree, tvb,
				    offset, next_offset - offset, ett_http_request, &hdr_item, text);

			expert_add_info_format(pinfo, hdr_item, &ei_http_chat, "%s", text);
			if (reqresp_dissector) {
				reqresp_dissector(tvb, req_tree, offset, line,
						  lineend, conv_data);
			}
		} else {
			/
			process_header(tvb, offset, next_offset, line, linelen,
			    colon_offset, pinfo, http_tree, &headers, conv_data,
			    http_type);
		}
		offset = next_offset;
	}

	if (tree && stat_info->http_host && stat_info->request_uri) {
		proto_item *e_ti;
		gchar      *uri;

		if ((g_ascii_strncasecmp(stat_info->request_uri, "http://", 7) == 0) ||
		    (g_ascii_strncasecmp(stat_info->request_uri, "https://", 8) == 0) ||
		    (g_ascii_strncasecmp(conv_data->request_method, "CONNECT", 7) == 0)) {
			uri = wmem_strdup(wmem_packet_scope(), stat_info->request_uri);
		}
		else {
			uri = wmem_strdup_printf(wmem_packet_scope(), "%s://%s%s",
				    is_ssl ? "https" : "http",
				    g_strstrip(wmem_strdup(wmem_packet_scope(), stat_info->http_host)), stat_info->request_uri);
		}

		e_ti = proto_tree_add_string(http_tree,
					     hf_http_request_full_uri, tvb, 0,
					     0, uri);

		PROTO_ITEM_SET_URL(e_ti);
		PROTO_ITEM_SET_GENERATED(e_ti);
	}

	if (!PINFO_FD_VISITED(pinfo)) {
		if (http_type == HTTP_REQUEST) {
			push_req(conv_data, pinfo);
		} else if (http_type == HTTP_RESPONSE) {
			push_res(conv_data, pinfo);
		}
	}

	if (tree) {
		proto_item *pi;
		http_req_res_t *curr = (http_req_res_t *)p_get_proto_data(wmem_file_scope(), pinfo, proto_http, 0);
		http_req_res_t *prev = curr ? curr->prev : NULL;
		http_req_res_t *next = curr ? curr->next : NULL;

		switch (http_type) {

		case HTTP_NOTIFICATION:
			hidden_item = proto_tree_add_boolean(http_tree,
					    hf_http_notification, tvb, 0, 0, 1);
			PROTO_ITEM_SET_HIDDEN(hidden_item);
			break;

		case HTTP_RESPONSE:
			hidden_item = proto_tree_add_boolean(http_tree,
					    hf_http_response, tvb, 0, 0, 1);
			PROTO_ITEM_SET_HIDDEN(hidden_item);

			if (curr) {
				nstime_t delta;

				pi = proto_tree_add_uint_format(http_tree, hf_http_response_number, tvb, 0, 0, curr->number, "HTTP response %u/%u", curr->number, conv_data->req_res_num);
				PROTO_ITEM_SET_GENERATED(pi);

				if (! nstime_is_unset(&(curr->req_ts))) {
					nstime_delta(&delta, &pinfo->abs_ts, &(curr->req_ts));
					pi = proto_tree_add_time(http_tree, hf_http_time, tvb, 0, 0, &delta);
					PROTO_ITEM_SET_GENERATED(pi);
				}
			}
			if (prev && prev->req_framenum) {
				pi = proto_tree_add_uint(http_tree, hf_http_prev_request_in, tvb, 0, 0, prev->req_framenum);
				PROTO_ITEM_SET_GENERATED(pi);
			}
			if (prev && prev->res_framenum) {
				pi = proto_tree_add_uint(http_tree, hf_http_prev_response_in, tvb, 0, 0, prev->res_framenum);
				PROTO_ITEM_SET_GENERATED(pi);
			}
			if (curr && curr->req_framenum) {
				pi = proto_tree_add_uint(http_tree, hf_http_request_in, tvb, 0, 0, curr->req_framenum);
				PROTO_ITEM_SET_GENERATED(pi);
			}
			if (next && next->req_framenum) {
				pi = proto_tree_add_uint(http_tree, hf_http_next_request_in, tvb, 0, 0, next->req_framenum);
				PROTO_ITEM_SET_GENERATED(pi);
			}
			if (next && next->res_framenum) {
				pi = proto_tree_add_uint(http_tree, hf_http_next_response_in, tvb, 0, 0, next->res_framenum);
				PROTO_ITEM_SET_GENERATED(pi);
			}

			break;

		case HTTP_REQUEST:
			hidden_item = proto_tree_add_boolean(http_tree,
					    hf_http_request, tvb, 0, 0, 1);
			PROTO_ITEM_SET_HIDDEN(hidden_item);

			if (curr) {
				pi = proto_tree_add_uint_format(http_tree, hf_http_request_number, tvb, 0, 0, curr->number, "HTTP request %u/%u", curr->number, conv_data->req_res_num);
				PROTO_ITEM_SET_GENERATED(pi);
			}
			if (prev && prev->req_framenum) {
				pi = proto_tree_add_uint(http_tree, hf_http_prev_request_in, tvb, 0, 0, prev->req_framenum);
				PROTO_ITEM_SET_GENERATED(pi);
			}
			if (curr && curr->res_framenum) {
				pi = proto_tree_add_uint(http_tree, hf_http_response_in, tvb, 0, 0, curr->res_framenum);
				PROTO_ITEM_SET_GENERATED(pi);
			}
			if (next && next->req_framenum) {
				pi = proto_tree_add_uint(http_tree, hf_http_next_request_in, tvb, 0, 0, next->req_framenum);
				PROTO_ITEM_SET_GENERATED(pi);
			}

			break;

		case HTTP_OTHERS:
		default:
			break;
		}
	}

	/
	if(have_tap_listener(http_follow_tap)) {
		tap_queue_packet(http_follow_tap, pinfo, tvb_new_subset_length(tvb, 0, offset));
	}

	reported_datalen = tvb_reported_length_remaining(tvb, offset);
	datalen = tvb_captured_length_remaining(tvb, offset);

	/
	if (headers.have_content_length &&
	    headers.transfer_encoding == HTTP_TE_NONE) {
		if (datalen > headers.content_length)
			datalen = (int)headers.content_length;

		/
		if (reported_datalen > headers.content_length)
			reported_datalen = (int)headers.content_length;
	} else {
		switch (http_type) {

		case HTTP_REQUEST:
			/
			if (headers.transfer_encoding == HTTP_TE_NONE)
				datalen = 0;
			else
				reported_datalen = -1;
			break;

		case HTTP_RESPONSE:
			if ((stat_info->response_code/100) == 1 ||
			    stat_info->response_code == 204 ||
			    stat_info->response_code == 304)
				datalen = 0;	/
			else {
				/
				reported_datalen = -1;
			}
			break;

		default:
			/
			reported_datalen = -1;
			break;
		}
	}

	if (datalen > 0) {
		/
		tvbuff_t *next_tvb;
		guint chunked_datalen = 0;
		char *media_str = NULL;
		const gchar *file_data;

		/
		next_tvb = tvb_new_subset(tvb, offset, datalen,
		    reported_datalen);

		/
		if (headers.transfer_encoding_chunked) {
			if (!http_dechunk_body) {
				/
				call_data_dissector(next_tvb, pinfo, http_tree);
				goto body_dissected;
			}

			chunked_datalen = chunked_encoding_dissector(
			    &next_tvb, pinfo, http_tree, 0);

			if (chunked_datalen == 0) {
				/
				goto body_dissected;
			} else {
				/
#if 0 /
				tvb_set_child_real_data_tvbuff(tvb,
					next_tvb);
#endif
				add_new_data_source(pinfo, next_tvb,
					"De-chunked entity body");
				/
				datalen = chunked_datalen;
			}
		}
		/
		switch (headers.transfer_encoding) {
		case HTTP_TE_COMPRESS:
		case HTTP_TE_DEFLATE:
		case HTTP_TE_GZIP:
			/
			call_data_dissector(next_tvb, pinfo, http_tree);
			goto body_dissected;
		default:
			/
			break;
		}
		/
		if (headers.content_encoding != NULL &&
		    g_ascii_strcasecmp(headers.content_encoding, "identity") != 0) {
			/
			tvbuff_t *uncomp_tvb = NULL;
			proto_item *e_ti = NULL;
			proto_tree *e_tree = NULL;

			if (http_decompress_body &&
			    (g_ascii_strcasecmp(headers.content_encoding, "gzip") == 0 ||
			     g_ascii_strcasecmp(headers.content_encoding, "deflate") == 0 ||
			     g_ascii_strcasecmp(headers.content_encoding, "x-gzip") == 0 ||
			     g_ascii_strcasecmp(headers.content_encoding, "x-deflate") == 0))
			{
				uncomp_tvb = tvb_child_uncompress(tvb, next_tvb, 0,
				    tvb_captured_length(next_tvb));
			}

			/
			e_tree = proto_tree_add_subtree_format(http_tree, next_tvb,
					0, tvb_captured_length(next_tvb), ett_http_encoded_entity, &e_ti,
					"Content-encoded entity body (%s): %u bytes",
					headers.content_encoding,
					tvb_captured_length(next_tvb));

			if (uncomp_tvb != NULL) {
				/

				/
				proto_item_append_text(e_ti, " -> %u bytes", tvb_captured_length(uncomp_tvb));
				next_tvb = uncomp_tvb;
				add_new_data_source(pinfo, next_tvb,
				    "Uncompressed entity body");
			} else {
				proto_item_append_text(e_ti, " [Error: Decompression failed]");
				call_data_dissector(next_tvb, pinfo, e_tree);

				goto body_dissected;
			}
		}
		/

		/
		if(have_tap_listener(http_eo_tap)) {
			eo_info = wmem_new(wmem_packet_scope(), http_eo_t);

			eo_info->hostname = conv_data->http_host;
			eo_info->filename = conv_data->request_uri;
			eo_info->content_type = headers.content_type;
			eo_info->payload_len = tvb_captured_length(next_tvb);
			eo_info->payload_data = tvb_get_ptr(next_tvb, 0, eo_info->payload_len);

			tap_queue_packet(http_eo_tap, pinfo, eo_info);
		}

		/
		if(have_tap_listener(http_follow_tap)) {
			tap_queue_packet(http_follow_tap, pinfo, next_tvb);
		}
		file_data = tvb_get_string_enc(wmem_packet_scope(), next_tvb, 0, tvb_reported_length(next_tvb), ENC_ASCII);
		proto_tree_add_string_format_value(http_tree, hf_http_file_data,
			next_tvb, 0, tvb_reported_length(next_tvb), file_data, "%u bytes", tvb_reported_length(next_tvb));

		/
		handle = NULL;
		if (headers.content_type != NULL) {
			/
			if (headers.content_type_parameters)
				media_str = wmem_strdup(wmem_packet_scope(), headers.content_type_parameters);

			/
			pinfo->match_string = headers.content_type;
			handle = dissector_get_string_handle(
			    media_type_subdissector_table,
			    headers.content_type);
			if (handle == NULL &&
			    strncmp(headers.content_type, "multipart/", sizeof("multipart/")-1) == 0) {
				/
				handle = dissector_get_string_handle(
				    media_type_subdissector_table,
				    "multipart/");
			}
		}

		/
		if (handle == NULL) {
			handle = dissector_get_uint_handle(port_subdissector_table,
			    pinfo->match_uint);
		}

		if (handle != NULL) {
			/
			dissected = call_dissector_only(handle, next_tvb, pinfo, tree, media_str);
			if (!dissected)
				expert_add_info(pinfo, http_tree, &ei_http_subdissector_failed);
		}

		if (!dissected) {
			/
			dissected = dissector_try_heuristic(heur_subdissector_list,
							    next_tvb, pinfo, tree, &hdtbl_entry, NULL);
		}

		if (dissected) {
			/
			if (ti != NULL)
				proto_item_set_len(ti, offset);
		} else {
			if (headers.content_type != NULL) {
				/
				call_dissector_with_data(media_handle, next_tvb, pinfo, tree, media_str);
			} else {
				/
				call_data_dissector(next_tvb, pinfo, http_tree);
			}
		}

	body_dissected:
		/
		offset += datalen;
	}

	if (http_type == HTTP_RESPONSE && conv_data->upgrade == UPGRADE_SSTP) {
		conv_data->startframe = pinfo->num + 1;
		headers.upgrade = conv_data->upgrade;
	}

	if (http_type == HTTP_RESPONSE && pinfo->desegment_offset<=0 && pinfo->desegment_len<=0) {
		conv_data->upgrade = headers.upgrade;
		conv_data->startframe = pinfo->num + 1;
		copy_address_wmem(wmem_file_scope(), &conv_data->server_addr, &pinfo->src);
		conv_data->server_port = pinfo->srcport;
	}

	tap_queue_packet(http_tap, pinfo, stat_info);

	return offset - orig_offset;
}
