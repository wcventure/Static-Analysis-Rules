static void
decode_subdevice_escape(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo _U_,
			guint offset, guint length, guint8 opcode _U_,
			proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	int j = 0;

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Subdevice Address: %d", (tvb_get_guint8(tvb, offset) & 0x0F));
	offset++;
	length--;

	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Subdevice Opcode: 0x%02x", (tvb_get_guint8(tvb, offset) & 0x7F));
	offset++;
	length--;

	while(length > 0) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Parameter Byte %2d: %d", j++,
			tvb_get_guint8(tvb, offset));
		offset++;
		length--;
	}
}
