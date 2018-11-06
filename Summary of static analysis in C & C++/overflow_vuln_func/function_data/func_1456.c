static void
proto_mpeg_descriptor_dissect_default_authority(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_default_authority_name, tvb, offset, len, ENC_ASCII|ENC_NA);
}
