static int
dissect_smb2_getinfo_request(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, int offset, smb2_info_t *si)
{
	guint32 getinfo_size = 0;
	guint32 getinfo_offset = 0;

	/
	offset = dissect_smb2_buffercode(tree, tvb, offset, NULL);

	/
	offset = dissect_smb2_class_infolevel(pinfo, tvb, offset, tree, si);

	/
	proto_tree_add_item(tree, hf_smb2_max_response_size, tvb, offset, 4, ENC_LITTLE_ENDIAN);
	offset += 4;

	/
	proto_tree_add_item_ret_uint(tree, hf_smb2_getinfo_offset, tvb, offset, 2, ENC_LITTLE_ENDIAN, &getinfo_offset);
	offset += 4;

	/
	proto_tree_add_item_ret_uint(tree, hf_smb2_getinfo_size, tvb, offset, 4, ENC_LITTLE_ENDIAN, &getinfo_size);
	offset += 4;

	/
	if (si->saved) {
		dissect_smb2_getinfo_parameters(tvb, pinfo, tree, offset, si);
	} else {
		/
		proto_tree_add_item(tree, hf_smb2_unknown, tvb, offset, 8, ENC_NA);
	}
	offset += 8;

	/
	dissect_smb2_fid(tvb, pinfo, tree, offset, si, FID_MODE_USE);

	/
	if (si->saved) {
		dissect_smb2_getinfo_buffer(tvb, pinfo, tree, getinfo_offset, getinfo_size, si);
	}
	offset = getinfo_offset + getinfo_size;

	return offset;
}
