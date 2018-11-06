static void
proto_mpeg_descriptor_dissect_subtitling(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint end = offset + len;

	while (offset < end) {
		proto_tree_add_item(tree, hf_mpeg_descr_subtitling_lang_code, tvb, offset, 3, ENC_ASCII|ENC_NA);
		offset += 3;

		proto_tree_add_item(tree, hf_mpeg_descr_subtitling_type, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(tree, hf_mpeg_descr_subtitling_composition_page_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(tree, hf_mpeg_descr_subtitling_ancillary_page_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

	}
}
