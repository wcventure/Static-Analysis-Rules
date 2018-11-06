static void
proto_mpeg_descriptor_dissect_std(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_std_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_std_leak_valid, tvb, offset, 1, ENC_BIG_ENDIAN);
}
