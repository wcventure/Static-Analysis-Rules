static void
decode_voice_channel(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo _U_,
		     guint offset, guint length, guint8 opcode _U_,
		     proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	static const value_string str_v[] = {
		{0x00, "Normal Voice Channel Mode"},
		{0x01, "Write 00 to Voice Channel"},
		{0, NULL}
	};
	static const value_string str_c[] = {
		{0x00, "Normal Codec Operation"},
		{0x02, "Write Quiet To Codec"},
		{0, NULL}
	};
	static const value_string str_b[] = {
		{0x00, "Use B1 As Voice Channel"},
		{0x04, "Use B3 As Voice Channel"},
		{0, NULL}
	};
	static const value_string str_voice_channel[] = {
		{0x00, "No"},
		{0x01, "B1"},
		{0x02, "B2"},
		{0x03, "B3"},
		{0, NULL}
	};

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	if(length == 1) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s",
			val_to_str((tvb_get_guint8(tvb, offset) & 0x01), str_v, "Unknown"));
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s",
			val_to_str((tvb_get_guint8(tvb, offset) & 0x02), str_c, "Unknown"));
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s",
			val_to_str((tvb_get_guint8(tvb, offset) & 0x04), str_b, "Unknown"));
		offset++;
		length--;
	} else if(length == 2) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Main Voice: %s",
			val_to_str(tvb_get_guint8(tvb, offset), str_voice_channel, "Unknown"));
		offset++;
		length--;
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Announce: %s",
			val_to_str(tvb_get_guint8(tvb, offset), str_voice_channel, "Unknown"));
		offset++;
		length--;
	} else if(length == 4) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "B General: %d",
			tvb_get_guint8(tvb, offset));
		offset++;
		length--;
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "B Loud Speaker: %d",
			tvb_get_guint8(tvb, offset));
		offset++;
		length--;
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "B Ear Piece: %d",
			tvb_get_guint8(tvb, offset));
		offset++;
		length--;
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "B Microphones: %d",
			tvb_get_guint8(tvb, offset));
		offset++;
		length--;
	}
}
