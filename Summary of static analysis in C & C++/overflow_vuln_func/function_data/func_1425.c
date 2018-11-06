static void
proto_mpeg_descriptor_dissect_max_bitrate(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	proto_item *rate_item;

	guint32 rate;

	proto_tree_add_item(tree, hf_mpeg_descr_max_bitrate_reserved, tvb, offset, 3, ENC_BIG_ENDIAN);
	rate = tvb_get_ntoh24(tvb, offset) & MPEG_DESCR_MAX_BITRATE_MASK;
	rate_item = proto_tree_add_item(tree, hf_mpeg_descr_max_bitrate, tvb, offset, 3, ENC_BIG_ENDIAN);
	proto_item_append_text(rate_item, " (%u bytes/sec)", rate * 50);
}
