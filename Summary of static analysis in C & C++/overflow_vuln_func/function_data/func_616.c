static void
dissect_afp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	struct aspinfo *aspinfo = pinfo->private_data;
	proto_tree      *afp_tree = NULL;
	proto_item	*ti;
	conversation_t	*conversation;
	gint		offset = 0;
	afp_request_key request_key, *new_request_key;
	afp_request_val *request_val;
	guint8	afp_command;

	int     len =  tvb_reported_length_remaining(tvb,0);
	
	if (check_col(pinfo->cinfo, COL_PROTOCOL))
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "AFP");
	if (check_col(pinfo->cinfo, COL_INFO))
		col_clear(pinfo->cinfo, COL_INFO);

	conversation = find_conversation(&pinfo->src, &pinfo->dst, pinfo->ptype,
		pinfo->srcport, pinfo->destport, 0);

	if (conversation == NULL)
	{
		conversation = conversation_new(&pinfo->src, &pinfo->dst,
			pinfo->ptype, pinfo->srcport, pinfo->destport, 0);
	}

	request_key.conversation = conversation->index;	
	request_key.seq = aspinfo->seq;

	request_val = (afp_request_val *) g_hash_table_lookup(
								afp_request_hash, &request_key);

	if (!request_val && !aspinfo->reply)  {
		afp_command = tvb_get_guint8(tvb, offset);
		new_request_key = g_mem_chunk_alloc(afp_request_keys);
		*new_request_key = request_key;

		request_val = g_mem_chunk_alloc(afp_request_vals);
		request_val->command = tvb_get_guint8(tvb, offset);

		g_hash_table_insert(afp_request_hash, new_request_key,
								request_val);
	}

	if (!request_val) {	/
		return;
	}

	afp_command = request_val->command;
	if (check_col(pinfo->cinfo, COL_INFO)) {
		col_add_fstr(pinfo->cinfo, COL_INFO, "%s %s",
			     val_to_str(afp_command, CommandCode_vals,
					"Unknown command (%u)"),
			     aspinfo->reply ? "reply" : "request");
	}

	if (tree)
	{
		ti = proto_tree_add_item(tree, proto_afp, tvb, offset, -1,FALSE);
		afp_tree = proto_item_add_subtree(ti, ett_afp);
	}
	if (!aspinfo->reply)  {
		proto_tree_add_uint(afp_tree, hf_afp_command, tvb,offset, 1, afp_command);
		offset++;
		switch(afp_command) {
		case AFP_BYTELOCK:
			offset = dissect_query_afp_byte_lock(tvb, pinfo, afp_tree, offset);break; 
		case AFP_OPENDT: 	/
		case AFP_FLUSH:		
		case AFP_CLOSEVOL:
			offset = dissect_query_afp_with_vol_id(tvb, pinfo, afp_tree, offset);break;
		case AFP_CLOSEDIR:
			/
			break;
		case AFP_CLOSEDT:	
			offset = dissect_query_afp_close_dt(tvb, pinfo, afp_tree, offset);break;
		case AFP_FLUSHFORK: /
		case AFP_CLOSEFORK:
			offset = dissect_query_afp_with_fork(tvb, pinfo, afp_tree, offset);break;
		case AFP_COPYFILE:
			/
		case AFP_CREATEFILE:
			offset = dissect_query_afp_create_file(tvb, pinfo, afp_tree, offset);break; 
		case AFP_ENUMERATE:
			offset = dissect_query_afp_enumerate(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETFORKPARAM:
			offset = dissect_query_afp_get_fork_param(tvb, pinfo, afp_tree, offset);break; 
		case AFP_GETSRVINFO:
			/
		case AFP_GETSRVPARAM:
			break;					/
		case AFP_GETVOLPARAM:
			offset = dissect_query_afp_get_vol_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_LOGIN:
			offset = dissect_query_afp_login(tvb, pinfo, afp_tree, offset);break;
		case AFP_LOGINCONT:
		case AFP_LOGOUT:
		case AFP_MAPID:
		case AFP_MAPNAME:
			break;
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
		case AFP_GETFLDRPARAM:
			offset = dissect_query_afp_get_fldr_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_SETFLDRPARAM:
			offset = dissect_query_afp_set_fldr_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_CHANGEPW:
		case AFP_GETSRVRMSG:
			break;
		case AFP_DELETE:	/
		case AFP_CREATEDIR:
		case AFP_CREATEID:
			offset = dissect_query_afp_create_id(tvb, pinfo, afp_tree, offset);break; 
		case AFP_DELETEID:
			offset = dissect_query_afp_delete_id(tvb, pinfo, afp_tree, offset);break; 
		case AFP_RESOLVEID:
			offset = dissect_query_afp_resolve_id(tvb, pinfo, afp_tree, offset);break; 
		case AFP_EXCHANGEFILE:
			break;
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
			break;
 		}
	}
 	else {
		proto_tree_add_uint(afp_tree, hf_afp_command, tvb, 0, 0, afp_command);
 		if (!len) {
 			/
 			return;
 		}
 		switch(afp_command) {
		case AFP_BYTELOCK:
			offset = dissect_reply_afp_byte_lock(tvb, pinfo, afp_tree, offset);break; 
 		case AFP_ENUMERATE:
 			offset = dissect_reply_afp_enumerate(tvb, pinfo, afp_tree, offset);break;
 		case AFP_OPENVOL:
 			offset = dissect_reply_afp_open_vol(tvb, pinfo, afp_tree, offset);break;
		case AFP_OPENFORK:
			offset = dissect_reply_afp_open_fork(tvb, pinfo, afp_tree, offset);break;
		case AFP_RESOLVEID:
		case AFP_GETFORKPARAM:
			offset =dissect_reply_afp_get_fork_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_GETSRVPARAM:
			offset = dissect_reply_afp_get_server_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_CREATEDIR:
			offset = dissect_reply_afp_create_dir(tvb, pinfo, afp_tree, offset);break; 
		case AFP_MOVE:		/
		case AFP_CREATEID:
			offset = dissect_reply_afp_create_id(tvb, pinfo, afp_tree, offset);break; 
		case AFP_GETVOLPARAM:
			offset = dissect_reply_afp_get_vol_param(tvb, pinfo, afp_tree, offset);break;
 		case AFP_GETFLDRPARAM:
 			offset = dissect_reply_afp_get_fldr_param(tvb, pinfo, afp_tree, offset);break;
		case AFP_OPENDT:
			offset = dissect_reply_afp_open_dt(tvb, pinfo, afp_tree, offset);break;
		case AFP_CATSEARCH:
			offset = dissect_reply_afp_cat_search(tvb, pinfo, afp_tree, offset);break; 
		case AFP_GETICON:
			offset = dissect_reply_afp_get_icon(tvb, pinfo, afp_tree, offset);break; 
		case AFP_GTICNINFO:
			offset = dissect_reply_afp_get_icon_info(tvb, pinfo, afp_tree, offset);break; 
		case AFP_GETAPPL:
			offset = dissect_reply_afp_get_appl(tvb, pinfo, afp_tree, offset);break; 
		case AFP_GETCMT:
			offset = dissect_reply_afp_get_cmt(tvb, pinfo, afp_tree, offset);break; 
		case AFP_WRITE:
			offset = dissect_reply_afp_write(tvb, pinfo, afp_tree, offset);break;
		}
	}
	if (tree && offset < len)
		call_dissector(data_handle,tvb_new_subset(tvb, offset,-1,tvb_reported_length_remaining(tvb,offset)), pinfo, afp_tree);
}
