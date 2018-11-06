static void
decode_mute(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
	    guint offset, guint length _U_, guint8 opcode _U_,
	    proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	guint8 command = tvb_get_guint8(tvb, offset);
	proto_tree *ua3g_body_tree;
	static const value_string str_mute[] = {
		{0x00, "Microphone Disable"},
		{0x01, "Microphone Enable"},
		{0, NULL}
	};

	/
	proto_item_append_text(ua3g_item, ", %s",
		val_to_str(command, str_mute, "Unknown"));
	proto_item_append_text(ua3g_body_item, " - %s",
		val_to_str(command, str_mute, "Unknown"));
	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	/
	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_fstr(pinfo->cinfo, COL_INFO, ": %s",
			val_to_str(command, str_mute, "Unknown"));

	proto_tree_add_uint_format(ua3g_body_tree, hf_ua3g_command, tvb, offset,
		1, command, "%s",
		val_to_str(command, str_mute, "Unknown"));
}
