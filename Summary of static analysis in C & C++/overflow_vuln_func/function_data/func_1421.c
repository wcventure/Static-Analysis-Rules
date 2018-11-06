static void
proto_mpeg_descriptor_dissect_data_stream_alignment(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_data_stream_alignment, tvb, offset, 1, ENC_BIG_ENDIAN);
}
