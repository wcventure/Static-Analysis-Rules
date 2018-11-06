static int
dissect_get_dfs_referral_data(tvbuff_t *tvb, packet_info *pinfo,
    proto_tree *tree, int offset, guint16 *bcp)
{
	smb_info_t *si = pinfo->private_data;
	guint16 numref;
	guint16 refsize;
	guint16 pathoffset;
	guint16 altpathoffset;
	guint16 nodeoffset;
	int fn_len;
	int stroffset;
	int offsetoffset;
	guint16 save_bc;
	const char *fn;
	int unklen;
	int ucstring_end;
	int ucstring_len;

	DISSECTOR_ASSERT(si);

	/
	CHECK_BYTE_COUNT_TRANS_SUBR(2);
	proto_tree_add_item(tree, hf_smb_dfs_path_consumed, tvb, offset, 2, TRUE);
	COUNT_BYTES_TRANS_SUBR(2);

	/
	CHECK_BYTE_COUNT_TRANS_SUBR(2);
	numref = tvb_get_letohs(tvb, offset);
	proto_tree_add_uint(tree, hf_smb_dfs_num_referrals, tvb, offset, 2, numref);
	COUNT_BYTES_TRANS_SUBR(2);

	/
	CHECK_BYTE_COUNT_TRANS_SUBR(2);
	offset = dissect_get_dfs_flags(tvb, tree, offset);
	*bcp -= 2;

	/
	CHECK_BYTE_COUNT_TRANS_SUBR(2);
	proto_tree_add_item(tree, hf_smb_padding, tvb, offset, 2, TRUE);
	COUNT_BYTES_TRANS_SUBR(2);

	/
	if(numref){
		proto_item *ref_item = NULL;
		proto_tree *ref_tree = NULL;
		int old_offset=offset;

		if(tree){
			tvb_ensure_bytes_exist(tvb, offset, *bcp);
			ref_item = proto_tree_add_text(tree,
				tvb, offset, *bcp, "Referrals");
			ref_tree = proto_item_add_subtree(ref_item,
				ett_smb_dfs_referrals);
		}
		ucstring_end = -1;

		while(numref--){
			proto_item *ri = NULL;
			proto_tree *rt = NULL;
			int old_offset=offset;
			guint16 version;

			if(tree){
				tvb_ensure_bytes_exist(tvb, offset, *bcp);
				ri = proto_tree_add_text(ref_tree,
					tvb, offset, *bcp, "Referral");
				rt = proto_item_add_subtree(ri,
					ett_smb_dfs_referral);
			}

			/
			CHECK_BYTE_COUNT_TRANS_SUBR(2);
			version = tvb_get_letohs(tvb, offset);
			proto_tree_add_uint(rt, hf_smb_dfs_referral_version,
				tvb, offset, 2, version);
			COUNT_BYTES_TRANS_SUBR(2);

			/
			CHECK_BYTE_COUNT_TRANS_SUBR(2);
			refsize = tvb_get_letohs(tvb, offset);
			proto_tree_add_uint(rt, hf_smb_dfs_referral_size, tvb, offset, 2, refsize);
			COUNT_BYTES_TRANS_SUBR(2);

			/
			CHECK_BYTE_COUNT_TRANS_SUBR(2);
			proto_tree_add_item(rt, hf_smb_dfs_referral_server_type, tvb, offset, 2, TRUE);
			COUNT_BYTES_TRANS_SUBR(2);

			/
			CHECK_BYTE_COUNT_TRANS_SUBR(2);
			offset = dissect_dfs_referral_flags(tvb, rt, offset);
			*bcp -= 2;

			switch(version){

			case 1:
				/
				fn = get_unicode_or_ascii_string(tvb, &offset, si->unicode, &fn_len, FALSE, FALSE, bcp);
				CHECK_STRING_TRANS_SUBR(fn);
				proto_tree_add_string(rt, hf_smb_dfs_referral_node, tvb, offset, fn_len,
					fn);
				COUNT_BYTES_TRANS_SUBR(fn_len);
				break;

			case 2:
			case 3:	/
				/
				CHECK_BYTE_COUNT_TRANS_SUBR(2);
				proto_tree_add_item(rt, hf_smb_dfs_referral_proximity, tvb, offset, 2, TRUE);
				COUNT_BYTES_TRANS_SUBR(2);

				/
				CHECK_BYTE_COUNT_TRANS_SUBR(2);
				proto_tree_add_item(rt, hf_smb_dfs_referral_ttl, tvb, offset, 2, TRUE);
				COUNT_BYTES_TRANS_SUBR(2);

				/
				CHECK_BYTE_COUNT_TRANS_SUBR(2);
				pathoffset = tvb_get_letohs(tvb, offset);
				proto_tree_add_uint(rt, hf_smb_dfs_referral_path_offset, tvb, offset, 2, pathoffset);
				COUNT_BYTES_TRANS_SUBR(2);

				/
				CHECK_BYTE_COUNT_TRANS_SUBR(2);
				altpathoffset = tvb_get_letohs(tvb, offset);
				proto_tree_add_uint(rt, hf_smb_dfs_referral_alt_path_offset, tvb, offset, 2, altpathoffset);
				COUNT_BYTES_TRANS_SUBR(2);

				/
				CHECK_BYTE_COUNT_TRANS_SUBR(2);
				nodeoffset = tvb_get_letohs(tvb, offset);
				proto_tree_add_uint(rt, hf_smb_dfs_referral_node_offset, tvb, offset, 2, nodeoffset);
				COUNT_BYTES_TRANS_SUBR(2);

				/
				if (pathoffset != 0) {
					stroffset = old_offset + pathoffset;
					offsetoffset = stroffset - offset;
					if (offsetoffset > 0 &&
					    *bcp > offsetoffset) {
						save_bc = *bcp;
						*bcp -= offsetoffset;
						fn = get_unicode_or_ascii_string(tvb, &stroffset, si->unicode, &fn_len, FALSE, FALSE, bcp);
						CHECK_STRING_TRANS_SUBR(fn);
						proto_tree_add_string(rt, hf_smb_dfs_referral_path, tvb, stroffset, fn_len,
							fn);
						stroffset += fn_len;
						if (ucstring_end < stroffset)
							ucstring_end = stroffset;
						*bcp = save_bc;
					}
				}

				/
				if (altpathoffset != 0) {
					stroffset = old_offset + altpathoffset;
					offsetoffset = stroffset - offset;
					if (offsetoffset > 0 &&
					    *bcp > offsetoffset) {
						save_bc = *bcp;
						*bcp -= offsetoffset;
						fn = get_unicode_or_ascii_string(tvb, &stroffset, si->unicode, &fn_len, FALSE, FALSE, bcp);
						CHECK_STRING_TRANS_SUBR(fn);
						proto_tree_add_string(rt, hf_smb_dfs_referral_alt_path, tvb, stroffset, fn_len,
							fn);
						stroffset += fn_len;
						if (ucstring_end < stroffset)
							ucstring_end = stroffset;
						*bcp = save_bc;
					}
				}

				/
				if (nodeoffset != 0) {
					stroffset = old_offset + nodeoffset;
					offsetoffset = stroffset - offset;
					if (offsetoffset > 0 &&
					    *bcp > offsetoffset) {
						save_bc = *bcp;
						*bcp -= offsetoffset;
						fn = get_unicode_or_ascii_string(tvb, &stroffset, si->unicode, &fn_len, FALSE, FALSE, bcp);
						CHECK_STRING_TRANS_SUBR(fn);
						proto_tree_add_string(rt, hf_smb_dfs_referral_node, tvb, stroffset, fn_len,
							fn);
						stroffset += fn_len;
						if (ucstring_end < stroffset)
							ucstring_end = stroffset;
						*bcp = save_bc;
					}
				}
				break;
			}

			/
			unklen = (old_offset + refsize) - offset;
			if (unklen < 0) {
				/
				unklen = 0;
			}
			if (unklen != 0) {
				CHECK_BYTE_COUNT_TRANS_SUBR(unklen);
				proto_tree_add_item(rt, hf_smb_unknown, tvb,
				    offset, unklen, TRUE);
				COUNT_BYTES_TRANS_SUBR(unklen);
			}

			proto_item_set_len(ri, offset-old_offset);
		}

		/
		if (ucstring_end > offset) {
			ucstring_len = ucstring_end - offset;
			if (*bcp < ucstring_len)
				ucstring_len = *bcp;
			offset += ucstring_len;
			*bcp -= ucstring_len;
		}
		proto_item_set_len(ref_item, offset-old_offset);
	}

	return offset;
}
