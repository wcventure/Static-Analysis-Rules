static int
dissect_nfs4_request_op(tvbuff_t *tvb, int offset, packet_info *pinfo, proto_tree *tree)
{
	const char *name = NULL, *source_name = NULL, *dest_name=NULL;
	const char *opname=NULL;
	int cbprog;
	guint opcode, highest_tier=5, current_tier=5, first_operation=1;
	/
	guint8 *clientid_array;
	guint16 sid_hash, clientid_hash=0;
	guint32 ops, ops_counter, summary_counter, string_length, last_fh_hash=0, saved_fh_hash=0, length;
	guint64 lock_length, file_offset;
	proto_item *fitem = NULL;
	proto_tree *ftree = NULL;
	proto_tree *newftree = NULL;
	nfs4_operation_summary *op_summary;

	ops = tvb_get_ntohl(tvb, offset+0);

	fitem = proto_tree_add_uint_format(tree, hf_nfs4_ops_count, tvb, offset+0, 4, ops,
		"Operations (count: %u)", ops);
	offset += 4;

#define MAX_NFSV4_OPS 128
	if (ops > MAX_NFSV4_OPS) {
		/
		expert_add_info_format(pinfo, fitem, PI_MALFORMED, PI_NOTE, "Too many operations");
		ops = MAX_NFSV4_OPS;
	}

	op_summary = (nfs4_operation_summary *)g_malloc0(sizeof(nfs4_operation_summary) * ops);

	if (fitem) {
		ftree = proto_item_add_subtree(fitem, ett_nfs4_request_op);
	}

	if (ops)
		proto_item_append_text(proto_tree_get_parent(tree), ", Ops(%d):", ops);

	for (ops_counter=0; ops_counter<ops; ops_counter++)
	{
		op_summary[ops_counter].optext = g_string_new("");
		opcode = tvb_get_ntohl(tvb, offset);
		op_summary[ops_counter].opcode = opcode;

		fitem = proto_tree_add_uint(ftree, hf_nfs4_op, tvb, offset, 4, opcode);

		/
		if((opcode < NFS4_OP_ACCESS || opcode > NFS4_OP_RECLAIM_COMPLETE) 
		&&  opcode != NFS4_OP_ILLEGAL)
			break;

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
		g_string_printf (op_summary[ops_counter].optext, "%s",
			val_to_str_ext_const(opcode, &names_nfs4_operation_ext, "Unknown"));

		proto_item_append_text(proto_tree_get_parent(tree),
			"%s%s", ops_counter ? ", " : " ", opname);
		proto_item_append_text(proto_tree_get_parent(ftree),
			"%s%s", ops_counter ? ", " : ": ", opname);

		switch(opcode)
		{
		case NFS4_OP_ACCESS:
			{
				guint32 *acc_request, amask;
				rpc_call_info_value *civ;

				/
				amask = tvb_get_ntohl(tvb, offset);
				acc_request = (guint32 *)se_memdup( &amask, sizeof(guint32));
				civ = (rpc_call_info_value *)pinfo->private_data;
				civ->private_data = acc_request;

				g_string_append_printf (op_summary[ops_counter].optext, " FH: 0x%08x", last_fh_hash);
				display_access_items(tvb, offset, pinfo, fitem, amask, 'C', 4,
					op_summary[ops_counter].optext, "Check") ;
				offset+=4;
			}
			break;

		case NFS4_OP_CLOSE:
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_seqid, offset);
			offset = dissect_nfs4_stateid(tvb, offset, newftree, &sid_hash);
			g_string_append_printf (op_summary[ops_counter].optext, " StateID: 0x%04x", sid_hash);
			break;

		case NFS4_OP_COMMIT:
			file_offset = tvb_get_ntoh64(tvb, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_offset, offset);
			length = tvb_get_ntohl(tvb, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_count, offset);
			g_string_append_printf (op_summary[ops_counter].optext,
				" FH: 0x%08x Offset: %"G_GINT64_MODIFIER"u Len: %u",
				last_fh_hash, file_offset, length);

			break;

		case NFS4_OP_CREATE:
			{
				guint create_type;

				create_type = tvb_get_ntohl(tvb, offset);
				offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_ftype, offset);

				switch(create_type)
				{
				case NF4LNK:
					offset = dissect_nfs_utf8string(tvb, offset, newftree, hf_nfs4_linktext, NULL);
					break;

				case NF4BLK:
				case NF4CHR:
					offset = dissect_nfs4_specdata(tvb, offset, newftree);
					break;

				case NF4SOCK:
				case NF4FIFO:
				case NF4DIR:
					break;

				default:
					break;
				}

				offset = dissect_nfs_utf8string(tvb, offset, newftree, hf_nfs4_component, NULL);
				offset = dissect_nfs4_fattrs(tvb, offset, pinfo, newftree, FATTR4_DISSECT_VALUES);
			}
			break;

		case NFS4_OP_DELEGPURGE:
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_clientid, offset);
			break;

		case NFS4_OP_DELEGRETURN:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, &sid_hash);
			g_string_append_printf (op_summary[ops_counter].optext, " StateID: 0x%04x", sid_hash);
			break;

		case NFS4_OP_GETATTR:
			offset = dissect_nfs4_fattrs(tvb, offset, pinfo, newftree, FATTR4_BITMAP_ONLY);

			if (last_fh_hash != 0)
				g_string_append_printf (op_summary[ops_counter].optext, " FH: 0x%08x", last_fh_hash);

			break;

		case NFS4_OP_GETFH:
			last_fh_hash=0;
			break;

		case NFS4_OP_LINK:
			offset = dissect_nfs_utf8string(tvb, offset, newftree, hf_nfs4_component, NULL);
			break;

		case NFS4_OP_LOCK:
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_lock_type, offset);
			offset = dissect_rpc_bool(tvb, newftree, hf_nfs4_lock_reclaim, offset);
			file_offset = tvb_get_ntoh64(tvb, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_offset, offset);
			lock_length = tvb_get_ntoh64(tvb, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_length, offset);
			offset = dissect_nfs4_locker(tvb, offset, newftree);
			if (lock_length == G_GINT64_CONSTANT(0xffffffffffffffffU))
				g_string_append_printf (op_summary[ops_counter].optext,
					" FH: 0x%08x Offset: %"G_GINT64_MODIFIER"u Length: <End of File>",
					last_fh_hash, file_offset);
			else 
				g_string_append_printf (op_summary[ops_counter].optext,
					" FH: 0x%08x Offset: %"G_GINT64_MODIFIER"u Length: %"G_GINT64_MODIFIER"u ",
					last_fh_hash, file_offset, lock_length);
			break;

		case NFS4_OP_LOCKT:
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_lock_type, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_offset, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_length, offset);
			offset = dissect_nfs4_lock_owner(tvb, offset, newftree);
			break;

		case NFS4_OP_LOCKU:
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_lock_type, offset);
			offset = dissect_rpc_uint32(tvb, tree, hf_nfs4_seqid, offset);
			offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			file_offset = tvb_get_ntoh64(tvb, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_offset, offset);
			lock_length = tvb_get_ntoh64(tvb, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_length, offset);
			if (lock_length == G_GINT64_CONSTANT(0xffffffffffffffffU))
				g_string_append_printf (op_summary[ops_counter].optext,
					" FH: 0x%08x Offset: %"G_GINT64_MODIFIER"u Length: <End of File>",
					last_fh_hash,file_offset);
			else 
				g_string_append_printf (op_summary[ops_counter].optext,
					" FH: 0x%08x Offset: %"G_GINT64_MODIFIER"u Length: %"G_GINT64_MODIFIER"u ",
					last_fh_hash,file_offset,lock_length);
			break;

		case NFS4_OP_LOOKUP:
			/
			offset = dissect_nfs_utf8string(tvb, offset, newftree, hf_nfs4_component, &name);
			if (nfs_file_name_snooping){
				rpc_call_info_value *civ=(rpc_call_info_value *)pinfo->private_data;
				nfs_name_snoop_add_name(civ->xid, tvb,
										/
										0, 0,
										0, 0, name);
			}
			g_string_append_printf (op_summary[ops_counter].optext, " ");
			if (last_fh_hash != 0)
				g_string_append_printf (op_summary[ops_counter].optext, "DH: 0x%08x/", last_fh_hash);
			if (name != NULL)
				g_string_append_printf (op_summary[ops_counter].optext, "%s", name);
			break;

		case NFS4_OP_LOOKUPP:
			break;

		case NFS4_OP_NVERIFY:
			offset = dissect_nfs4_fattrs(tvb, offset, pinfo, newftree, FATTR4_DISSECT_VALUES);
			if (last_fh_hash != 0)
				g_string_append_printf (op_summary[ops_counter].optext, " FH: 0x%08x", last_fh_hash);
			break;

		case NFS4_OP_OPEN:
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_seqid, offset);
			offset = dissect_nfs4_open_share_access(tvb, offset, newftree);
			offset = dissect_nfs4_open_share_deny(tvb, offset, newftree);
			offset = dissect_nfs4_open_owner(tvb, offset, newftree);
			offset = dissect_nfs4_openflag(tvb, offset, pinfo, newftree);
			offset = dissect_nfs4_open_claim(tvb, offset, pinfo, newftree, &name);
			g_string_append_printf (op_summary[ops_counter].optext, " ");
			if (last_fh_hash != 0)
				g_string_append_printf (op_summary[ops_counter].optext, "DH: 0x%08x/", last_fh_hash);
			if (name != NULL)
				g_string_append_printf (op_summary[ops_counter].optext, "%s", name);
			break;

		case NFS4_OP_OPENATTR:
			offset = dissect_rpc_bool(tvb, newftree, hf_nfs4_attr_dir_create, offset);
			break;

		case NFS4_OP_OPEN_CONFIRM:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_seqid, offset);
			break;

		case NFS4_OP_OPEN_DOWNGRADE:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_seqid, offset);
			offset = dissect_nfs4_open_share_access(tvb, offset, newftree);
			offset = dissect_nfs4_open_share_deny(tvb, offset, newftree);
			break;

		case NFS4_OP_PUTFH:
			offset = dissect_nfs4_fh(tvb, offset, pinfo, newftree, "filehandle", &last_fh_hash);
			break;

		case NFS4_OP_PUTPUBFH:
		case NFS4_OP_PUTROOTFH:
			break;

		case NFS4_OP_READ:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, &sid_hash);
			file_offset = tvb_get_ntoh64(tvb, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_offset, offset);
			length = tvb_get_ntohl(tvb, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_count, offset);
			if (sid_hash != 0)
				g_string_append_printf (op_summary[ops_counter].optext, 
					" StateID: 0x%04x Offset: %" G_GINT64_MODIFIER "u Len: %u", 
					sid_hash, file_offset, length);
			break;

		case NFS4_OP_READDIR:
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_cookie, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_cookie_verf,	offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_count_dircount, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_count_maxcount, offset);
			offset = dissect_nfs4_fattrs(tvb, offset, pinfo, newftree, FATTR4_BITMAP_ONLY);
			if (last_fh_hash != 0)
				g_string_append_printf (op_summary[ops_counter].optext, " FH: 0x%08x", last_fh_hash);
			break;

		case NFS4_OP_READLINK:
			break;

		case NFS4_OP_TEST_STATEID:
			offset = dissect_rpc_array(tvb, pinfo, newftree, offset, dissect_nfs4_test_stateid_arg, hf_nfs4_test_stateid_arg);
			break;

		case NFS4_OP_DESTROY_CLIENTID:
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_clientid, offset);
			break;
		case NFS4_OP_RECLAIM_COMPLETE:
			offset = dissect_rpc_bool(tvb, newftree, hf_nfs4_reclaim_one_fs, offset);
			break;

		case NFS4_OP_REMOVE:
			offset = dissect_nfs_utf8string(tvb, offset, newftree, hf_nfs4_component, &name);
			g_string_append_printf (op_summary[ops_counter].optext, " ");
			if (last_fh_hash != 0)
				g_string_append_printf (op_summary[ops_counter].optext, "DH: 0x%08x/", last_fh_hash);
			if (name != NULL)
				g_string_append_printf (op_summary[ops_counter].optext, "%s", name);
			break;

		case NFS4_OP_RENAME:
			offset = dissect_nfs_utf8string(tvb, offset, newftree, hf_nfs4_component, &source_name);
			offset = dissect_nfs_utf8string(tvb, offset, newftree, hf_nfs4_component, &dest_name);
			g_string_append_printf (op_summary[ops_counter].optext, " From: %s To: %s",
				source_name ? source_name : "Unknown", dest_name ? dest_name : "Unknown");
			break;

		case NFS4_OP_RENEW:
			clientid_array = tvb_get_ephemeral_string(tvb, offset, 8);
			clientid_hash = crc16_ccitt(clientid_array, 8);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_clientid, offset);
			g_string_append_printf (op_summary[ops_counter].optext, " CID: 0x%04x", clientid_hash);

			break;

		case NFS4_OP_RESTOREFH:
			last_fh_hash = saved_fh_hash;
			break;

		case NFS4_OP_SAVEFH:
			saved_fh_hash = last_fh_hash;
			break;

		case NFS4_OP_SECINFO:
			offset = dissect_nfs_utf8string(tvb, offset, newftree,
				hf_nfs4_component, NULL);
			break;

		case NFS4_OP_SECINFO_NO_NAME:
			proto_tree_add_item(newftree, hf_nfs4_secinfo_style, tvb, offset, 4, ENC_BIG_ENDIAN);
			offset += 4;
			break;

		case NFS4_OP_SETATTR:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			offset = dissect_nfs4_fattrs(tvb, offset, pinfo, newftree, FATTR4_DISSECT_VALUES);
			g_string_append_printf (op_summary[ops_counter].optext, " FH: 0x%08x", last_fh_hash);
			break;

		case NFS4_OP_SETCLIENTID:
			{
				proto_tree *client_tree = NULL;
				proto_tree *callback_tree = NULL;

				fitem = proto_tree_add_text(newftree, tvb, offset, 0, "client");

				client_tree = proto_item_add_subtree(fitem, ett_nfs4_client_id);

				offset = dissect_nfs4_client_id(tvb, offset, client_tree);

				fitem = proto_tree_add_text(newftree, tvb, offset, 0, "callback");
				callback_tree = proto_item_add_subtree(fitem,
						ett_nfs4_cb_client);

				offset = dissect_nfs4_cb_client4(tvb, offset, callback_tree);

				offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_callback_ident,
					offset);
			}
			break;

		case NFS4_OP_SETCLIENTID_CONFIRM:
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_clientid, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_verifier, offset);
			break;

		case NFS4_OP_VERIFY:
			offset = dissect_nfs4_fattrs(tvb, offset, pinfo, newftree, FATTR4_DISSECT_VALUES);
			break;

		case NFS4_OP_WRITE:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, &sid_hash);
			file_offset = tvb_get_ntoh64(tvb, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_offset, offset);
			offset = dissect_nfs4_stable_how(tvb, offset, newftree, "stable");
			string_length = tvb_get_ntohl(tvb,offset+0);
			dissect_rpc_uint32(tvb, newftree, hf_nfs4_write_data_length, offset); /
			offset = dissect_nfsdata(tvb, offset, newftree, hf_nfs_data);
			if (sid_hash != 0)
				g_string_append_printf (op_summary[ops_counter].optext,
					" StateID: 0x%04x Offset: %"G_GINT64_MODIFIER"u Len: %u",
					sid_hash, file_offset, string_length);
			break;

		case NFS4_OP_RELEASE_LOCKOWNER:
			offset = dissect_nfs4_lock_owner(tvb, offset, newftree);
			break;

			/
		case NFS4_OP_BACKCHANNEL_CTL:
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_cb_program, offset);
			offset = dissect_rpc_secparms4(tvb, offset, newftree);
			break;
		case NFS4_OP_BIND_CONN_TO_SESSION:
			offset = dissect_nfs4_sessionid(tvb, offset, newftree);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_bctsa_dir, offset);
			offset = dissect_rpc_bool(tvb, newftree, hf_nfs4_bctsa_use_conn_in_rdma_mode, offset);
			break;

		case NFS4_OP_EXCHANGE_ID:
			{
			proto_tree *eia_clientowner_tree = NULL;
			proto_tree *eia_flags_tree = NULL;

			fitem = proto_tree_add_text(newftree, tvb, offset, 0, "eia_clientowner");
			eia_clientowner_tree = proto_item_add_subtree(fitem, ett_nfs4_clientowner);
			offset = dissect_rpc_uint64(tvb, eia_clientowner_tree, hf_nfs4_verifier, offset);
			offset = dissect_nfsdata(tvb, offset, eia_clientowner_tree, hf_nfs_data);

			fitem = proto_tree_add_item(newftree, hf_nfs4_exchid_call_flags, tvb, offset, 4,
				ENC_BIG_ENDIAN);
			eia_flags_tree = proto_item_add_subtree(fitem, ett_nfs4_exchangeid_call_flags);
			proto_tree_add_item(eia_flags_tree, hf_nfs4_exchid_flags_confirmed_r, tvb, offset, 4,
				ENC_BIG_ENDIAN);
			proto_tree_add_item(eia_flags_tree, hf_nfs4_exchid_flags_upd_conf_rec_a, tvb, offset, 4,
				ENC_BIG_ENDIAN);
			proto_tree_add_item(eia_flags_tree, hf_nfs4_exchid_flags_pnfs_ds, tvb, offset, 4,
				ENC_BIG_ENDIAN);
			proto_tree_add_item(eia_flags_tree, hf_nfs4_exchid_flags_pnfs_mds, tvb, offset, 4,
				ENC_BIG_ENDIAN);
			proto_tree_add_item(eia_flags_tree, hf_nfs4_exchid_flags_non_pnfs, tvb, offset, 4,
				ENC_BIG_ENDIAN);
			proto_tree_add_item(eia_flags_tree, hf_nfs4_exchid_flags_bind_princ, tvb, offset, 4,
				ENC_BIG_ENDIAN);
			proto_tree_add_item(eia_flags_tree, hf_nfs4_exchid_flags_moved_migr, tvb, offset, 4,
				ENC_BIG_ENDIAN);
			proto_tree_add_item(eia_flags_tree, hf_nfs4_exchid_flags_moved_refer, tvb, offset, 4,
				ENC_BIG_ENDIAN);
			offset += 4;

			offset = dissect_nfs4_state_protect_a(tvb, offset, newftree);
			offset = dissect_rpc_nfs_impl_id4(tvb, offset, newftree, "eia_client_impl_id");
			}
			break;

		case NFS4_OP_CREATE_SESSION:
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_clientid, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_seqid, offset);
			offset = dissect_nfs_create_session_flags(tvb, offset, newftree, 
				hf_nfs4_create_session_flags_csa);
			offset = dissect_rpc_chanattrs4(tvb, offset, newftree, "csa_fore_chan_attrs");
			offset = dissect_rpc_chanattrs4(tvb, offset, newftree, "csa_back_chan_attrs");
			cbprog = tvb_get_ntohl(tvb, offset);
			reg_callback(cbprog);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_cb_program, offset);
			offset = dissect_rpc_secparms4(tvb, offset, newftree);
			break;

		case NFS4_OP_DESTROY_SESSION:
			offset = dissect_nfs4_sessionid(tvb, offset, newftree);
			break;
		case NFS4_OP_FREE_STATEID:
			offset = dissect_nfs4_stateid(tvb, offset, newftree, &sid_hash);
			g_string_append_printf(op_summary[ops_counter].optext, " StateID: 0x%04x", sid_hash);
			break;

			/
		case NFS4_OP_LAYOUTGET:
			offset = dissect_rpc_bool(tvb, newftree, hf_nfs4_layout_avail, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_layout_type, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_iomode, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_offset, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_length, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_length_minlength, offset);
			offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_count_maxcount,
										offset);
			break;

		case NFS4_OP_LAYOUTCOMMIT:
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_offset, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_length, offset);
			offset = dissect_rpc_bool(tvb, newftree, hf_nfs4_reclaim, offset);
			offset = dissect_nfs4_stateid(tvb, offset, newftree, NULL);
			offset = dissect_nfs4_newoffset(tvb, offset, newftree);
			offset = dissect_nfs4_newtime(tvb, offset, newftree);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_layout_type, offset);
			offset = dissect_nfsdata(tvb, offset, newftree, hf_nfs4_layoutupdate);
			break;

		case NFS4_OP_LAYOUTRETURN:
			offset = dissect_rpc_bool(tvb, newftree, hf_nfs4_reclaim, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_layout_type, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_iomode, offset);
			offset = dissect_nfs4_layoutreturn(tvb, offset, newftree);
			break;

		case NFS4_OP_GETDEVINFO:
			offset = dissect_nfs4_deviceid(tvb, offset, newftree);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_layout_type, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_count_maxcount, offset);
			offset = dissect_nfs4_notification_bitmap(tvb, newftree, offset);
			break;

		case NFS4_OP_GETDEVLIST:
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_layout_type, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_count_maxcount, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_cookie, offset);
			offset = dissect_rpc_uint64(tvb, newftree, hf_nfs4_cookie_verf,	offset);
			break;

		case NFS4_OP_SEQUENCE:
			offset = dissect_nfs4_sessionid(tvb, offset, newftree);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_seqid, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_slotid, offset);
			offset = dissect_rpc_uint32(tvb, newftree, hf_nfs4_high_slotid, offset);
			offset = dissect_rpc_bool(tvb, newftree, hf_nfs4_cachethis, offset);
			break;

		/
		case NFS4_OP_ILLEGAL:
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
			highest_tier = current_tier;
	}

	/
	for (summary_counter=0; summary_counter < ops_counter; summary_counter++)
	{
		guint main_opcode;
		proto_item *main_op_item = NULL;

		main_opcode = op_summary[summary_counter].opcode;
		current_tier = nfs4_operation_tiers[op_summary[summary_counter].opcode];

		/
		if (current_tier==highest_tier || !display_major_nfs4_ops) {
			if (current_tier==highest_tier) {
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
