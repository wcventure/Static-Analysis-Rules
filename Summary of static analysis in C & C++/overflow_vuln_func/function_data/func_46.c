static void
decode_dwl_dtmf_clck_format(proto_tree *tree _U_, tvbuff_t *tvb,
			    packet_info *pinfo _U_, guint offset, guint length,
			    guint8 opcode _U_, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	static const value_string str_clock_format[] = {
		{0, "Europe"},
		{1, "US"},
		{0, NULL}
	};

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Minimum 'ON' Time: %d ms", (tvb_get_guint8(tvb, offset) * 10));
	offset++;
	length--;
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Inter-Digit Pause Time: %d ms",
		(tvb_get_guint8(tvb, offset) * 10));
	offset++;
	length--;
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Clock Time Format: %s",
		val_to_str(tvb_get_guint8(tvb, offset), str_clock_format, "Unknown"));
	offset++;
	length--;

	if(length > 0)
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"DTMF Country Adaptation: %d", tvb_get_guint8(tvb, offset));
}
