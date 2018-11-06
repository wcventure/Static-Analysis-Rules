static void
decode_subdevice_metastate(proto_tree *tree _U_, tvbuff_t *tvb,
			   packet_info *pinfo _U_, guint offset, guint length,
			   guint8 opcode _U_, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	static const value_string str_new_metastate[] = {
		{0x00, "Disable"},
		{0x01, "Active"},
		{0x02, "Wake Up"},
		{0, NULL}
	};

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Subchannel Address: %d", tvb_get_guint8(tvb, offset));
	offset++;
	length--;
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "New Metastate: %s",
		val_to_str(tvb_get_guint8(tvb, offset), str_new_metastate, "Unknown"));
}
