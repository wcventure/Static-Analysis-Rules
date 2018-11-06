static void
decode_i_m_here(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo _U_,
		guint offset, guint length _U_, guint8 opcode _U_,
		proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Id Code: %s",
		val_to_str(tvb_get_guint8(tvb, offset), str_device_type, "Unknown"));
}
