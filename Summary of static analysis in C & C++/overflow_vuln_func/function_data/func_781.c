static int
dissect_nt_trans_data_response(tvbuff_t *tvb, packet_info *pinfo,
			       int offset, proto_tree *parent_tree, int len,
			       nt_trans_data *ntd _U_)
{
	proto_item *item = NULL;
	proto_tree *tree = NULL;
	smb_info_t *si;
	smb_nt_transact_info_t *nti;
	guint16 bcp;

	si = (smb_info_t *)pinfo->private_data;
	DISSECTOR_ASSERT(si);

	if (si->sip != NULL && si->sip->extra_info_type == SMB_EI_NTI)
		nti = si->sip->extra_info;
	else
		nti = NULL;

	if(parent_tree){
		tvb_ensure_bytes_exist(tvb, offset, len);
		if(nti != NULL){
			item = proto_tree_add_text(parent_tree, tvb, offset, len,
				"%s Data",
				val_to_str(nti->subcmd, nt_cmd_vals, "Unknown NT Transaction (%u)"));
		} else {
			/
			item = proto_tree_add_text(parent_tree, tvb, offset, len,
				"Unknown NT Transaction Data (matching request not seen)");
		}
		tree = proto_item_add_subtree(item, ett_smb_nt_trans_data);
	}

	if (nti == NULL) {
		offset += len;
		return offset;
	}
	switch(nti->subcmd){
	case NT_TRANS_CREATE:
		break;
	case NT_TRANS_IOCTL:
		/
		proto_tree_add_item(tree, hf_smb_nt_ioctl_data, tvb, offset, len, TRUE);
		offset += len;

		break;
	case NT_TRANS_SSD:
		break;
	case NT_TRANS_NOTIFY:
		break;
	case NT_TRANS_RENAME:
		/
		break;
	case NT_TRANS_QSD: {
		/
		offset = dissect_nt_sec_desc(
			tvb, offset, pinfo, tree, NULL, len, NULL);
		break;
	}
	case NT_TRANS_GET_USER_QUOTA:
		bcp=len;
		offset = dissect_nt_user_quota(tvb, tree, offset, &bcp);
		break;
	case NT_TRANS_SET_USER_QUOTA:
		/
		break;
	}

	return offset;
}
