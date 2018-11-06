static void
proto_mpeg_descriptor_dissect_association_tag(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint   end = offset + len;
	guint16 use;
	guint8  selector_len;

	proto_tree_add_item(tree, hf_mpeg_descr_association_tag, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	use = tvb_get_ntohs(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_association_tag_use, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	selector_len = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_association_tag_selector_len, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset ++;

	if (use == 0x00) {
		if (selector_len != 8)
			return;
		proto_tree_add_item(tree, hf_mpeg_descr_association_tag_transaction_id, tvb, offset, 4, ENC_BIG_ENDIAN);
		offset += 4;

		proto_tree_add_item(tree, hf_mpeg_descr_association_tag_timeout, tvb, offset, 4, ENC_BIG_ENDIAN);
		offset += 4;

	} else {
		proto_tree_add_item(tree, hf_mpeg_descr_association_tag_selector_bytes, tvb, offset, selector_len, ENC_NA);
		offset += selector_len;
	}

	if (offset < end)
		proto_tree_add_item(tree, hf_mpeg_descr_association_tag_private_bytes, tvb, offset, end - offset, ENC_NA);
}
