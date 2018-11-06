static void
decode_external_ringing(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
			guint offset, guint length _U_, guint8 opcode _U_,
			proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	static const value_string str_ext_ring_cmd[] = {
		{0x00, "Turn Off"},
		{0x01, "Turn On"},
		{0x02, "Follow The Normal Ringing"},
		{0, NULL}
	};

	/
	proto_item_append_text(ua3g_item, ", %s",
		val_to_str(tvb_get_guint8(tvb, offset), str_ext_ring_cmd, "Unknown"));
	proto_item_append_text(ua3g_body_item, " - %s",
		val_to_str(tvb_get_guint8(tvb, offset), str_ext_ring_cmd, "Unknown"));
	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	/
	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_fstr(pinfo->cinfo, COL_INFO, ": %s",
			val_to_str(tvb_get_guint8(tvb, offset), str_ext_ring_cmd, "Unknown"));

	proto_tree_add_uint_format(ua3g_body_tree, hf_ua3g_command, tvb, offset, 1,
		tvb_get_guint8(tvb, offset), "External Ringing Command: %s",
		val_to_str(tvb_get_guint8(tvb, offset), str_ext_ring_cmd, "Unknown"));
}
