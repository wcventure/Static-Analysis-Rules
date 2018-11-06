static void
proto_mpeg_descriptor_dissect_service(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	guint8 descr_len, name_len;

	proto_tree_add_item(tree, hf_mpeg_descr_service_type, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	descr_len = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_service_provider_name_length, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_service_provider, tvb, offset, descr_len, ENC_ASCII|ENC_NA);
	offset += descr_len;

	name_len = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_service_name_length, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_service_name, tvb, offset, name_len, ENC_ASCII|ENC_NA);

}
