gboolean
dissect_rpc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	int offset = 0;
	guint32	msg_type;
	rpc_call_info rpc_key;
	rpc_call_info *rpc_call = NULL;
	rpc_prog_info_value *rpc_prog = NULL;
	rpc_prog_info_key rpc_prog_key;

	unsigned int xid;
	unsigned int rpcvers;
	unsigned int prog = 0;
	unsigned int vers = 0;
	unsigned int proc = 0;
	unsigned int flavor = 0;
	unsigned int gss_proc = 0;
	unsigned int gss_svc = 0;
	int	proto = 0;
	int	ett = 0;

	unsigned int reply_state;
	unsigned int accept_state;
	unsigned int reject_state;

	char *msg_type_name = NULL;
	char *progname;
	char *procname = NULL;
	static char procname_static[20];

	unsigned int vers_low;
	unsigned int vers_high;

	unsigned int auth_state;

	proto_item *rpc_item=NULL;
	proto_tree *rpc_tree = NULL;

	proto_item *pitem=NULL;
	proto_tree *ptree = NULL;
	int offset_old = offset;

	int use_rm = 0;
	guint32 rpc_rm = 0;

	rpc_call_info	rpc_call_msg;
	rpc_proc_info_key	key;
	rpc_proc_info_value	*value = NULL;
	conversation_t* conversation;
	static address null_address = { AT_NONE, 0, NULL };

	dissect_function_t *dissect_function = NULL;

	if (!proto_is_protocol_enabled(proto_rpc))
	  return FALSE;

	pinfo->current_proto = "RPC";

	/
	use_rm = (pinfo->ptype == PT_TCP);

	/
	if (use_rm) {
		if (!tvb_bytes_exist(tvb, offset, 4))
			return FALSE;
		rpc_rm = tvb_get_ntohl(tvb, offset);
		offset += 4;
	}

	/
	if (!tvb_bytes_exist(tvb, offset, 8)) {
		/
		return FALSE;
	}

	/
	msg_type = tvb_get_ntohl(tvb, offset + 4);

	switch (msg_type) {

	case RPC_CALL:
		/
		if (!tvb_bytes_exist(tvb, offset, 16)) {
			/
			return FALSE;
		}

		/
		rpc_prog_key.prog = tvb_get_ntohl(tvb, offset + 12);
		if (tvb_get_ntohl(tvb, offset + 8) != 2 ||
		    ((rpc_prog = g_hash_table_lookup(rpc_progs, &rpc_prog_key))
		       == NULL)) {
			/
			return FALSE;
		}
		break;

	case RPC_REPLY:
		/
		conversation = find_conversation(&null_address, &pinfo->dst,
		    pinfo->ptype, pinfo->srcport, pinfo->destport, 0);
		if (conversation == NULL) {
			/
			return FALSE;
		}

		/
		rpc_key.xid = tvb_get_ntohl(tvb, offset + 0);
		rpc_key.conversation = conversation;
		if ((rpc_call = rpc_call_lookup(&rpc_key)) == NULL) {
			/
			return FALSE;
		}
		break;

	default:
		/
		return FALSE;
	}

	if (check_col(pinfo->fd, COL_PROTOCOL))
		col_set_str(pinfo->fd, COL_PROTOCOL, "RPC");

	if (tree) {
		rpc_item = proto_tree_add_item(tree, proto_rpc, tvb, 0,
				tvb_length(tvb), FALSE);
		if (rpc_item) {
			rpc_tree = proto_item_add_subtree(rpc_item, ett_rpc);
		}
	}

	if (use_rm && rpc_tree) {
		proto_tree_add_boolean(rpc_tree,hf_rpc_lastfrag, tvb,
			offset-4, 4, (rpc_rm >> 31) & 0x1);
		proto_tree_add_uint(rpc_tree,hf_rpc_fraglen, tvb,
			offset-4, 4, rpc_rm & RPC_RM_FRAGLEN);
	}

	xid      = tvb_get_ntohl(tvb, offset + 0);
	if (rpc_tree) {
		proto_tree_add_uint_format(rpc_tree,hf_rpc_xid, tvb,
			offset+0, 4, xid, "XID: 0x%x (%u)", xid, xid);
	}

	msg_type_name = val_to_str(msg_type,rpc_msg_type,"%u");
	if (rpc_tree) {
		proto_tree_add_uint(rpc_tree, hf_rpc_msgtype, tvb,
			offset+4, 4, msg_type);
	}

	offset += 8;

	if (msg_type==RPC_CALL) {
		/
		proto = rpc_prog->proto;
		ett = rpc_prog->ett;
		progname = rpc_prog->progname;

		rpcvers = tvb_get_ntohl(tvb, offset + 0);
		if (rpc_tree) {
			proto_tree_add_uint(rpc_tree,
				hf_rpc_version, tvb, offset+0, 4, rpcvers);
		}

		prog = tvb_get_ntohl(tvb, offset + 4);
		
		if (rpc_tree) {
			proto_tree_add_uint_format(rpc_tree,
				hf_rpc_program, tvb, offset+4, 4, prog,
				"Program: %s (%u)", progname, prog);
		}
		
		if (check_col(pinfo->fd, COL_PROTOCOL)) {
			/
			col_add_fstr(pinfo->fd, COL_PROTOCOL, "%s", progname);
		}

		if (!tvb_bytes_exist(tvb, offset+8,4))
			return TRUE;
		vers = tvb_get_ntohl(tvb, offset+8);
		if (rpc_tree) {
			proto_tree_add_uint(rpc_tree,
				hf_rpc_programversion, tvb, offset+8, 4, vers);
		}

		if (!tvb_bytes_exist(tvb, offset+12,4))
			return TRUE;
		proc = tvb_get_ntohl(tvb, offset+12);

		/
		if (proc == 0 && tvb_bytes_exist(tvb, offset+16,28)) {
			flavor = tvb_get_ntohl(tvb, offset+16);
			if (flavor == RPCSEC_GSS) {
				gss_proc = tvb_get_ntohl(tvb, offset+28);
				gss_svc = tvb_get_ntohl(tvb, offset+34);
			}
		}
		key.prog = prog;
		key.vers = vers;
		key.proc = proc;

		if ((value = g_hash_table_lookup(rpc_procs,&key)) != NULL) {
			dissect_function = value->dissect_call;
			procname = value->name;
		}
		else {
			/
			dissect_function = NULL;
			sprintf(procname_static, "proc-%u", proc);
			procname = procname_static;
		}
		
		if (rpc_tree) {
			proto_tree_add_uint_format(rpc_tree,
				hf_rpc_procedure, tvb, offset+12, 4, proc,
				"Procedure: %s (%u)", procname, proc);
		}

		if (check_col(pinfo->fd, COL_INFO)) {
			col_add_fstr(pinfo->fd, COL_INFO,"V%u %s %s XID 0x%x",
				vers,
				procname,
				msg_type_name,
				xid);
		}

		/
		conversation = find_conversation(&pinfo->src, &null_address,
		    pinfo->ptype, pinfo->srcport, pinfo->destport, 0);
		if (conversation == NULL) {
			/
			conversation = conversation_new(&pinfo->src,
			    &null_address, pinfo->ptype, pinfo->srcport,
			    pinfo->destport, NULL, 0);
		}

		/
		rpc_call_msg.xid = xid;
		rpc_call_msg.conversation = conversation;

		/
		if ((rpc_call = rpc_call_lookup(&rpc_call_msg)) != NULL) {
			/
			if (pinfo->fd->num != rpc_call->req_num) {
				/
				if (check_col(pinfo->fd, COL_INFO)) {
					col_append_fstr(pinfo->fd, COL_INFO,
						" dup XID 0x%x", xid);
					if (rpc_tree) {
						proto_tree_add_uint_hidden(rpc_tree,
							hf_rpc_dup, tvb, 0,0, xid);
						proto_tree_add_uint_hidden(rpc_tree,
							hf_rpc_call_dup, tvb, 0,0, xid);
					}
				}
			}
		}
		else {
			/
			rpc_call_msg.req_num = pinfo->fd->num;
			rpc_call_msg.rep_num = 0;
			rpc_call_msg.prog = prog;
			rpc_call_msg.vers = vers;
			rpc_call_msg.proc = proc;
			rpc_call_msg.flavor = flavor;
			rpc_call_msg.gss_proc = gss_proc;
			rpc_call_msg.gss_svc = gss_svc;
			rpc_call_msg.proc_info = value;
			/
			rpc_call_insert(&rpc_call_msg);
		}

		offset += 16;

		offset = dissect_rpc_cred(tvb, pinfo, rpc_tree, offset);
		offset = dissect_rpc_verf(tvb, pinfo, rpc_tree, offset);

		/

	} /
	else if (msg_type == RPC_REPLY)
	{
		/
		prog = rpc_call->prog;
		vers = rpc_call->vers;
		proc = rpc_call->proc;
		flavor = rpc_call->flavor;
		gss_proc = rpc_call->gss_proc;
		gss_svc = rpc_call->gss_svc;

		/
		proto_tree_add_text(rpc_tree, tvb, 0, 0,
		    "This is a reply to a request starting in frame %u",
		    rpc_call->req_num);

		if (rpc_call->proc_info != NULL) {
			dissect_function = rpc_call->proc_info->dissect_reply;
			if (rpc_call->proc_info->name != NULL) {
				procname = rpc_call->proc_info->name;
			}
			else {
				sprintf(procname_static, "proc-%u", proc);
				procname = procname_static;
			}
		}
		else {
			dissect_function = NULL;
			sprintf(procname_static, "proc-%u", proc);
			procname = procname_static;
		}

		rpc_prog_key.prog = prog;
		if ((rpc_prog = g_hash_table_lookup(rpc_progs,&rpc_prog_key)) == NULL) {
			proto = 0;
			ett = 0;
			progname = "Unknown";
		}
		else {
			proto = rpc_prog->proto;
			ett = rpc_prog->ett;
			progname = rpc_prog->progname;

			if (check_col(pinfo->fd, COL_PROTOCOL)) {
				/
				col_add_fstr(pinfo->fd, COL_PROTOCOL, "%s",
				    progname);
			}
		}

		if (check_col(pinfo->fd, COL_INFO)) {
			col_add_fstr(pinfo->fd, COL_INFO,"V%u %s %s XID 0x%x",
				vers,
				procname,
				msg_type_name,
				xid);
		}

		if (rpc_tree) {
			proto_tree_add_uint_format(rpc_tree,
				hf_rpc_program, tvb, 0, 0, prog,
				"Program: %s (%u)", progname, prog);
			proto_tree_add_uint(rpc_tree,
				hf_rpc_programversion, tvb, 0, 0, vers);
			proto_tree_add_uint_format(rpc_tree,
				hf_rpc_procedure, tvb, 0, 0, proc,
				"Procedure: %s (%u)", procname, proc);
		}

		if (rpc_call->rep_num == 0) {
			/
			rpc_call->rep_num = pinfo->fd->num;
		} else {
			/
			if (rpc_call->rep_num != pinfo->fd->num) {
				/
				if (check_col(pinfo->fd, COL_INFO)) {
					col_append_fstr(pinfo->fd, COL_INFO,
						" dup XID 0x%x", xid);
					if (rpc_tree) {
						proto_tree_add_uint_hidden(rpc_tree,
							hf_rpc_dup, tvb, 0,0, xid);
						proto_tree_add_uint_hidden(rpc_tree,
							hf_rpc_reply_dup, tvb, 0,0, xid);
					}
				}
			}
		}

		if (!tvb_bytes_exist(tvb, offset,4))
			return TRUE;
		reply_state = tvb_get_ntohl(tvb,offset+0);
		if (rpc_tree) {
			proto_tree_add_uint(rpc_tree, hf_rpc_state_reply, tvb,
				offset+0, 4, reply_state);
		}
		offset += 4;

		if (reply_state == MSG_ACCEPTED) {
			offset = dissect_rpc_verf(tvb, pinfo, rpc_tree, offset);
			if (!tvb_bytes_exist(tvb, offset,4))
				return TRUE;
			accept_state = tvb_get_ntohl(tvb,offset+0);
			if (rpc_tree) {
				proto_tree_add_uint(rpc_tree, hf_rpc_state_accept, tvb,
					offset+0, 4, accept_state);
			}
			offset += 4;
			switch (accept_state) {
				case SUCCESS:
					/
				break;
				case PROG_MISMATCH:
					if (!tvb_bytes_exist(tvb,offset,8))
						return TRUE;
					vers_low = tvb_get_ntohl(tvb,offset+0);
					vers_high = tvb_get_ntohl(tvb,offset+4);
					if (rpc_tree) {
						proto_tree_add_uint(rpc_tree,
							hf_rpc_programversion_min,
							tvb, offset+0, 4, vers_low);
						proto_tree_add_uint(rpc_tree,
							hf_rpc_programversion_max,
							tvb, offset+4, 4, vers_high);
					}
					offset += 8;
				break;
				default:
					/
				break;
			}
		} else if (reply_state == MSG_DENIED) {
			if (!tvb_bytes_exist(tvb,offset,4))
				return TRUE;
			reject_state = tvb_get_ntohl(tvb,offset+0);
			if (rpc_tree) {
				proto_tree_add_uint(rpc_tree,
					hf_rpc_state_reject, tvb, offset+0, 4,
					reject_state);
			}
			offset += 4;

			if (reject_state==RPC_MISMATCH) {
				if (!tvb_bytes_exist(tvb,offset,8))
					return TRUE;
				vers_low = tvb_get_ntohl(tvb,offset+0);
				vers_high = tvb_get_ntohl(tvb,offset+4);
				if (rpc_tree) {
					proto_tree_add_uint(rpc_tree,
						hf_rpc_version_min,
						tvb, offset+0, 4, vers_low);
					proto_tree_add_uint(rpc_tree,
						hf_rpc_version_max,
						tvb, offset+4, 4, vers_high);
				}
				offset += 8;
			} else if (reject_state==AUTH_ERROR) {
				if (!tvb_bytes_exist(tvb,offset,4))
					return TRUE;
				auth_state = tvb_get_ntohl(tvb,offset+0);
				if (rpc_tree) {
					proto_tree_add_uint(rpc_tree,
						hf_rpc_state_auth, tvb, offset+0, 4,
						auth_state);
				}
				offset += 4;
			}
		} 
	} /

	/
	if (rpc_item) {
		proto_item_set_len(rpc_item, offset - offset_old);
	}

	/
	if (tree) {
		pitem = proto_tree_add_item(tree, proto, tvb,
				offset, tvb_length(tvb) - offset, FALSE);
		if (pitem) {
			ptree = proto_item_add_subtree(pitem, ett);
		}

		if (ptree) {
			proto_tree_add_uint(ptree,
				hf_rpc_programversion, NullTVB, 0, 0, vers);
			proto_tree_add_uint_format(ptree,
				hf_rpc_procedure, NullTVB, 0, 0, proc,
				"Procedure: %s (%u)", procname, proc);
		}
	}

	if (!proto_is_protocol_enabled(proto)) {
		dissect_function = NULL;
	}

	/
	if (flavor == RPCSEC_GSS) {
		switch (gss_proc) {
		case RPCSEC_GSS_INIT:
		case RPCSEC_GSS_CONTINUE_INIT:
			if (msg_type == RPC_CALL) {
				offset = dissect_rpc_authgss_initarg(tvb,
					pinfo, ptree, offset);
			}
			else {
				offset = dissect_rpc_authgss_initres(tvb,
					pinfo, ptree, offset);
			}
			break;
		case RPCSEC_GSS_DATA:
			if (gss_svc == RPCSEC_GSS_SVC_NONE) {
				offset = call_dissect_function(tvb, 
						pinfo, ptree, offset, 
						dissect_function);
			}
			else if (gss_svc == RPCSEC_GSS_SVC_INTEGRITY) {
				offset = dissect_rpc_authgss_integ_data(tvb,
						pinfo, ptree, offset,
						dissect_function);
			}
			else if (gss_svc == RPCSEC_GSS_SVC_PRIVACY) {
				offset = dissect_rpc_authgss_priv_data(tvb,
						pinfo, ptree, offset);
			}
			break;
		default:
			dissect_function = NULL;
			break;
		}
	}
	else {
		offset=call_dissect_function(tvb, pinfo, ptree, offset,
				dissect_function);
	}

	/
	dissect_data(tvb, offset, pinfo, ptree);

	return TRUE;
}
