static void
decode_feedback(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
		guint offset, guint length, guint8 opcode _U_,
		proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	guint8 command = tvb_get_guint8(tvb, offset);
	proto_tree *ua3g_body_tree;

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
			"Level: %d dB",
			(signed char)(tvb_get_guint8(tvb, offset) / 2));
		offset++;
		length--;

		if(length > 0) {
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Duration: %d ms",
				(tvb_get_guint8(tvb, offset) * 10));
		}
	}
}
