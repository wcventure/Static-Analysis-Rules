static int
dissect_entry3(tvbuff_t *tvb, int offset, packet_info *pinfo _U_,
	       proto_tree* tree)
{
	proto_item* entry_item = NULL;
	proto_tree* entry_tree = NULL;
	int old_offset = offset;
	const char *name=NULL;

	if (tree) {
		entry_item = proto_tree_add_item(tree, hf_nfs_readdir_entry, tvb,
			offset+0, -1, ENC_NA);
		entry_tree = proto_item_add_subtree(entry_item, ett_nfs_readdir_entry);
	}

	offset = dissect_rpc_uint64(tvb, entry_tree, hf_nfs3_readdir_entry_fileid, offset);

	offset = dissect_nfs3_filename(tvb, offset, entry_tree,	hf_nfs3_readdir_entry_name, &name);
	if (entry_item)
		proto_item_set_text(entry_item, "Entry: name %s", name);

	col_append_fstr(pinfo->cinfo, COL_INFO," %s", name);

	offset = dissect_rpc_uint64(tvb, entry_tree, hf_nfs3_readdir_entry_cookie, offset);

	/
	if (entry_item) 
		proto_item_set_len(entry_item, offset - old_offset);

	return offset;
}
