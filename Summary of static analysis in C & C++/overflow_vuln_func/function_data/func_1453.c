static void
proto_mpeg_descriptor_dissect_data_bcast_id(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_data_bcast_id_bcast_id, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	if (len > 2)
		proto_tree_add_item(tree, hf_mpeg_descr_data_bcast_id_id_selector_bytes, tvb, offset, len - 2, ENC_NA);
}
