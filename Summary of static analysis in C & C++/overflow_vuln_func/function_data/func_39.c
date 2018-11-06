static void
decode_segment_msg(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo _U_,
		   guint offset, guint length, guint8 opcode _U_,
		   proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	int first_sub_segment;
	int j = 0;
	static const value_string str_first_sub_segment[] = {
		{0x00, "Subsequent Segment"},
		{0x80, "Fisrt Segment"},
		{0, NULL}
	};

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
	first_sub_segment = (tvb_get_guint8(tvb, offset) & 0x80);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "F/S: %s (%d)",
		val_to_str(first_sub_segment, str_first_sub_segment, "Unknown"),
		first_sub_segment);
	proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
		"Number Of Remaining Segments: %d",
		(tvb_get_guint8(tvb, offset) & 0x7F));
	offset++;
	length--;

	if(first_sub_segment == 0x80) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 2, "Length: %d",
			tvb_get_ntohs(tvb, offset));
		offset += 2;
		length -= 2;
	}

	while(length > 0) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Segment Message byte %d: %d",
			j++, tvb_get_guint8(tvb, offset));
		offset++;
		length--;
	}
}
