static void
proto_mpeg_descriptor_dissect_app_sig(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint  offset_start;

	offset_start = offset;
	while (offset-offset_start < len) {
		proto_tree_add_item(tree, hf_mpeg_descr_app_sig_app_type, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;
		proto_tree_add_item(tree, hf_mpeg_descr_app_sig_ait_ver, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset++;
	}
}
