static void
decode_icon_cmd(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo _U_,
		guint offset, guint length, guint8 opcode _U_,
		proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	int i;
	static const value_string str_state[] = {
		{0x00, "Off"},
		{0x01, "Slow Flash"},
		{0x02, "Not Used"},
		{0x03, "Steady On"},
		{0, NULL}
	};

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Icon Number: %d", tvb_get_guint8(tvb, offset));
	offset++;
	length--;

	for(i = 0; i < 8; i++) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 2,
			"Segment %d: %s (%d)", i,
			val_to_str((((tvb_get_guint8(tvb, offset) << 1) >> i) & 0x02) + ((tvb_get_guint8(tvb, offset + 1) >> i) & 0x01),str_state, "Unknown"),
			(((tvb_get_guint8(tvb, offset) << 1) >> i) & 0x02) + ((tvb_get_guint8(tvb, offset + 1) >> i) & 0x01));
	}
}
