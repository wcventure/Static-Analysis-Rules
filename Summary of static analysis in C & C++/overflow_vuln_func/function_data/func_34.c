static void
decode_with_one_parameter(proto_tree *tree _U_, tvbuff_t *tvb,
			  packet_info *pinfo _U_, guint offset, guint length,
			  guint8 opcode, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	static const value_string str_first_parameter[] = {
		{0x01, "Production Test Command"},
		{0x06, "Reserved For Compatibility"},
		{0x2B, "Temporization"},
		{0x3B, "Volmue"},
		{0x42, "Subdevice Address"},
		{0, NULL}
	};

	if(length > 0) {
		ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
		proto_tree_add_text(ua3g_body_tree, tvb, offset, length, "%s: %d",
			val_to_str(opcode, str_first_parameter, "Unknown"), tvb_get_guint8(tvb, offset));
		length--;
	}
}
