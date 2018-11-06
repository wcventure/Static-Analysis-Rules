static void
decode_main_voice_mode(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
		       guint offset, guint length, guint8 opcode _U_,
		       proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	guint8 mode = tvb_get_guint8(tvb, offset);
	proto_tree *ua3g_body_tree;
	static const value_string str_voice_mode[] = {
		{0x00, "Idle"},
		{0x01, "Handset"},
		{0x02, "Group Listening"},
		{0x03, "On Hook Dial"},
		{0x04, "Handsfree"},
		{0x05, "Announce Loudspeaker"},
		{0x06, "Ringing"},
		{0x10, "Idle"},
		{0x11, "Handset"},
		{0x12, "Headset"},
		{0x13, "Handsfree"},
		{0, NULL}
	};

	/
	proto_item_append_text(ua3g_item, ", %s",
		val_to_str(mode, str_voice_mode, "Unknown"));
	proto_item_append_text(ua3g_body_item, " - %s",
		val_to_str(mode, str_voice_mode, "Unknown"));
	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	/
	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_fstr(pinfo->cinfo, COL_INFO, ": %s",
			val_to_str(mode, str_voice_mode, "Unknown"));

	proto_tree_add_uint_format(ua3g_body_tree, hf_ua3g_command, tvb, offset,
		1, mode, "Voice Mode: %s",
		val_to_str(mode, str_voice_mode, "Unknown"));
	offset++;
	length--;

	switch(mode) {
	case 0x06: /
		{
			static const value_string str_cadence[] = {
				{0x00, "Standard Ringing"},
				{0x01, "Double Burst"},
				{0x02, "Triple Burst"},
				{0x03, "Continuous Ringing"},
				{0x04, "Priority Attendant Ringing"},
				{0x05, "Regular Attendant Ringing"},
				{0x06, "Programmable Cadence"},
				{0x07, "Programmable Cadence"},
				{0, NULL}
			};

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Tune: %d", tvb_get_guint8(tvb, offset));
			offset++;
			length--;

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Cadence: %s (%d)",
				val_to_str(tvb_get_guint8(tvb, offset), str_cadence, "Unknown"),
				tvb_get_guint8(tvb, offset));
			offset++;
			length--;
		}
		/
	case 0x02: /
	case 0x03: /
	case 0x04: /
	case 0x05: /
		{
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Speaker Volume: %d",
				tvb_get_guint8(tvb, offset));
			offset++;
			length--;

			if(length > 0) {
				proto_tree_add_text(ua3g_body_tree, tvb, offset,
					1, "Microphone Volume: %d",
					tvb_get_guint8(tvb, offset));
			}
			break;
		}
	case 0x11: /
	case 0x12: /
	case 0x13: /
		{
			signed char level;
			static const value_string str_receiving_level[] = {
				{0x11, "Receiving Level "},
				{0x12, "Receiving Level "},
				{0x13, "Speaker Level   "},
				{0, NULL}
			};

			level = (signed char)(tvb_get_guint8(tvb, offset)) / 2;
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"%s: %d dB",
				val_to_str(mode, str_receiving_level, "Unknown"), level);
			offset++;
			length--;

			level = (signed char)(tvb_get_guint8(tvb, offset)) / 2;
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Sending Level:  %d dB", level);
			break;
		}
	case 0x00: /
	case 0x01: /
	case 0x10: /
	default:
		{
			break;
		}
	}
}
