static void
proto_mpeg_descriptor_dissect_smoothing_buffer(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	proto_item *leak_rate_item;

	guint32 leak_rate;

	proto_tree_add_item(tree, hf_mpeg_descr_smoothing_buffer_reserved1, tvb, offset, 3, ENC_BIG_ENDIAN);
	leak_rate = tvb_get_ntoh24(tvb, offset) & MPEG_DESCR_SMOOTHING_BUFFER_LEAK_RATE_MASK;
	leak_rate_item = proto_tree_add_item(tree, hf_mpeg_descr_smoothing_buffer_leak_rate, tvb, offset, 3, ENC_BIG_ENDIAN);
	proto_item_append_text(leak_rate_item, " (%u bytes/sec)", leak_rate * 400 / 8);
	offset += 3;

	proto_tree_add_item(tree, hf_mpeg_descr_smoothing_buffer_reserved2, tvb, offset, 3, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_smoothing_buffer_size, tvb, offset, 3, ENC_BIG_ENDIAN);

}
