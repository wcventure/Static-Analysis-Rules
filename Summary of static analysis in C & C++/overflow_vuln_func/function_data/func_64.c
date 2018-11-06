static void
decode_ring(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
	    guint offset, guint length, guint8 opcode _U_,
	    proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	guint8 command = tvb_get_guint8(tvb, offset);
	proto_tree *ua3g_body_tree;
	static const value_string str_cadence[] = {
		{0x00, "Standard Ringing"},
		{0x01, "Double Burst"},
		{0x02, "Triple Burst"},
		{0x03, "Continuous"},
		{0x04, "Priority Attendant Ringing"},
		{0x05, "Regular Attendant Ringing"},
		{0x06, "Programmable Cadence"},
		{0, NULL}
	};

	/
	proto_item_append_text(ua3g_item, ", %s",
		val_to_str(command, str_on_off, "On"));
	proto_item_append_text(ua3g_body_item, " - %s",
		val_to_str(command, str_on_off, "On"));
	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	/
	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_fstr(pinfo->cinfo, COL_INFO, ": %s",
			val_to_str(command, str_on_off, "On"));

	proto_tree_add_uint_format(ua3g_body_tree, hf_ua3g_command, tvb, offset,
		1, command, "Command: %s",
		val_to_str(command, str_on_off, "On"));
	offset++;
	length--;

	if(command == 0x01) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Melody: %d", tvb_get_guint8(tvb, offset));
		offset++;
		length--;
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Cadence: %s",
			val_to_str(tvb_get_guint8(tvb, offset), str_cadence, "Unknown"));
		offset++;
		length--;
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Speaker level: %d dB",
			(signed char)(tvb_get_guint8(tvb, offset)));
		offset++;
		length--;
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Beep number: %d", tvb_get_guint8(tvb, offset));
		offset++;
		length--;
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Silent: %s",
			val_to_str((tvb_get_guint8(tvb, offset) & 0x80), str_on_off, "On"));
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Progressive: %d",
			(tvb_get_guint8(tvb, offset) & 0x03));
	}
}
