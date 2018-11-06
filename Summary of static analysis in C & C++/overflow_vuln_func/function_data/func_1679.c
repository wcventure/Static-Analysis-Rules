static int
dissect_smb2_class_infolevel(packet_info *pinfo, tvbuff_t *tvb, int offset, proto_tree *tree, smb2_info_t *si)
{
	guint8		  cl, il;
	proto_item	 *item;
	int		  hfindex;
	value_string_ext *vsx;

	if (si->flags & SMB2_FLAGS_RESPONSE) {
		if (!si->saved) {
			return offset;
		}
		cl = si->saved->smb2_class;
		il = si->saved->infolevel;
	} else {
		cl = tvb_get_guint8(tvb, offset);
		il = tvb_get_guint8(tvb, offset+1);
		if (si->saved) {
			si->saved->smb2_class = cl;
			si->saved->infolevel = il;
		}
	}


	switch (cl) {
	case SMB2_CLASS_FILE_INFO:
		hfindex = hf_smb2_infolevel_file_info;
		vsx = &smb2_file_info_levels_ext;
		break;
	case SMB2_CLASS_FS_INFO:
		hfindex = hf_smb2_infolevel_fs_info;
		vsx = &smb2_fs_info_levels_ext;
		break;
	case SMB2_CLASS_SEC_INFO:
		hfindex = hf_smb2_infolevel_sec_info;
		vsx = &smb2_sec_info_levels_ext;
		break;
	case SMB2_CLASS_QUOTA_INFO:
		/
		hfindex = hf_smb2_infolevel;
		vsx = NULL;
		break;
	case SMB2_CLASS_POSIX_INFO:
		hfindex = hf_smb2_infolevel_posix_info;
		vsx = &smb2_posix_info_levels_ext;
		break;
	default:
		hfindex = hf_smb2_infolevel;
		vsx = NULL;  /
	}


	/
	item = proto_tree_add_uint(tree, hf_smb2_class, tvb, offset, 1, cl);
	if (si->flags & SMB2_FLAGS_RESPONSE) {
		PROTO_ITEM_SET_GENERATED(item);
	}
	/
	item = proto_tree_add_uint(tree, hfindex, tvb, offset+1, 1, il);
	if (si->flags & SMB2_FLAGS_RESPONSE) {
		PROTO_ITEM_SET_GENERATED(item);
	}
	offset += 2;

	if (!(si->flags & SMB2_FLAGS_RESPONSE)) {
		/
		col_append_fstr(pinfo->cinfo, COL_INFO, " %s/%s",
				val_to_str(cl, smb2_class_vals, "(Class:0x%02x)"),
				val_to_str_ext(il, vsx, "(Level:0x%02x)"));
	}

	return offset;
}
