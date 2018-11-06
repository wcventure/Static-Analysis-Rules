static int
dissect_text_lines(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data)
{
	proto_tree	*subtree;
	proto_item	*ti;
	gint		offset = 0, next_offset;
	gint		len;
	const char	*data_name;
	int length = tvb_captured_length(tvb);

	/
	if(length > 38){
		if (tvb_strncaseeql(tvb, 0, "<?xml", 5) == 0){
			call_dissector(xml_handle, tvb, pinfo, tree);
			return length;
		}
	}

	data_name = pinfo->match_string;
	if (! (data_name && data_name[0])) {
		/
		data_name = (char *)data;
		if (! (data_name && data_name[0])) {
			/
			data_name = NULL;
		}
	}

	if (data_name)
		col_append_sep_fstr(pinfo->cinfo, COL_INFO, " ", "(%s)",
				data_name);

	if (tree) {
		ti = proto_tree_add_item(tree, proto_text_lines,
				tvb, 0, -1, ENC_NA);
		if (data_name)
			proto_item_append_text(ti, ": %s", data_name);
		subtree = proto_item_add_subtree(ti, ett_text_lines);
		/
		while (tvb_offset_exists(tvb, offset)) {
			/
			len = tvb_find_line_end(tvb, offset, -1, &next_offset, FALSE);
			if (len == -1)
				break;

			/
			proto_tree_add_format_text(subtree, tvb, offset, next_offset - offset);
			offset = next_offset;
		}
	}

	return length;
}
