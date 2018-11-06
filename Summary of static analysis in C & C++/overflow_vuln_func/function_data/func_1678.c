static int
dissect_smb2_getinfo_buffer(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, int offset, int size, smb2_info_t *si)
{
	switch (si->saved->smb2_class) {
	case SMB2_CLASS_QUOTA_INFO:
		dissect_smb2_getinfo_buffer_quota(tvb, pinfo, tree, offset, si);
		break;
	default:
		if (size > 0) {
			proto_tree_add_item(tree, hf_smb2_unknown, tvb, offset, size, ENC_NA);
		}
	}
	offset += size;

	return offset;
}
