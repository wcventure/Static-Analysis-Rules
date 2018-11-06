static void
decode_digit_dialed(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo _U_,
		    guint offset, guint length _U_, guint8 opcode _U_,
		    proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	static const value_string str_digit[] = {
		{0, "0"},
		{1, "1"},
		{2, "2"},
		{3, "3"},
		{4, "4"},
		{5, "5"},
		{6, "6"},
		{7, "7"},
		{8, "8"},
		{9, "9"},
		{10, "*"},
		{11, "#"},
		{12, "A"},
		{13, "B"},
		{14, "C"},
		{15, "D"},
		{16, "Flash"},
		{0, NULL}
	};

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Digit Value: %s",
		val_to_str(tvb_get_guint8(tvb, offset), str_digit, "Unknown"));
}
