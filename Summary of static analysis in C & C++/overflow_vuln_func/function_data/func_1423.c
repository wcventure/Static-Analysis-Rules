static void
proto_mpeg_descriptor_dissect_iso639(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	if (len > 1)
		proto_tree_add_item(tree, hf_mpeg_descr_iso639_lang, tvb, offset, len - 1, ENC_ASCII|ENC_NA);
	offset += len - 1;
	proto_tree_add_item(tree, hf_mpeg_descr_iso639_type, tvb, offset, 1, ENC_BIG_ENDIAN);
}
