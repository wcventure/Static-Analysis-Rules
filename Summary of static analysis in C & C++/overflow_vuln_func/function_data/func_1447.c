static void
proto_mpeg_descriptor_dissect_teletext(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint end = offset + len;

	while (offset < end) {
		proto_tree_add_item(tree, hf_mpeg_descr_teletext_lang_code, tvb, offset, 3, ENC_ASCII|ENC_NA);
		offset += 3;

		proto_tree_add_item(tree, hf_mpeg_descr_teletext_type, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_teletext_magazine_number, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(tree, hf_mpeg_descr_teletext_page_number, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
	}
}
