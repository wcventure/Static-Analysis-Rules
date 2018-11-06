static void
decode_ringing_cadence(proto_tree *tree _U_, tvbuff_t *tvb,
		       packet_info *pinfo _U_, guint offset, guint length,
		       guint8 opcode _U_, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	int i = 0;

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Cadence: %d",
		tvb_get_guint8(tvb, offset));
	offset++;
	length--;

	while(length > 0) {
		i++;
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "On / Off  : %s",
			val_to_str((tvb_get_guint8(tvb, offset) & 0x80), str_on_off, "On"));
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Length %d : %d ms",
			i, ((tvb_get_guint8(tvb, offset) & 0x7F) * 10));
		offset++;
		length--;
	}
}
