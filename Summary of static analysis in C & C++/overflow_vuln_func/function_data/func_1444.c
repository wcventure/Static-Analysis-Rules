static void
proto_mpeg_descriptor_dissect_ca_identifier(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint end = offset + len;

	while (offset < end) {
		proto_tree_add_item(tree, hf_mpeg_descr_ca_identifier_system_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;
	}

}
