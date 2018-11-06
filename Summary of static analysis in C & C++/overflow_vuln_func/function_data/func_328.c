static int
dissect_nfs4_response_op(tvbuff_t *tvb, int offset, packet_info *pinfo, proto_tree *tree)
{
	guint highest_tier=5;
	guint current_tier=5;
	guint first_operation=1;
	guint16 sid_hash=0;
	guint32 ops, ops_counter, summary_counter, opcode, status, last_fh_hash=0;
	const char *opname=NULL;
	nfs4_operation_summary *op_summary;
	proto_item *fitem = NULL;
	proto_tree *ftree = NULL;
	proto_tree *newftree = NULL;

	ops = tvb_get_ntohl(tvb, offset+0);

	fitem = proto_tree_add_uint_format(tree, hf_nfs4_ops_count, tvb, offset+0, 4, ops,
		"Operations (count: %u)", ops);
	offset += 4;

	if (ops > MAX_NFSV4_OPS) {
		expert_add_info_format(pinfo, fitem, PI_MALFORMED, PI_NOTE, "Too many operations");
		ops = MAX_NFSV4_OPS;
	}

	op_summary = (nfs4_operation_summary *)g_malloc0(sizeof(nfs4_operation_summary) * ops);

	if (fitem) {
		ftree = proto_item_add_subtree(fitem, ett_nfs4_response_op);
	}

	proto_item_append_text(tree, ", Ops(%d):", ops);

	for (ops_counter = 0; ops_counter < ops; ops_counter++)
	{
		op_summary[ops_counter].optext = g_string_new("");
		opcode = tvb_get_ntohl(tvb, offset);
		op_summary[ops_counter].iserror=FALSE;
		op_summary[ops_counter].opcode = opcode;

		/
		if ((opcode < NFS4_OP_ACCESS || opcode > NFS4_OP_RECLAIM_COMPLETE) &&
			(opcode != NFS4_OP_ILLEGAL))
			break;

		fitem = proto_tree_add_uint(ftree, hf_nfs4_op, tvb, offset, 4, opcode);

		/
		if (opcode == NFS4_OP_ILLEGAL) {
			newftree = proto_item_add_subtree(fitem, ett_nfs4_illegal);
		} else if (nfs4_operation_ett[opcode - 3]) {
			newftree = proto_item_add_subtree(fitem, *nfs4_operation_ett[opcode - 3]);
		} else {
			break;
		}

		opname = val_to_str_ext_const(opcode, &names_nfs4_operation_ext, "Unknown");
		offset += 4;
		g_string_append_printf (op_summary[ops_counter].optext, "%s", opname);

		offset = dissect_nfs4_status(tvb, offset, newftree, &status);
		if (status != NFS4_OK) {
			proto_item_append_text(tree, " %s(%s)", opname,
				val_to_str_ext(status, &names_nfs4_status_ext, "Unknown error: %u"));
		} else {
			proto_item_append_text(tree, " %s", opname);
		}

		/
		if(status != NFS4_OK 
		&& opcode != NFS4_OP_LOCK 
		&& opcode != NFS4_OP_LOCKT 
		&& opcode != NFS4_OP_SETATTR) {
			op_summary[ops_counter].iserror=TRUE;
			continue;
		}

		/
		switch(opcode)
		{
		case NFS4_OP_ACCESS:
			offset = dissect_access_reply(tvb, offset, pinfo, fitem, 4,	op_summary[ops_counter].optext);
			break;

		case NFS4_OP_CLOSE:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			break;

		case NFS4_OP_COMMIT:
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_verifier, offset);
			break;

		case NFS4_OP_CREATE:
			offset = dissect_nfs4_change_info(tvb, offset, newftree, "change_info");
			offset = dissect_nfs4_fattrs(tvb, offset, pinfo, newftree, FATTR4_DISSECT_VALUES);
			break;

		case NFS4_OP_GETATTR:
			offset = dissect_nfs4_fattrs(tvb, offset, pinfo, newftree, FATTR4_DISSECT_VALUES);
			break;

		case NFS4_OP_GETFH:
			offset = dissect_nfs4_fh(tvb, offset, pinfo, newftree, "Filehandle", &last_fh_hash);
			break;

		case NFS4_OP_LINK:
			offset = dissect_nfs4_change_info(tvb, offset, newftree, "change_info");
			break;

		case NFS4_OP_LOCK:
		case NFS4_OP_LOCKT:
			if (status == NFS4_OK)
			{
				if (opcode == NFS4_OP_LOCK)
					offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			}
			else
			if (status == NFS4ERR_DENIED)
				offset = dissect_nfs4_lockdenied(tvb, offset, newftree);
			break;

		case NFS4_OP_LOCKU:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			break;

		case NFS4_OP_OPEN:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, &sid_hash);
			offset = dissect_nfs4_change_info(tvb, offset, newftree,
				"change_info");
			offset = dissect_nfs4_open_rflags(tvb, offset, newftree);
			offset = dissect_nfs4_fattrs(tvb, offset, pinfo, newftree, FATTR4_DISSECT_VALUES);
			offset = dissect_nfs4_open_delegation(tvb, offset, pinfo, newftree);
			g_string_append_printf (op_summary[ops_counter].optext, " StateID: 0x%04x", sid_hash);
			break;

		case NFS4_OP_OPEN_CONFIRM:
		case NFS4_OP_OPEN_DOWNGRADE:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			break;

		case NFS4_OP_RESTOREFH:
		case NFS4_OP_SAVEFH:
		case NFS4_OP_PUTFH:
			break;

		case NFS4_OP_READ:
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_eof, offset);
			dissect_rpc_uint32(tvb, newftree, hf_nfs4_read_data_length, offset); /
			offset = dissect_nfsdata(tvb, offset, newftree, hf_nfs_data);
			break;

		case NFS4_OP_READDIR:
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_verifier, offset);
			offset = dissect_nfs4_dirlist(tvb, offset, pinfo, newftree);
			break;

		case NFS4_OP_READLINK:
			offset = dissect_nfs_utf8string(tvb, offset, newftree, hf_nfs4_linktext, NULL);
			break;

		case NFS4_OP_RECLAIM_COMPLETE:
			break;

		case NFS4_OP_REMOVE:
			offset = dissect_nfs4_change_info(tvb, offset, newftree, "change_info");
			break;

		case NFS4_OP_RENAME:
			offset = dissect_nfs4_change_info(tvb, offset, newftree, "source_cinfo");
			offset = dissect_nfs4_change_info(tvb, offset, newftree, "target_cinfo");
			break;

		case NFS4_OP_SECINFO:
		case NFS4_OP_SECINFO_NO_NAME:
			offset = dissect_rpc_array(tvb, pinfo, newftree, offset,
				dissect_nfs4_secinfo_res, hf_nfs4_secinfo_arr);
			break;

		case NFS4_OP_SETATTR:
			offset = dissect_nfs4_fattrs(tvb, offset, pinfo, newftree, FATTR4_DISSECT_VALUES);
			break;

		case NFS4_OP_SETCLIENTID:
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_clientid,
				offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_verifier,
				offset);
			break;

		case NFS4_OP_WRITE:
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_count, offset);
			offset = dissect_nfs4_stable_how(tvb, offset, newftree,	"committed");
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_verifier, offset);
			break;

			/
		case NFS4_OP_BIND_CONN_TO_SESSION:
			offset = dissect_nfs4_sessionid(tvb, offset, newftree);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_bctsr_dir, offset);
			offset = dissect_rpc_bool(tvb, newftree, hf_nfs4_bctsr_use_conn_in_rdma_mode, offset);
			break;

		case NFS4_OP_EXCHANGE_ID: {
				proto_tree *eir_flags_tree = NULL;
				proto_tree *eir_server_owner_tree = NULL;

				offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_clientid, offset);
				offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_seqid, offset);

				fitem = proto_tree_add_item(newftree, hf_nfs4_exchid_reply_flags, tvb, offset, 4,
					ENC_BIG_ENDIAN);
				eir_flags_tree = proto_item_add_subtree(fitem, ett_nfs4_exchangeid_reply_flags);
				proto_tree_add_item(eir_flags_tree, hf_nfs4_exchid_flags_confirmed_r, tvb, offset, 4,
					ENC_BIG_ENDIAN);
				proto_tree_add_item(eir_flags_tree, hf_nfs4_exchid_flags_upd_conf_rec_a, tvb, offset, 4,
					ENC_BIG_ENDIAN);
				proto_tree_add_item(eir_flags_tree, hf_nfs4_exchid_flags_pnfs_ds, tvb, offset, 4,
					ENC_BIG_ENDIAN);
				proto_tree_add_item(eir_flags_tree, hf_nfs4_exchid_flags_pnfs_mds, tvb, offset, 4,
					ENC_BIG_ENDIAN);
				proto_tree_add_item(eir_flags_tree, hf_nfs4_exchid_flags_non_pnfs, tvb, offset, 4,
					ENC_BIG_ENDIAN);
				proto_tree_add_item(eir_flags_tree, hf_nfs4_exchid_flags_bind_princ, tvb, offset, 4,
					ENC_BIG_ENDIAN);
				proto_tree_add_item(eir_flags_tree, hf_nfs4_exchid_flags_moved_migr, tvb, offset, 4,
					ENC_BIG_ENDIAN);
				proto_tree_add_item(eir_flags_tree, hf_nfs4_exchid_flags_moved_refer, tvb, offset, 4,
					ENC_BIG_ENDIAN);
				offset += 4;

				offset = dissect_nfs4_state_protect_r(tvb, offset, newftree);

				fitem = proto_tree_add_text(newftree, tvb, offset, 0, "eir_server_owner");
				eir_server_owner_tree = proto_item_add_subtree(fitem, ett_nfs4_server_owner);
				offset = dissect_rpc_serverowner4(tvb, offset, eir_server_owner_tree);
				offset = dissect_nfsdata(tvb, offset, newftree, hf_nfs4_serverscope4);
				offset = dissect_rpc_nfs_impl_id4(tvb, offset, newftree, "eir_server_impl_id");
			}
			break;
		case NFS4_OP_CREATE_SESSION:
			offset = dissect_nfs4_sessionid(tvb, offset, newftree);
			offset = dissect_rpc_uint32(tvb, newftree,
					hf_nfs4_seqid, offset);
			offset = dissect_nfs_create_session_flags(tvb, offset, newftree,
				hf_nfs4_create_session_flags_csr);
			offset = dissect_rpc_chanattrs4(tvb, offset, newftree, "csr_fore_chan_attrs");
			offset = dissect_rpc_chanattrs4(tvb, offset, newftree, "csr_back_chan_attrs");
			break;

		case NFS4_OP_DESTROY_SESSION:
			break;
		case NFS4_OP_FREE_STATEID:
			break;
		case NFS4_OP_TEST_STATEID:
			offset = dissect_rpc_array(tvb, pinfo, newftree, offset, dissect_nfs4_test_stateid_res, hf_nfs4_test_stateid_res);
			break;

		case NFS4_OP_LAYOUTGET:
			offset = dissect_rpc_bool(tvb, newftree, hf_nfs4_return_on_close,
									  offset);
			offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			offset = dissect_nfs4_layout(tvb, offset, pinfo, newftree);
			break;

		case NFS4_OP_LAYOUTCOMMIT:
			offset = dissect_nfs4_newsize(tvb, offset, newftree);
			break;

		case NFS4_OP_LAYOUTRETURN:
			offset = dissect_nfs_layoutreturn_stateid(tvb, newftree, offset);
			break;

		case NFS4_OP_GETDEVINFO:
			offset = dissect_nfs4_deviceaddr(tvb, offset, newftree);
			offset = dissect_nfs4_notification_bitmap(tvb, newftree, offset);
			break;

		case NFS4_OP_GETDEVLIST:
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_cookie, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_cookie_verf,	offset);
			offset = dissect_nfs4_devicelist(tvb, offset, newftree);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_eof, offset);
			break;

		case NFS4_OP_SEQUENCE:
			{
			proto_tree* sf_tree = NULL;

			offset = dissect_nfs4_sessionid(tvb, offset, newftree);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_seqid, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_slotid, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_high_slotid, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_target_high_slotid, offset);
			fitem = proto_tree_add_item(newftree, hf_nfs4_sequence_status_flags,
						tvb, offset, 4,	ENC_BIG_ENDIAN);
			sf_tree = proto_item_add_subtree(fitem, ett_nfs4_sequence_status_flags);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_cb_path_down,
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_cb_gss_contexts_expiring,
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_cb_gss_contexts_expired,
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_expired_all_state_revoked,
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_expired_some_state_revoked,
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_admin_state_revoked, 
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_recallable_state_revoked,
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_lease_moved, 
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_restart_reclaim_needed, 
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_cb_path_down_session,
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_backchannel_fault, 
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_devid_changed, 
				tvb, offset, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(sf_tree, hf_nfs4_sequence_status_flags_devid_deleted, 
				tvb, offset, 4, ENC_BIG_ENDIAN);
			offset += 4;
			}
			break;

		default:
			break;
		}
	}

	/
	for (summary_counter=0; summary_counter < ops_counter; summary_counter++)
	{
		current_tier = nfs4_operation_tiers[op_summary[summary_counter].opcode];
		if (current_tier < highest_tier)
			highest_tier=current_tier;
	}

	/
	for (summary_counter=0; summary_counter < ops_counter; summary_counter++)
	{
		guint main_opcode;
		proto_item *main_op_item = NULL;

		main_opcode=op_summary[summary_counter].opcode;
		current_tier = nfs4_operation_tiers[op_summary[summary_counter].opcode];

		/
		if (current_tier == highest_tier 
		|| !display_major_nfs4_ops 
		|| op_summary[summary_counter].iserror==TRUE) 
		{
			if (current_tier == highest_tier) {
				const char *main_opname=NULL;

				/
				main_opname = val_to_str_ext_const(main_opcode, &names_nfs4_operation_ext, "Unknown");
				main_op_item = proto_tree_add_uint_format_value(tree, hf_nfs4_main_opcode, tvb, 0, 0,
									main_opcode, "%s (%u)", main_opname, main_opcode);
				PROTO_ITEM_SET_GENERATED(main_op_item);
			}

			if (first_operation==0)
				/
				col_append_fstr(pinfo->cinfo, COL_INFO, " |");

			if (op_summary[summary_counter].optext->len > 0)
				col_append_fstr(pinfo->cinfo, COL_INFO, " %s", 
					op_summary[summary_counter].optext->str);
			first_operation=0;
		}
	}

	for (ops_counter=0; ops_counter<ops; ops_counter++)
	{
		if (op_summary[ops_counter].optext)
			g_string_free(op_summary[ops_counter].optext, TRUE);
	}

	g_free(op_summary);

	return offset;
}
