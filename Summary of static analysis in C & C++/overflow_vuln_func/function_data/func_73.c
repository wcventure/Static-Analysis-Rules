static void
decode_subdevice_state(proto_tree *tree _U_, tvbuff_t *tvb,
		       packet_info *pinfo _U_, guint offset, guint length _U_,
		       guint8 opcode _U_, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	int i;

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	for(i = 0; i <= 7; i++) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Subdevice %d State: %d",
			i, (tvb_get_guint8(tvb, offset) & 0x0F));
		i++;
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Subdevice %d State: %d",
			i, ((tvb_get_guint8(tvb, offset) & 0xF0) >> 4));
		offset++;
	}
}
