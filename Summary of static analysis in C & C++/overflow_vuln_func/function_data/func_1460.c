static void
proto_mpeg_descriptor_dissect_rcs_content(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint end = offset + len;

	while (offset < end) {
		proto_tree_add_item(tree, hf_mpeg_descr_rcs_content_table_id, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
	}
}
