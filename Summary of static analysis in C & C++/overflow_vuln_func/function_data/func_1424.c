static void
proto_mpeg_descriptor_dissect_system_clock(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_system_clock_external_clock_reference_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_system_clock_reserved1, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_system_clock_accuracy_integer, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_system_clock_accuracy_exponent, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_system_clock_reserved2, tvb, offset, 1, ENC_BIG_ENDIAN);
}
