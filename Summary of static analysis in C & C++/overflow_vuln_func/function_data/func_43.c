static void
decode_lcd_line_cmd(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
		    guint offset, guint length, guint8 opcode _U_,
		    proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	guint8 lcd_options, command, column_n;
	guint i;
	proto_tree *ua3g_body_tree;
	proto_item *ua3g_param_item;
	proto_tree *ua3g_param_tree;
	proto_item *ua3g_option_item;
	proto_tree *ua3g_option_tree;
	static char str_ascii[40];
/
	static const value_string str_command[] = {
		{0, "Clear Line & Write From Column"},
		{1, "Write From Column"},
		{2, "Append To Current Line"},
		{0, NULL}
	};
	static const value_string str_enable_disable[] = {
		{0, "Disable"},
		{0, NULL}
	};
	static const value_string str_lcd_option[] = {
		{7, "Suspend Display Refresh"},
		{6, "Time Of Day Display    "},
		{5, "Call Timer Display     "},
		{4, "Call Timer Control     "},
		{2, "Blink                  "},
		{0, NULL}
	};
	static const value_string str_call_timer_ctrl[] = {
		{0x00, "Call Timer Status Not Changed"},
		{0x08, "Stop Call Timer"},
		{0x10, "Start Call Timer From Current Value"},
		{0x18, "Initialize And Call Timer"},
		{0, NULL}
	};
	emem_strbuf_t *strbuf = ep_strbuf_new_label("");

	lcd_options = tvb_get_guint8(tvb, offset) & 0xFC;
	command = tvb_get_guint8(tvb, offset) & 0x03;
	column_n = tvb_get_guint8(tvb, offset + 1);

	/
	proto_item_append_text(ua3g_item, ", %s %d",
		val_to_str(command, str_command, "Unknown"),
		column_n);
	proto_item_append_text(ua3g_body_item, " %s %d",
		val_to_str(command, str_command, "Unknown"),
		column_n);
	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	/
	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_fstr(pinfo->cinfo, COL_INFO, ": %s %d",
			val_to_str(command, str_command, "Unknown"),
			column_n);

	ep_strbuf_truncate(strbuf, 0);
	ep_strbuf_append(strbuf, "\"");
	for(i = 0; i < length - 2; i++) {
		if(isprint(tvb_get_guint8(tvb, offset + 2 + i)))
			ep_strbuf_append_printf(strbuf, "%c", tvb_get_guint8(tvb, offset + 2 + i));
		else
			ep_strbuf_append_printf(strbuf, "'0x%02x'", tvb_get_guint8(tvb, offset + 2 + i));
	}
	ep_strbuf_append(strbuf, "\"");

	ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
		length, "%s %d: %s",
		val_to_str(command, str_command, "Unknown"), column_n, str_ascii);
	ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);

	proto_tree_add_uint_format(ua3g_body_tree, hf_ua3g_command, tvb, offset,
		1, command, "Command: %s",
		val_to_str(command, str_command, "Unknown"));
	ua3g_option_item = proto_tree_add_text(ua3g_param_tree, tvb, offset,
		1, "LCD Options: 0x%x", lcd_options);
	ua3g_option_tree = proto_item_add_subtree(ua3g_option_item, ett_ua3g_option);

	for(i = 2; i <= 7; i++) {
		int shift_value = 1 << i;
		if(i != 3) {
			proto_tree_add_text(ua3g_option_tree, tvb, offset, 1, "%s: %s",
				val_to_str(i, str_lcd_option, "Unknown"),
				val_to_str((tvb_get_guint8(tvb, offset) & shift_value), str_enable_disable, "Enable"));
		} else {
			i++;
			proto_tree_add_text(ua3g_option_tree, tvb, offset, 1, "%s: %s",
				val_to_str(i, str_lcd_option, "Unknown"),
				val_to_str((tvb_get_guint8(tvb, offset) & 0x18), str_call_timer_ctrl, "Unknown"));
		}
	}
	offset++;
	length--;

	if(command != 3)
		proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Starting Column: %d", column_n);
	else
		proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Unused");

	offset++;
	length--;
	proto_tree_add_text(ua3g_param_tree, tvb, offset, length, "ASCII Char: %s", str_ascii);
}
