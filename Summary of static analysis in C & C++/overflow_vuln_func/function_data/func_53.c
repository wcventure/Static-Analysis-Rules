static void
decode_set_lcd_contrast(proto_tree *tree _U_, tvbuff_t *tvb,
			packet_info *pinfo _U_, guint offset, guint length,
			guint8 opcode _U_, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	static const value_string str_driver_number[] = {
		{0x00, "Display"},
		{0x01, "Icon"},
		{0, NULL}
	};

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Driver Number: %s",
		val_to_str(tvb_get_guint8(tvb, offset), str_driver_number, "Unknown"));
	offset++;
	length--;
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Contrast Value: %d", tvb_get_guint8(tvb, offset));
}
