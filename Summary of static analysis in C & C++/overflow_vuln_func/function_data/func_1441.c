static void
proto_mpeg_descriptor_dissect_extended_event(tvbuff_t *tvb, guint offset, proto_tree *tree)
{

	guint8 items_len, item_descr_len, item_len, text_len;
	guint  items_end;

	proto_item *ii;
	proto_tree *item_tree;

	proto_tree_add_item(tree, hf_mpeg_descr_extended_event_descriptor_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_extended_event_last_descriptor_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_extended_event_lang_code, tvb, offset, 3, ENC_ASCII|ENC_NA);
	offset += 3;

	items_len = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_extended_event_length_of_items, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	items_end = offset + items_len;

	while (offset < items_end) {
		ii = proto_tree_add_text(tree, tvb, offset, 0, "Item");
		item_tree = proto_item_add_subtree(ii, ett_mpeg_descriptor_extended_event_item);

		item_descr_len = tvb_get_guint8(tvb, offset);
		proto_tree_add_item(item_tree, hf_mpeg_descr_extended_event_item_description_length, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(item_tree, hf_mpeg_descr_extended_event_item_description_char, tvb, offset, item_descr_len, ENC_ASCII|ENC_NA);
		offset += item_descr_len;

		item_len = tvb_get_guint8(tvb, offset);
		proto_tree_add_item(item_tree, hf_mpeg_descr_extended_event_item_length, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(item_tree, hf_mpeg_descr_extended_event_item_char, tvb, offset, item_len, ENC_ASCII|ENC_NA);
		offset += item_len;
	}

	text_len = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_extended_event_text_length, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_extended_event_text_char, tvb, offset, text_len, ENC_ASCII|ENC_NA);

}
