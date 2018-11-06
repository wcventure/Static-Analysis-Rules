static void
dissect_afp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	struct aspinfo	*aspinfo = pinfo->private_data;
	proto_tree	*afp_tree = NULL;
	proto_item	*ti;
	conversation_t	*conversation;
	gint		offset = 0;
	afp_request_key request_key, *new_request_key;
	afp_request_val *request_val;
	guint8		afp_command;
	nstime_t	delta_ts;

	int	len =  tvb_reported_length(tvb);

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "AFP");
	col_clear(pinfo->cinfo, COL_INFO);

	conversation = find_or_create_conversation(pinfo);

	request_key.conversation = conversation->index;
	request_key.seq = aspinfo->seq;

	request_val = (afp_request_val *) g_hash_table_lookup(
								afp_request_hash, &request_key);

	if (!request_val && !aspinfo->reply)  {
		afp_command = tvb_get_guint8(tvb, offset);
		new_request_key = se_alloc(sizeof(afp_request_key));
		*new_request_key = request_key;

		request_val = se_alloc(sizeof(afp_request_val));
		request_val->command = afp_command;

		if (afp_command == AFP_SPOTLIGHTRPC)
			request_val->spotlight_req_command = tvb_get_ntohl(tvb, offset + 2 + 2 + 4);
		else
			request_val->spotlight_req_command = -1;

		request_val->frame_req = pinfo->fd->num;
		request_val->frame_res = 0;
		request_val->req_time=pinfo->fd->abs_ts;

		g_hash_table_insert(afp_request_hash, new_request_key,
								request_val);
	}

	if (!request_val) {	/
		col_set_str(pinfo->cinfo, COL_INFO, "[Reply without query?]");
		return;
	}

	afp_command = request_val->command;
	col_add_fstr(pinfo->cinfo, COL_INFO, "%s %s",
		     val_to_str_ext(afp_command, &CommandCode_vals_ext,
				"Unknown command (%u)"),
		     aspinfo->reply ? "reply" : "request");
	if (aspinfo->reply && aspinfo->code != 0) {
		col_append_fstr(pinfo->cinfo, COL_INFO, ": %s (%d)",
			val_to_str_ext(aspinfo->code, &asp_error_vals_ext,
				"Unknown error (%u)"), aspinfo->code);
	}

	if (tree)
	{
		ti = proto_tree_add_item(tree, proto_afp, tvb, offset, -1,ENC_NA);
		afp_tree = proto_item_add_subtree(ti, ett_afp);
	}
	if (!aspinfo->reply)  {

		ti = proto_tree_add_uint(afp_tree, hf_afp_command, tvb,offset, 1, afp_command);
		if (afp_command != tvb_get_guint8(tvb, offset))
		{
			/
			col_set_str(pinfo->cinfo, COL_INFO,
				    "[Error!IP port reused, you need to split the capture file]");
			expert_add_info_format(pinfo, ti, PI_SEQUENCE, PI_WARN,
					       "IP port reused, you need to split the capture file");
			return;
		}

		/
		if (request_val->frame_res != 0) {
			ti = proto_tree_add_uint(afp_tree, hf_afp_response_in,
			    tvb, 0, 0, request_val->frame_res);
			PROTO_ITEM_SET_GENERATED(ti);
		}

		offset++;
		switch(afp_command) {
		case AFP_BYTELOCK:
			offset = dissect_query_afp_byte_lock(tvb, pinfo, afp_tree, offset);break;
		case AFP_BYTELOCK_EXT:
			offset = dissect_query_afp_byte_lock_ext(tvb, pinfo, afp_tree, offset);break;
		case AFP_OPENDT:	/
		case AFP_FLUSH:
		case AFP_CLOSEVOL:
			offset = dissect_query_afp_with_vol_id(tvb, pinfo, afp_tree, offset);break;
		case AFP_CLOSEDIR:
			/
			break;
		case AFP_CLOSEDT:
			offset = dissect_query_afp_close_dt(tvb, pinfo, afp_tree, offset);break;
		case AFP_FLUSHFORK: /
		case AFP_SYNCFORK:
		case AFP_CLOSEFORK:
			offset = dissect_query_afp_with_fork(tvb, pinfo, afp_tree, offset);break;
		case AFP_COPYFILE:
			offset = dissect_query_afp_copy_file(tvb, pinfo, afp_tree, offset);break;
		case AFP_CREATEFILE:
			offset = dissect_query_afp_create_file(tvb, pinfo, afp_tree, offset);break;
		case AFP_DISCTOLDSESS:
			offset = dissect_query_afp_disconnect_old_session(tvb, pinfo, afp_tree, offset);break;
		case AFP_ENUMERATE_EXT2:
			offset = dissect_query_afp_enumerate_ext2(tvb, pinfo, afp_tree, offset);break;
		case AFP_ENUMERATE_EXT:
		case AFP_ENUMERATE:
			offset = dissect_query_afp_enumerate(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETFORKPARAM:
			offset = dissect_query_afp_get_fork_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETSESSTOKEN:
			offset = dissect_query_afp_get_session_token(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETUSERINFO:
			offset = dissect_query_afp_get_user_info(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETSRVINFO:
			/
		case AFP_GETSRVPARAM:
			break;					/
		case AFP_GETVOLPARAM:
			offset = dissect_query_afp_get_vol_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_LOGIN_EXT:
			offset = dissect_query_afp_login_ext(tvb, pinfo, afp_tree, offset);break;
		case AFP_LOGIN:
			offset = dissect_query_afp_login(tvb, pinfo, afp_tree, offset);break;
		case AFP_LOGINCONT:
		case AFP_LOGOUT:
			break;
		case AFP_MAPID:
			offset = dissect_query_afp_map_id(tvb, pinfo, afp_tree, offset);break;
		case AFP_MAPNAME:
			offset = dissect_query_afp_map_name(tvb, pinfo, afp_tree, offset);break;
		case AFP_MOVE:
			offset = dissect_query_afp_move(tvb, pinfo, afp_tree, offset);break;
		case AFP_OPENVOL:
			offset = dissect_query_afp_open_vol(tvb, pinfo, afp_tree, offset);break;
		case AFP_OPENDIR:
			break;
		case AFP_OPENFORK:
			offset = dissect_query_afp_open_fork(tvb, pinfo, afp_tree, offset);break;
		case AFP_READ:
			offset = dissect_query_afp_read(tvb, pinfo, afp_tree, offset);break;
		case AFP_READ_EXT:
			offset = dissect_query_afp_read_ext(tvb, pinfo, afp_tree, offset);break;
		case AFP_RENAME:
			offset = dissect_query_afp_rename(tvb, pinfo, afp_tree, offset);break;
		case AFP_SETDIRPARAM:
			offset = dissect_query_afp_set_dir_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_SETFILEPARAM:
			offset = dissect_query_afp_set_file_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_SETFORKPARAM:
			offset = dissect_query_afp_set_fork_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_SETVOLPARAM:
			offset = dissect_query_afp_set_vol_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_WRITE:
			offset = dissect_query_afp_write(tvb, pinfo, afp_tree, offset);break;
		case AFP_WRITE_EXT:
			offset = dissect_query_afp_write_ext(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETFLDRPARAM:
			offset = dissect_query_afp_get_fldr_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_SETFLDRPARAM:
			offset = dissect_query_afp_set_fldr_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_CHANGEPW:
			break;
		case AFP_GETSRVRMSG:
			offset = dissect_query_afp_get_server_message(tvb, pinfo, afp_tree, offset);break;
		case AFP_DELETE:	/
		case AFP_CREATEDIR:
		case AFP_CREATEID:
			offset = dissect_query_afp_create_id(tvb, pinfo, afp_tree, offset);break;
		case AFP_DELETEID:
			offset = dissect_query_afp_delete_id(tvb, pinfo, afp_tree, offset);break;
		case AFP_RESOLVEID:
			offset = dissect_query_afp_resolve_id(tvb, pinfo, afp_tree, offset);break;
		case AFP_EXCHANGEFILE:
			offset = dissect_query_afp_exchange_file(tvb, pinfo, afp_tree, offset);break;
		case AFP_CATSEARCH_EXT:
			offset = dissect_query_afp_cat_search_ext(tvb, pinfo, afp_tree, offset);break;
		case AFP_CATSEARCH:
			offset = dissect_query_afp_cat_search(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETICON:
			offset = dissect_query_afp_get_icon(tvb, pinfo, afp_tree, offset);break;
		case AFP_GTICNINFO:
			offset = dissect_query_afp_get_icon_info(tvb, pinfo, afp_tree, offset);break;
		case AFP_ADDAPPL:
			offset = dissect_query_afp_add_appl(tvb, pinfo, afp_tree, offset);break;
		case AFP_RMVAPPL:
			offset = dissect_query_afp_rmv_appl(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETAPPL:
			offset = dissect_query_afp_get_appl(tvb, pinfo, afp_tree, offset);break;
		case AFP_ADDCMT:
			offset = dissect_query_afp_add_cmt(tvb, pinfo, afp_tree, offset);break;
		case AFP_RMVCMT: /
		case AFP_GETCMT:
			offset = dissect_query_afp_get_cmt(tvb, pinfo, afp_tree, offset);break;
		case AFP_ADDICON:
			offset = dissect_query_afp_add_icon(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETEXTATTR:
			offset = dissect_query_afp_get_ext_attr(tvb, pinfo, afp_tree, offset);break;
		case AFP_SETEXTATTR:
			offset = dissect_query_afp_set_ext_attr(tvb, pinfo, afp_tree, offset);break;
		case AFP_LISTEXTATTR:
			offset = dissect_query_afp_list_ext_attrs(tvb, pinfo, afp_tree, offset);break;
		case AFP_REMOVEATTR:
			offset = dissect_query_afp_remove_ext_attr(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETACL:
			offset = dissect_query_afp_get_acl(tvb, pinfo, afp_tree, offset);break;
		case AFP_SETACL:
			offset = dissect_query_afp_set_acl(tvb, pinfo, afp_tree, offset);break;
		case AFP_ACCESS:
			offset = dissect_query_afp_access(tvb, pinfo, afp_tree, offset);break;
		case AFP_SYNCDIR:
			offset = dissect_query_afp_with_did(tvb, pinfo, afp_tree, offset);break;
		case AFP_SPOTLIGHTRPC:
			offset = dissect_query_afp_spotlight(tvb, pinfo, afp_tree, offset, request_val);
			break;
		}
	}
	else {
		proto_tree_add_uint(afp_tree, hf_afp_command, tvb, 0, 0, afp_command);

		/
		if (request_val->frame_req != 0) {
			ti = proto_tree_add_uint(afp_tree, hf_afp_response_to,
			    tvb, 0, 0, request_val->frame_req);
			PROTO_ITEM_SET_GENERATED(ti);
			nstime_delta(&delta_ts, &pinfo->fd->abs_ts, &request_val->req_time);
			ti = proto_tree_add_time(afp_tree, hf_afp_time, tvb,
			    0, 0, &delta_ts);
			PROTO_ITEM_SET_GENERATED(ti);
		}

		/
		if (request_val->frame_res == 0)
			request_val->frame_res = pinfo->fd->num;

		/
		tap_queue_packet(afp_tap, pinfo, request_val);

		if (!len) {
			/
			return;
		}

		switch(afp_command) {
		case AFP_BYTELOCK:
			offset = dissect_reply_afp_byte_lock(tvb, pinfo, afp_tree, offset);break;
		case AFP_BYTELOCK_EXT:
			offset = dissect_reply_afp_byte_lock_ext(tvb, pinfo, afp_tree, offset);break;
		case AFP_ENUMERATE_EXT2:
		case AFP_ENUMERATE_EXT:
			offset = dissect_reply_afp_enumerate_ext(tvb, pinfo, afp_tree, offset);break;
		case AFP_ENUMERATE:
			offset = dissect_reply_afp_enumerate(tvb, pinfo, afp_tree, offset);break;
		case AFP_OPENVOL:
			offset = dissect_reply_afp_open_vol(tvb, pinfo, afp_tree, offset);break;
		case AFP_OPENFORK:
			offset = dissect_reply_afp_open_fork(tvb, pinfo, afp_tree, offset);break;
		case AFP_RESOLVEID:
		case AFP_GETFORKPARAM:
			offset =dissect_reply_afp_get_fork_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETUSERINFO:
			offset = dissect_reply_afp_get_user_info(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETSRVPARAM:
			offset = dissect_reply_afp_get_server_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETSRVRMSG:
			offset = dissect_reply_afp_get_server_message(tvb, pinfo, afp_tree, offset);break;
		case AFP_CREATEDIR:
			offset = dissect_reply_afp_create_dir(tvb, pinfo, afp_tree, offset);break;
		case AFP_MAPID:
			offset = dissect_reply_afp_map_id(tvb, pinfo, afp_tree, offset);break;
		case AFP_MAPNAME:
			offset = dissect_reply_afp_map_name(tvb, pinfo, afp_tree, offset);break;
		case AFP_MOVE:		/
		case AFP_CREATEID:
			offset = dissect_reply_afp_create_id(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETSESSTOKEN:
			offset = dissect_reply_afp_get_session_token(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETVOLPARAM:
			offset = dissect_reply_afp_get_vol_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETFLDRPARAM:
			offset = dissect_reply_afp_get_fldr_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_OPENDT:
			offset = dissect_reply_afp_open_dt(tvb, pinfo, afp_tree, offset);break;
		case AFP_CATSEARCH_EXT:
			offset = dissect_reply_afp_cat_search_ext(tvb, pinfo, afp_tree, offset);break;
		case AFP_CATSEARCH:
			offset = dissect_reply_afp_cat_search(tvb, pinfo, afp_tree, offset);break;
		case AFP_GTICNINFO:
			offset = dissect_reply_afp_get_icon_info(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETAPPL:
			offset = dissect_reply_afp_get_appl(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETCMT:
			offset = dissect_reply_afp_get_cmt(tvb, pinfo, afp_tree, offset);break;
		case AFP_WRITE:
			offset = dissect_reply_afp_write(tvb, pinfo, afp_tree, offset);break;
		case AFP_WRITE_EXT:
			offset = dissect_reply_afp_write_ext(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETEXTATTR:
			offset = dissect_reply_afp_get_ext_attr(tvb, pinfo, afp_tree, offset);break;
		case AFP_LISTEXTATTR:
			offset = dissect_reply_afp_list_ext_attrs(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETACL:
			offset = dissect_reply_afp_get_acl(tvb, pinfo, afp_tree, offset);break;
		case AFP_SPOTLIGHTRPC:
			offset = dissect_reply_afp_spotlight(tvb, pinfo, afp_tree, offset, request_val);break;
		}
	}
	if (offset < len) {
		call_dissector(data_handle, tvb_new_subset_remaining(tvb, offset),
		    pinfo, afp_tree);
	}
}
