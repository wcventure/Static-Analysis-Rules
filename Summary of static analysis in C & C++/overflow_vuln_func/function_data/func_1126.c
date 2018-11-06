static int
dissect_form_urlencoded(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data)
{
	proto_tree	*url_tree;
	proto_tree	*sub;
	proto_item	*ti;
	gint		offset = 0, next_offset;
	const char	*data_name;

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
		col_append_sep_fstr(pinfo->cinfo, COL_INFO, " ", "(%s)", data_name);

	ti = proto_tree_add_item(tree, hfi_urlencoded, tvb, 0, -1, ENC_NA);
	if (data_name)
		proto_item_append_text(ti, ": %s", data_name);
	url_tree = proto_item_add_subtree(ti, ett_form_urlencoded);

	while (tvb_reported_length_remaining(tvb, offset) > 0) {
		const int start_offset = offset;
		char *key, *value;

		sub = proto_tree_add_subtree(url_tree, tvb, offset, 0, ett_form_keyvalue, &ti, "Form item");

		next_offset = get_form_key_value(tvb, &key, offset, '=');
		if (next_offset == -1)
			break;
		proto_tree_add_string(sub, &hfi_form_key, tvb, offset, next_offset - offset, key);
		proto_item_append_text(sub, ": \"%s\"", key);

		offset = next_offset+1;

		next_offset = get_form_key_value(tvb, &value, offset, '&');
		if (next_offset == -1)
			break;
		proto_tree_add_string(sub, &hfi_form_value, tvb, offset, next_offset - offset, value);
		proto_item_append_text(sub, " = \"%s\"", value);

		offset = next_offset+1;

		proto_item_set_len(ti, offset - start_offset);
	}

	return tvb_captured_length(tvb);
}
