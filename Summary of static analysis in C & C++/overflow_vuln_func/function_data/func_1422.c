static void
proto_mpeg_descriptor_dissect_ca(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_ca_system_id, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(tree, hf_mpeg_descr_ca_reserved, tvb, offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_ca_pid, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	if (len > 4)
		proto_tree_add_item(tree, hf_mpeg_descr_ca_private, tvb, offset, len - 4, ENC_NA);
}
