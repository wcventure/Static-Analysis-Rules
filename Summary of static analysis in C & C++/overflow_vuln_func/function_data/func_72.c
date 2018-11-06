static void
decode_special_key(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo _U_,
		   guint offset, guint length, guint8 opcode,
		   proto_item *ua3g_body_item)
{
/
	proto_tree *ua3g_body_tree;
	int i;
	static const value_string str_parameters[] = {
		{0x00, "Not Received Default In Effect"},
		{0x02, "Downloaded Values In Effect"},
		{0, NULL}
	};
	static const value_string str_special_key_status[] = {
		{0x00, "Released"},
		{0, NULL}
	};
	static const value_string str_special_key[] = {
		{0, "Shift "},
		{1, "Ctrl  "},
		{2, "Alt   "},
		{3, "Cmd   "},
		{4, "Shift'"},
		{5, "Ctrl' "},
		{6, "Alt'  "},
		{7, "Cmd'  "},
		{0, NULL}
	};

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	if(opcode == 0x23) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Parameters Received for DTMF: %s",
			val_to_str((tvb_get_guint8(tvb, offset) & 0x02), str_parameters, "Unknown"));
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Hookswitch Status: %shook",
			val_to_str((tvb_get_guint8(tvb, offset) & 0x01), str_on_off, "On"));
		offset++;
		length--;
	}

	for(i = 0; i < 8; i++) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s: %s",
			val_to_str(i, str_special_key, "Unknown"),
			val_to_str((tvb_get_guint8(tvb, offset) & (0x01 << i)),
			str_special_key_status, "Pressed"));
	}
}
