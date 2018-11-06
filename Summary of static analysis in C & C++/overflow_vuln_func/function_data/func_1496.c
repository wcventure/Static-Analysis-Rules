static int
decode_gtp_data_req(tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree) {

	guint16		length, format_ver;
	guint8		no, format;
	proto_tree	*ext_tree;
	proto_item	*te;
	tvbuff_t	*next_tvb;

	te = proto_tree_add_text(tree, tvb, offset, 1, val_to_str(GTP_EXT_DATA_REQ, gtp_val, "Unknown message"));
	ext_tree = proto_item_add_subtree(te, ett_gtp_ext);

	length = tvb_get_ntohs(tvb, offset + 1);
	no = tvb_get_guint8(tvb, offset + 3);
	format = tvb_get_guint8(tvb, offset + 4);
	format_ver = tvb_get_ntohs(tvb, offset + 5);

	proto_tree_add_text(ext_tree, tvb, offset+1, 2, "Length: %u", length);
	proto_tree_add_text(ext_tree, tvb, offset+3, 1, "Number of data records: %u", no);
	proto_tree_add_text(ext_tree, tvb, offset+4, 1, "Data record format: %u", format);
	proto_tree_add_text(ext_tree, tvb, offset+5, 2, "Data record format version: %u", format_ver);
	
	if (gtpcdr_handle) {
		next_tvb = tvb_new_subset (tvb, offset, -1, -1);
		call_dissector (gtpcdr_handle, next_tvb, pinfo, tree);
	}
	else
		proto_tree_add_text (tree, tvb, offset, 0, "Data");

	return 3+length;
}
