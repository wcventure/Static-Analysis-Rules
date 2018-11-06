static void
proto_mpeg_descriptor_stuffing(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_stuffing, tvb, offset, len, ENC_NA);
}
