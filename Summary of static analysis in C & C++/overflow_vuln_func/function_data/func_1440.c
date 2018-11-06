static void
proto_mpeg_descriptor_dissect_short_event(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	guint8 name_len, text_len;

	proto_tree_add_item(tree, hf_mpeg_descr_short_event_lang_code, tvb, offset, 3, ENC_ASCII|ENC_NA);
	offset += 3;

	name_len = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_short_event_name_length, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_short_event_name, tvb, offset, name_len, ENC_ASCII|ENC_NA);
	offset += name_len;

	text_len = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_short_event_text_length, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_short_event_text, tvb, offset, text_len, ENC_ASCII|ENC_NA);

}
