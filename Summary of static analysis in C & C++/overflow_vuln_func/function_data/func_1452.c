static void
proto_mpeg_descriptor_dissect_data_bcast(tvbuff_t *tvb, guint offset, proto_tree *tree)
{

	guint8 selector_len, text_len;

	proto_tree_add_item(tree, hf_mpeg_descr_data_bcast_bcast_id, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(tree, hf_mpeg_descr_data_bcast_component_tag, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	selector_len = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_data_bcast_selector_len, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	if (selector_len > 0) {
		proto_tree_add_item(tree, hf_mpeg_descr_data_bcast_selector_bytes, tvb, offset, selector_len, ENC_NA);
		offset += selector_len;
	}

	proto_tree_add_item(tree, hf_mpeg_descr_data_bcast_lang_code, tvb, offset, 3, ENC_ASCII|ENC_NA);
	offset += 3;

	text_len = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_data_bcast_text_len, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	if (text_len > 0)
		proto_tree_add_item(tree, hf_mpeg_descr_data_bcast_text, tvb, offset, text_len, ENC_ASCII|ENC_NA);
}
