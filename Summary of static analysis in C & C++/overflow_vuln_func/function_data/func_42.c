static void
decode_led_command(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
		   guint offset, guint length, guint8 opcode _U_,
		   proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	int command = tvb_get_guint8(tvb, offset);
	static const value_string str_command[] = {
		{0x00, "Led Off"},
		{0x01, "Led On"},
		{0x02, "Red Led Fast Flash"},
		{0x03, "Red Led Slow Flash"},
		{0x04, "Green Led On"},
		{0x05, "Green Led Fast Flash"},
		{0x06, "Green Led Slow Flash"},
		{0x07, "All Led Off"},
		{0, NULL}
	};

	/
	proto_item_append_text(ua3g_item, ", %s",
		val_to_str(command, str_command, "Unknown"));
	proto_item_append_text(ua3g_body_item, "s - %s",
		val_to_str(command, str_command, "Unknown"));
	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	/
	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_fstr(pinfo->cinfo, COL_INFO, ": %s",
			val_to_str(command, str_command, "Unknown"));

	proto_tree_add_uint_format(ua3g_body_tree, hf_ua3g_command, tvb, offset,
		1, command, "Command: %s",
		val_to_str(command, str_command, "Unknown"));
	offset++;
	length--;

	if(command != 0x07) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Led Number: %d", tvb_get_guint8(tvb, offset));
	}
}
