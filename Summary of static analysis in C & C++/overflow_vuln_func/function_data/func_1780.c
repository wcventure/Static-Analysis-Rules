static gboolean
dissect_smb(tvbuff_t *tvb, packet_info *pinfo, proto_tree *parent_tree)
{
	int offset = 0;
	proto_item *item = NULL, *hitem = NULL;
	proto_tree *tree = NULL, *htree = NULL;
	guint8          flags;
	guint16         flags2;
	smb_info_t 	si;
	smb_saved_info_t *sip = NULL;
	smb_saved_info_key_t key;
	smb_saved_info_key_t *new_key;
	guint32 nt_status = 0;
	guint8 errclass = 0;
	guint16 errcode = 0;
	guint32 pid_mid;
	conversation_t *conversation;
	nstime_t ns;

	top_tree=parent_tree;

	/
	if (!tvb_bytes_exist(tvb, 0, 4))
		return FALSE;

	if( (tvb_get_guint8(tvb, 0) != 0xff)
	    || (tvb_get_guint8(tvb, 1) != 'S')
	    || (tvb_get_guint8(tvb, 2) != 'M')
	    || (tvb_get_guint8(tvb, 3) != 'B') ){
		return FALSE;
	}

	if (check_col(pinfo->cinfo, COL_PROTOCOL)){
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "SMB");
	}
	if (check_col(pinfo->cinfo, COL_INFO)){
		col_clear(pinfo->cinfo, COL_INFO);
	}

	/
	si.cmd = tvb_get_guint8(tvb, offset+4);
	flags = tvb_get_guint8(tvb, offset+9);
	si.request = !(flags&SMB_FLAGS_DIRN);
	flags2 = tvb_get_letohs(tvb, offset+10);
	if(flags2 & 0x8000){
		si.unicode = TRUE; /
	} else {
		si.unicode = FALSE;
	}
	si.tid = tvb_get_letohs(tvb, offset+24);
	si.pid = tvb_get_letohs(tvb, offset+26);
	si.uid = tvb_get_letohs(tvb, offset+28);
	si.mid = tvb_get_letohs(tvb, offset+30);
	pid_mid = (si.pid << 16) | si.mid;
	si.info_level = -1;
	si.info_count = -1;

	if (parent_tree) {
		item = proto_tree_add_item(parent_tree, proto_smb, tvb, offset,
			-1, FALSE);
		tree = proto_item_add_subtree(item, ett_smb);

		hitem = proto_tree_add_text(tree, tvb, offset, 32,
			"SMB Header");

		htree = proto_item_add_subtree(hitem, ett_smb_hdr);
	}

	proto_tree_add_text(htree, tvb, offset, 4, "Server Component: SMB");
	offset += 4;  /

	/
	conversation = find_conversation(&pinfo->src, &pinfo->dst,
		 pinfo->ptype,  pinfo->srcport, pinfo->destport, 0);
	if(!conversation){
		/
		conversation = conversation_new(&pinfo->src, &pinfo->dst,
			pinfo->ptype, pinfo->srcport, pinfo->destport, 0);
	}
	/
	si.ct=conversation_get_proto_data(conversation, proto_smb);
	if(!si.ct){
		/
		si.ct = g_mem_chunk_alloc(conv_tables_chunk);
		conv_tables = g_slist_prepend(conv_tables, si.ct);
		si.ct->matched= g_hash_table_new(smb_saved_info_hash_matched,
			smb_saved_info_equal_matched);
		si.ct->unmatched= g_hash_table_new(smb_saved_info_hash_unmatched,
			smb_saved_info_equal_unmatched);
		si.ct->dcerpc_fid_to_frame=g_hash_table_new(
			smb_saved_info_hash_unmatched,
			smb_saved_info_equal_unmatched);
		si.ct->tid_service=g_hash_table_new(
			smb_saved_info_hash_unmatched,
			smb_saved_info_equal_unmatched);
		conversation_add_proto_data(conversation, proto_smb, si.ct);
	}

	if( (si.request)
	    &&  (si.mid==0)
	    &&  (si.uid==0)
	    &&  (si.pid==0)
	    &&  (si.tid==0) ){
		/
		si.unidir = TRUE;
	} else if( (si.cmd==SMB_COM_NT_CANCEL)     /
		   ||(si.cmd==SMB_COM_TRANSACTION_SECONDARY)   /
		   ||(si.cmd==SMB_COM_TRANSACTION2_SECONDARY)   /
		   ||(si.cmd==SMB_COM_NT_TRANSACT_SECONDARY)){ /
		/

		si.unidir = TRUE;  /

		if(!pinfo->fd->flags.visited){
			/
			/
			sip=g_hash_table_lookup(si.ct->unmatched, (void *)pid_mid);
			if(sip!=NULL){
				new_key = g_mem_chunk_alloc(smb_saved_info_key_chunk);
				new_key->frame = pinfo->fd->num;
				new_key->pid_mid = pid_mid;
				g_hash_table_insert(si.ct->matched, new_key,
				    sip);
			}
		} else {
			/
			key.frame = pinfo->fd->num;
			key.pid_mid = pid_mid;
			sip=g_hash_table_lookup(si.ct->matched, &key);
			if(sip==NULL){
			/
			}
		}


		if(sip && sip->frame_req){
			switch(si.cmd){
			case SMB_COM_NT_CANCEL:
				proto_tree_add_uint(htree, hf_smb_cancel_to,
						    tvb, 0, 0, sip->frame_req);
				break;
			case SMB_COM_TRANSACTION_SECONDARY:
			case SMB_COM_TRANSACTION2_SECONDARY:
			case SMB_COM_NT_TRANSACT_SECONDARY:
				proto_tree_add_uint(htree, hf_smb_continuation_to,
						    tvb, 0, 0, sip->frame_req);
				break;
			}
		} else {
			switch(si.cmd){
			case SMB_COM_NT_CANCEL:
				proto_tree_add_text(htree, tvb, 0, 0,
						    "Cancellation to: <unknown frame>");
				break;
			case SMB_COM_TRANSACTION_SECONDARY:
			case SMB_COM_TRANSACTION2_SECONDARY:
			case SMB_COM_NT_TRANSACT_SECONDARY:
				proto_tree_add_text(htree, tvb, 0, 0,
						    "Continuation to: <unknown frame>");
				break;
			}
		}
	} else { /
		si.unidir = FALSE;

		if(!pinfo->fd->flags.visited){
			/
			sip=g_hash_table_lookup(si.ct->unmatched, (void *)pid_mid);
			if(sip!=NULL){
				gboolean cmd_match=FALSE;

				/
				if(si.cmd==sip->cmd){
					cmd_match=TRUE;
				}
				else if(si.cmd==SMB_COM_NT_CANCEL){
					cmd_match=TRUE;
				}
				else if((si.cmd==SMB_COM_TRANSACTION_SECONDARY)
				     && (sip->cmd==SMB_COM_TRANSACTION)){
					cmd_match=TRUE;
				}
				else if((si.cmd==SMB_COM_TRANSACTION2_SECONDARY)
				     && (sip->cmd==SMB_COM_TRANSACTION2)){
					cmd_match=TRUE;
				}
				else if((si.cmd==SMB_COM_NT_TRANSACT_SECONDARY)
				     && (sip->cmd==SMB_COM_NT_TRANSACT)){
					cmd_match=TRUE;
				}

				if( (si.request) || (!cmd_match) ) {
					/
					g_hash_table_remove(si.ct->unmatched, (void *)pid_mid);
					sip=NULL; /
				} else {
					/
					if(sip->frame_res==0){
						/
						sip->frame_res = pinfo->fd->num;
						new_key = g_mem_chunk_alloc(smb_saved_info_key_chunk);
						new_key->frame = sip->frame_res;
						new_key->pid_mid = pid_mid;
						g_hash_table_insert(si.ct->matched, new_key, sip);
					} else {
						/
						new_key = g_mem_chunk_alloc(smb_saved_info_key_chunk);
						new_key->frame = pinfo->fd->num;
						new_key->pid_mid = pid_mid;
						g_hash_table_insert(si.ct->matched, new_key, sip);
					}
				}
			}
			if(si.request){
				sip = g_mem_chunk_alloc(smb_saved_info_chunk);
				sip->frame_req = pinfo->fd->num;
				sip->frame_res = 0;
				sip->req_time.secs=pinfo->fd->abs_secs;
				sip->req_time.nsecs=pinfo->fd->abs_usecs*1000;
				sip->flags = 0;
				if(g_hash_table_lookup(si.ct->tid_service, (void *)si.tid)
				    == (void *)TID_IPC) {
					sip->flags |= SMB_SIF_TID_IS_IPC;
				}
				sip->cmd = si.cmd;
				sip->extra_info = NULL;
				g_hash_table_insert(si.ct->unmatched, (void *)pid_mid, sip);
				new_key = g_mem_chunk_alloc(smb_saved_info_key_chunk);
				new_key->frame = sip->frame_req;
				new_key->pid_mid = pid_mid;
				g_hash_table_insert(si.ct->matched, new_key, sip);
			}
		} else {
			/
			key.frame = pinfo->fd->num;
			key.pid_mid = pid_mid;
			sip=g_hash_table_lookup(si.ct->matched, &key);
		}
	}

	/
	si.sip = sip;

	if (sip != NULL) {
		/
		if(si.request){
			if (sip->frame_res != 0)
				proto_tree_add_uint(htree, hf_smb_response_in, tvb, 0, 0, sip->frame_res);
		} else {
			if (sip->frame_req != 0) {
				proto_tree_add_uint(htree, hf_smb_response_to, tvb, 0, 0, sip->frame_req);
				ns.secs = pinfo->fd->abs_secs - sip->req_time.secs;
				ns.nsecs = pinfo->fd->abs_usecs*1000 - sip->req_time.nsecs;
				if(ns.nsecs<0){
					ns.nsecs+=1000000000;
					ns.secs--;
				}
				proto_tree_add_time(htree, hf_smb_time, tvb,
				    0, 0, &ns);
			}
		}
	}

	/
	proto_tree_add_uint_format(htree, hf_smb_cmd, tvb, offset, 1, si.cmd, "SMB Command: %s (0x%02x)", decode_smb_name(si.cmd), si.cmd);
	offset += 1;

	if(flags2 & 0x4000){
		/

                nt_status = tvb_get_letohl(tvb, offset);

		proto_tree_add_item(htree, hf_smb_nt_status, tvb, offset, 4,
			TRUE);
		offset += 4;

	} else {
		/
		errclass = tvb_get_guint8(tvb, offset);
		proto_tree_add_uint(htree, hf_smb_error_class, tvb, offset, 1,
			errclass);
		offset += 1;

		/
		proto_tree_add_item(htree, hf_smb_reserved, tvb, offset, 1, TRUE);
		offset += 1;

		/
		/
		errcode = tvb_get_letohs(tvb, offset);
		proto_tree_add_uint_format(htree, hf_smb_error_code, tvb,
			offset, 2, errcode, "Error Code: %s",
			decode_smb_error(errclass, errcode));
		offset += 2;
	}

	/
	offset = dissect_smb_flags(tvb, htree, offset);

	/
	offset = dissect_smb_flags2(tvb, htree, offset);

	/

	/
	proto_tree_add_item(htree, hf_smb_reserved, tvb, offset, 12, TRUE);
	offset += 12;

	/
	proto_tree_add_uint(htree, hf_smb_tid, tvb, offset, 2, si.tid);
	offset += 2;

	/
	proto_tree_add_uint(htree, hf_smb_pid, tvb, offset, 2, si.pid);
	offset += 2;

	/
	proto_tree_add_uint(htree, hf_smb_uid, tvb, offset, 2, si.uid);
	offset += 2;

	/
	proto_tree_add_uint(htree, hf_smb_mid, tvb, offset, 2, si.mid);
	offset += 2;

	pinfo->private_data = &si;
        dissect_smb_command(tvb, pinfo, offset, tree, si.cmd, TRUE);

	/
	if (!si.request && check_col(pinfo->cinfo, COL_INFO)) {
		if (flags2 & 0x4000) {
			/
			if (nt_status != 0) {
				/
				col_append_fstr(
					pinfo->cinfo, COL_INFO, ", Error: %s",
					val_to_str(nt_status, NT_errors,
					    "Unknown (0x%08X)"));
			}
		} else {
			/
			if (errclass != SMB_SUCCESS) {
				/
				col_append_fstr(
					pinfo->cinfo, COL_INFO, ", Error: %s",
					decode_smb_error(errclass, errcode));
			}
		}
	}

	return TRUE;
}
