static void
dissect_imap(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	gboolean        is_request;
	proto_tree      *imap_tree, *reqresp_tree;
	proto_item      *ti, *hidden_item;
	gint		offset = 0;
	gint		uid_offset = 0;
	gint            folder_offset = 0;
	const guchar	*line;
	const guchar    *uid_line;
	const guchar    *folder_line;
	gint		next_offset;
	int		linelen;
	int		tokenlen;
	int             uid_tokenlen;
	int             folder_tokenlen;
	const guchar	*next_token;
	const guchar    *uid_next_token;
	const guchar    *folder_next_token;
	guchar          *tokenbuf;
	guchar          *command_token;
	int             iter;
	int             commandlen;

	tokenbuf = (guchar *)wmem_alloc(wmem_packet_scope(), MAX_BUFFER);
	command_token = (guchar *)wmem_alloc(wmem_packet_scope(), MAX_BUFFER);
	memset(tokenbuf, '\0', MAX_BUFFER);
	memset(command_token, '\0', MAX_BUFFER);
	commandlen = 0;
	folder_offset = 0;
	folder_tokenlen = 0;
	folder_line = NULL;
	col_set_str(pinfo->cinfo, COL_PROTOCOL, "IMAP");


	if (pinfo->match_uint == pinfo->destport)
		is_request = TRUE;
	else
		is_request = FALSE;

	/
	linelen = tvb_find_line_end(tvb, offset, -1, &next_offset, FALSE);
	line = tvb_get_ptr(tvb, offset, linelen);

	col_add_fstr(pinfo->cinfo, COL_INFO, "%s: %s",
			     is_request ? "Request" : "Response",
			     format_text(line, linelen));

	if (tree) {
		ti = proto_tree_add_item(tree, proto_imap, tvb, offset, -1, ENC_NA);
		imap_tree = proto_item_add_subtree(ti, ett_imap);

		hidden_item = proto_tree_add_boolean(imap_tree, hf_imap_isrequest, tvb, 0, 0, is_request);
		PROTO_ITEM_SET_HIDDEN(hidden_item);

		while(tvb_length_remaining(tvb, offset) > 0) {

			/
			linelen = tvb_find_line_end(tvb, offset, -1, &next_offset, FALSE);
			line = tvb_get_ptr(tvb, offset, linelen);

			/
			ti = proto_tree_add_item(imap_tree, hf_imap_line, tvb, offset,
						 next_offset - offset, ENC_ASCII|ENC_NA);

			reqresp_tree = proto_item_add_subtree(ti, ett_imap_reqresp);

			/
			if ( (line) && ((line[0] != '*') || (TRUE == is_request)) ) {
			  /

			  /
			  tokenlen = get_token_len(line, line + linelen, &next_token);
			  if (tokenlen != 0) {
			    proto_tree_add_item(reqresp_tree, (is_request) ? hf_imap_request_tag : hf_imap_response_tag,
						tvb, offset, tokenlen, ENC_ASCII|ENC_NA);

			    offset += (gint) (next_token - line);
			    linelen -= (int) (next_token - line);
			    line = next_token;
			  }

			  /
			  tokenlen = get_token_len(line, line + linelen, &next_token);
			  if (tokenlen != 0) {
			    for (iter = 0; iter < tokenlen && iter < MAX_BUFFER-1; iter++) {
			      tokenbuf[iter] = tolower(line[iter]);
			    }
			    if ( TRUE == is_request && strncmp(tokenbuf,"uid",tokenlen) == 0) {
			      proto_tree_add_item(reqresp_tree, hf_imap_request_uid, tvb, offset, tokenlen, ENC_ASCII|ENC_NA);
			      /
			      uid_offset = offset;
			      uid_offset += (gint) (next_token - line);
			      uid_line = next_token;
			      uid_tokenlen = get_token_len(uid_line, uid_line + (linelen - tokenlen), &uid_next_token);
			      if (tokenlen != 0) {
				proto_tree_add_item(reqresp_tree, hf_imap_request_command,
						    tvb, uid_offset, uid_tokenlen, ENC_ASCII|ENC_NA);

				/
				for (iter = 0; iter < uid_tokenlen && iter < MAX_BUFFER-1; iter++) {
				  command_token[iter] = tolower(uid_line[iter]);
				}
				commandlen = uid_tokenlen;

				folder_offset = uid_offset;
				folder_offset += (gint) (uid_next_token - uid_line);
				folder_line = uid_next_token;
				folder_tokenlen = get_token_len(folder_line, folder_line + (linelen - tokenlen - uid_tokenlen), &folder_next_token);
			      }
			    } else {
			      /
			      proto_tree_add_item(reqresp_tree, (is_request) ? hf_imap_request_command : hf_imap_response_status,
						  tvb, offset, tokenlen, ENC_ASCII|ENC_NA);

			      if (is_request) {
				/
				for (iter = 0; iter < tokenlen && iter < 256; iter++) {
				  command_token[iter] = tolower(line[iter]);
				}
				commandlen = tokenlen;

				folder_offset = offset;
				folder_offset += (gint) (next_token - line);
				folder_line = next_token;
				folder_tokenlen = get_token_len(folder_line, folder_line + (linelen - tokenlen), &folder_next_token);
			      }
			    }

			    if (commandlen > 0 && (
				strncmp(command_token, "select", commandlen) == 0 ||
				strncmp(command_token, "examine", commandlen) == 0 ||
				strncmp(command_token, "create", commandlen) == 0 ||
				strncmp(command_token, "delete", commandlen) == 0 ||
				strncmp(command_token, "rename", commandlen) == 0 ||
				strncmp(command_token, "subscribe", commandlen) == 0 ||
				strncmp(command_token, "unsubscribe", commandlen) == 0 ||
				strncmp(command_token, "status", commandlen) == 0 ||
				strncmp(command_token, "append", commandlen) == 0 ||
				strncmp(command_token, "search", commandlen) == 0)) {
			      /
			      if (folder_tokenlen != 0)
				proto_tree_add_item(reqresp_tree, hf_imap_request_folder, tvb, folder_offset, folder_tokenlen, ENC_ASCII|ENC_NA);
			    }

			    if ( is_request && (NULL != folder_line) &&
				 strncmp(command_token, "copy", commandlen) == 0) {
			      /
			      folder_offset += (gint) (folder_next_token - folder_line);
			      folder_line = folder_next_token;
			      folder_tokenlen = get_token_len(folder_line, folder_line + (linelen - tokenlen), &folder_next_token);

			      if (folder_tokenlen != 0)
				proto_tree_add_item(reqresp_tree, hf_imap_request_folder, tvb, folder_offset, folder_tokenlen, ENC_ASCII|ENC_NA);
			    }

			  }

			  /
			  if (linelen != 0) {
			    proto_tree_add_item(reqresp_tree, (is_request) ? hf_imap_request : hf_imap_response,
						tvb, offset, linelen, ENC_ASCII|ENC_NA);
			  }

			}

			offset = next_offset; /
		}
	}
}
