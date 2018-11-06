static void
proto_mpeg_descriptor_dissect_carousel_identifier(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint8 key_len, format_id;
	guint  private_len = 0;

	proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_id, tvb, offset, 4, ENC_BIG_ENDIAN);
	offset += 4;

	format_id = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_format_id, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	if (format_id == 0x01) {
		proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_module_version, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_module_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_block_size, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_module_size, tvb, offset, 4, ENC_BIG_ENDIAN);
		offset += 4;

		proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_compression_method, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_original_size, tvb, offset, 4, ENC_BIG_ENDIAN);
		offset += 4;

		proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_timeout, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		key_len = tvb_get_guint8(tvb, offset);
		proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_object_key_len, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_object_key_data, tvb, offset, key_len, ENC_NA);
		offset += key_len;

		if (len > 20 + key_len)
			private_len = len - 20 - key_len;

	} else {
		if (len > 5)
			private_len = len - 5;
	}

	if (private_len)
		proto_tree_add_item(tree, hf_mpeg_descr_carousel_identifier_private, tvb, offset, private_len, ENC_NA);

}
