static int
dissect_json(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data)
{
	proto_tree *json_tree = NULL;
	proto_item *ti = NULL;

	json_parser_data_t parser_data;
	tvbparse_t *tt;

	const char *data_name;
	int offset;

	/
	wmem_list_frame_t *proto = wmem_list_frame_prev(wmem_list_tail(pinfo->layers));
	if (proto) {
		const char *name = proto_get_protocol_filter_name(GPOINTER_TO_INT(wmem_list_frame_data(proto)));

		if (!strcmp(name, "frame")) {
			col_set_str(pinfo->cinfo, COL_PROTOCOL, "JSON");
			col_set_str(pinfo->cinfo, COL_INFO, "JavaScript Object Notation");
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

	if (tree) {
		ti = proto_tree_add_item(tree, hfi_json, tvb, 0, -1, ENC_NA);
		json_tree = proto_item_add_subtree(ti, ett_json);

		if (data_name)
			proto_item_append_text(ti, ": %s", data_name);
	}

	offset = 0;

	parser_data.stack = wmem_stack_new(wmem_packet_scope());
	wmem_stack_push(parser_data.stack, json_tree);

	tt = tvbparse_init(tvb, offset, -1, &parser_data, want_ignore);

	/
	while ((tvbparse_get(tt, want)))
		;

	offset = tvbparse_curr_offset(tt);

	proto_item_set_len(ti, offset);

	/
	if (tvb_reported_length_remaining(tvb, offset) > 0) {
		tvbuff_t *next_tvb;

		next_tvb = tvb_new_subset_remaining(tvb, offset);

		call_dissector_with_data(text_lines_handle, next_tvb, pinfo, tree, data);
	} else if (data_name) {
		col_append_sep_fstr(pinfo->cinfo, COL_INFO, " ", "(%s)", data_name);
	}

	return tvb_captured_length(tvb);
}
