static void
decode_set_clck_timer_pos(proto_tree *tree _U_, tvbuff_t *tvb,
			  packet_info *pinfo _U_, guint offset, guint length,
			  guint8 opcode _U_, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Clock Line Number: %d", tvb_get_guint8(tvb, offset));
	offset++;
	length--;
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Clock Column Number: %d", tvb_get_guint8(tvb, offset));
	offset++;
	length--;
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Call Timer Line Number: %d", tvb_get_guint8(tvb, offset));
	offset++;
	length--;
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Call Timer Column Number: %d", tvb_get_guint8(tvb, offset));
}
