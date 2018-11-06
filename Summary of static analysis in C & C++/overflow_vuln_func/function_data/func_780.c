static int
dissect_nt_trans_data_request(tvbuff_t *tvb, packet_info *pinfo, int offset, proto_tree *parent_tree, int bc, nt_trans_data *ntd)
{
	proto_item *item = NULL;
	proto_tree *tree = NULL;
	smb_info_t *si;
	int old_offset = offset;
	guint16 bcp=bc; /

	si = (smb_info_t *)pinfo->private_data;

	DISSECTOR_ASSERT(si);

	if(parent_tree){
		tvb_ensure_bytes_exist(tvb, offset, bc);
		item = proto_tree_add_text(parent_tree, tvb, offset, bc,
				"%s Data",
				val_to_str(ntd->subcmd, nt_cmd_vals, "Unknown NT transaction (%u)"));
		tree = proto_item_add_subtree(item, ett_smb_nt_trans_data);
	}

	switch(ntd->subcmd){
	case NT_TRANS_CREATE:
		/
		if(ntd->sd_len){
		        offset = dissect_nt_sec_desc(
				tvb, offset, pinfo, tree, NULL, ntd->sd_len, 
				NULL);
		}

		/
		if(ntd->ea_len){
			proto_tree_add_item(tree, hf_smb_extended_attributes, tvb, offset, ntd->ea_len, TRUE);
			offset += ntd->ea_len;
		}

		break;
	case NT_TRANS_IOCTL:
		/
		proto_tree_add_item(tree, hf_smb_nt_ioctl_data, tvb, offset, bc, TRUE);
		offset += bc;

		break;
	case NT_TRANS_SSD:
		offset = dissect_nt_sec_desc(
			tvb, offset, pinfo, tree, NULL, bc, NULL);
		break;
	case NT_TRANS_NOTIFY:
		break;
	case NT_TRANS_RENAME:
		/
		break;
	case NT_TRANS_QSD:
		break;
	case NT_TRANS_GET_USER_QUOTA:
		/
		proto_tree_add_item(tree, hf_smb_unknown, tvb,
			    offset, 4, TRUE);
		offset += 4;

		/
		proto_tree_add_text(tree, tvb, offset, 4, "Length of SID: %d", tvb_get_letohl(tvb, offset));
		offset +=4;

		offset = dissect_nt_sid(tvb, offset, tree, "Quota", NULL, -1);
		break;
	case NT_TRANS_SET_USER_QUOTA:
		offset = dissect_nt_user_quota(tvb, tree, offset, &bcp);
		break;
	}

	/
	if((offset-old_offset) < bc){
		proto_tree_add_item(tree, hf_smb_unknown, tvb, offset,
		    bc - (offset-old_offset), TRUE);
		offset += bc - (offset-old_offset);
	}

	return offset;
}
