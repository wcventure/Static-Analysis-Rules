static void
decode_set_clck(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
		guint offset, guint length, guint8 opcode _U_,
		proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	guint8 command = tvb_get_guint8(tvb, offset);
	proto_tree *ua3g_body_tree;
	proto_item *ua3g_param_item;
	proto_tree *ua3g_param_tree;
	int hour, minute, second, call_timer;
	static const value_string str_command[] = {
		{0x00, "Set Current Time/Call Timer"},
		{0x01, "Set Current Time"},
		{0x02, "Set Call Timer"},
		{0, NULL}
	};

	/
	proto_item_append_text(ua3g_item, ", %s",
		val_to_str(command, str_command, "Unknown"));
	proto_item_append_text(ua3g_body_item, " - %s",
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
	call_timer = 0;

	switch(command) {
	case 0x02: /
		{
			call_timer = 1;
		}
		/
	case 0x00: /
	case 0x01: /
		{
			static const value_string str_call_timer[] = {
				{1, "Call Timer "},
				{0, NULL}
			};

			while(length > 0) {
				hour = tvb_get_guint8(tvb, offset);
				minute = tvb_get_guint8(tvb, offset + 1);
				second = tvb_get_guint8(tvb, offset + 2);

				ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset, 3,
					"%s: %d:%d:%d",
					val_to_str(call_timer, str_call_timer, "Current Time"), hour, minute, second);
				ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);
				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "%sHour: %d",
					val_to_str(call_timer, str_call_timer, ""), hour);
				offset++;
				length--;
				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "%sMinute: %d",
					val_to_str(call_timer, str_call_timer, ""), minute);
				offset++;
				length--;
				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "%sSecond: %d",
					val_to_str(call_timer, str_call_timer, ""), second);
				offset++;
				length--;

				call_timer = 1;
			}
		}
	default:
		{
			break;
		}
	}

}
