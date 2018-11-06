static void
decode_super_msg(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo _U_,
		 guint offset, guint length, guint8 opcode,
		 proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	int i, parameter_length;
	int j = 0;

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	while(length > 0) {
		if(opcode == 0x17) {
			parameter_length = tvb_get_ntohs(tvb, offset);
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 2,
				"Length %d: %d", j++, parameter_length);
			offset += 2;
			length -= 2;
		} else {
			parameter_length = tvb_get_guint8(tvb, offset);
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Length %d: %d", j++, parameter_length);
			offset++;
			length--;
		}

		for(i = 1; i <= parameter_length; i++) {
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"L%d Byte %2d: %d",
				j, i, tvb_get_guint8(tvb, offset));
			offset++;
			length--;
		}
	}
}
