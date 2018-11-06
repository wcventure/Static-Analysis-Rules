static void
proto_mpeg_descriptor_dissect_private_data_specifier(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_private_data_specifier_id, tvb, offset, 4, ENC_BIG_ENDIAN);
}
