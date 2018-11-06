static void
decode_r_w_peripheral(proto_tree *tree _U_, tvbuff_t *tvb,
		      packet_info *pinfo _U_, guint offset, guint length,
		      guint8 opcode _U_, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 2, "Address: %d",
		tvb_get_ntohs(tvb, offset));
	offset += 2;
	length -= 2;

	if(length > 0) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Content: %d", tvb_get_guint8(tvb, offset));
	}
}
