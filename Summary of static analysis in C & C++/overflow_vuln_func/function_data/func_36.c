static void
decode_software_reset(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo _U_,
		      guint offset, guint length, guint8 opcode _U_,
		      proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	static const value_string str_verswitch[] = {
		{0x00, "Reset Without Version Switch"},
		{0x01, "Reset With Version Switch"},
		{0, NULL}
	};

	if(length > 0) {
		ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s",
			val_to_str(tvb_get_guint8(tvb, offset), str_verswitch, "Unknown"));
		offset++;
		length--;
	}
}
