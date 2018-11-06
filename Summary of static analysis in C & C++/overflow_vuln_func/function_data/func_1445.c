static void
proto_mpeg_descriptor_dissect_content(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	proto_item *ni;
	proto_tree *nibble_tree;

	guint end = offset + len;

	while (offset < end) {
		ni = proto_tree_add_item(tree, hf_mpeg_descr_content_nibble, tvb, offset, 1, ENC_BIG_ENDIAN);
		nibble_tree = proto_item_add_subtree(ni, ett_mpeg_descriptor_content_nibble);

		proto_tree_add_item(nibble_tree, hf_mpeg_descr_content_nibble_level_1, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(nibble_tree, hf_mpeg_descr_content_nibble_level_2, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(tree, hf_mpeg_descr_content_user_byte, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
	}

}
