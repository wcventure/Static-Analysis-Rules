static void
proto_mpeg_descriptor_dissect_stream_identifier(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_stream_identifier_component_tag, tvb, offset, 1, ENC_BIG_ENDIAN);
}
