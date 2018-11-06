static void
proto_mpeg_descriptor_dissect_component(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{

	proto_item *cti;
	proto_tree *content_type_tree;

	proto_tree_add_item(tree, hf_mpeg_descr_component_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);

	cti = proto_tree_add_item(tree, hf_mpeg_descr_component_content_type, tvb, offset, 2, ENC_BIG_ENDIAN);
	content_type_tree = proto_item_add_subtree(cti, ett_mpeg_descriptor_component_content_type);

	proto_tree_add_item(content_type_tree, hf_mpeg_descr_component_stream_content, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(content_type_tree, hf_mpeg_descr_component_type, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_component_tag, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_component_lang_code, tvb, offset, 3, ENC_ASCII|ENC_NA);
	offset += 3;

	if (offset < len)
		proto_tree_add_item(tree, hf_mpeg_descr_component_text, tvb, offset, len - offset, ENC_ASCII|ENC_NA);
}
