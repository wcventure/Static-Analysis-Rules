static int
dissect_nfs2_rename_call(tvbuff_t *tvb, int offset, packet_info *pinfo,
			 proto_tree *tree)
{
	guint32 from_hash;
	const char *from_name=NULL;
	guint32 to_hash;
	const char *to_name=NULL;

	offset = dissect_diropargs(tvb, offset, pinfo, tree, "from", &from_hash, &from_name);
	offset = dissect_diropargs(tvb, offset, pinfo, tree, "to", &to_hash, &to_name);

	col_append_fstr(pinfo->cinfo, COL_INFO,", From DH: 0x%08x/%s To DH: 0x%08x/%s", 
		from_hash, from_name, to_hash, to_name);
	proto_item_append_text(tree, ", RENAME Call From DH: 0x%08x/%s To DH: 0x%08x/%s", 
		from_hash, from_name, to_hash, to_name);

	return offset;
}
